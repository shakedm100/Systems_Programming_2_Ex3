#include "TurnController.hpp"
#include <sstream>

TurnController::TurnController(Game& game, sf::Font& font, sf::RenderWindow& wnd)
            : game(game), font(font), wnd(wnd)
{
    // prepare action buttons
    float x = 20.f, y = 20.f;
    for (const auto& name : game.getActionNames())
    {
        sf::RectangleShape b({120.f, 40.f});
        b.setPosition(x, y);
        b.setFillColor({100,100,200});
        actions.push_back(name);
        btns.push_back(b);
        sf::Text t(name, font, 25);
        t.setFillColor(sf::Color::White);
        centerTextIn(b, t);
        btnLabels.push_back(t);
        x += 140.f;
    }
    // current-player label
    currentPlayer.setFont(font);
    currentPlayer.setCharacterSize(28);
    currentPlayer.setFillColor(sf::Color::White);

    // coin label
    coinLabel.setFont(font);
    coinLabel.setCharacterSize(28);
    coinLabel.setFillColor(sf::Color::Yellow);
    coinLabel.setPosition(20.f, wnd.getSize().y - 40.f);

    // Role label
    roleLabel.setFont(font);
    roleLabel.setCharacterSize(40);
    roleLabel.setFillColor(sf::Color::White);

    // error label
    errorLabel.setFont(font);
    errorLabel.setCharacterSize(40);
    errorLabel.setFillColor(sf::Color::Red);
    errorLabel.setString("");
    centerOrigin(errorLabel);
    errorLabel.setPosition(wnd.getSize().x/2.f, wnd.getSize().y/2.f);

    playerStatusLabels.resize(MAX_PLAYERS);
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        auto& lbl = playerStatusLabels[i];
        lbl.setFont(font);
        lbl.setCharacterSize(18);
        lbl.setFillColor(sf::Color::White);
    }

    phase = Phase::StartTurn;
}

void TurnController::handleClick(const sf::Event& evt)
{
    if (evt.type != sf::Event::MouseButtonPressed || evt.mouseButton.button != sf::Mouse::Left)
        return;
    sf::Vector2f m(evt.mouseButton.x, evt.mouseButton.y);
    switch (phase)
    {
        case Phase::ChooseAction:
            for (size_t i=0; i<btns.size(); ++i)
            {
                if (btns[i].getGlobalBounds().contains(m))
                {
                    pendingAction = actions[i];
                    if (game.actionNeedsTarget(pendingAction))
                    {
                        enterTargetMode();
                        phase = Phase::ChooseTarget;
                    }
                    else
                        phase = Phase::ResolveAction;

                    return;
                }
            }
            break;

        case Phase::ChooseTarget:
            for (size_t t=0; t<targetBtns.size(); ++t) {
                if (targetBtns[t].getGlobalBounds().contains(m)) {
                    pendingTarget = otherPlayers[t];
                    phase = Phase::ResolveAction;
                    return;
                }
            }
            break;

        default:
            break;
    }
}

void TurnController::update()
{
    switch (phase)
    {
        case Phase::StartTurn:
            setupForCurrentPlayer();
        phase = Phase::ChooseAction;
        break;
        case Phase::ResolveAction:
            applyPending();
        if (game.checkWinner())
            finishGame();
        else if (game.getCurrentTurn()->getExtraTurns() > 0)
        {
            game.getCurrentTurn()->decreaseExtraTurns();
            phase = Phase::StartTurn;
        }
        else
            phase = Phase::EndTurn;
        break;
        case Phase::EndTurn:
            game.nextTurn();
        phase = Phase::StartTurn;
        break;
        default:
            break;
    }
}

void TurnController::render()
{
    if (errorActive && errorClock.getElapsedTime().asSeconds() > 1.f)
        errorActive = false;

    // draw only; clearing&display done in main
    if (phase==Phase::GameOver)
        wnd.draw(gameOverLabel);
    else
    {
        updateStatusLabels();
        for (auto& l : playerStatusLabels)
            wnd.draw(l);
        for (auto& b:btns)
            wnd.draw(b);
        for (auto& l:btnLabels)
            wnd.draw(l);
        if (phase==Phase::ChooseTarget)
        {
            for (auto& b:targetBtns) wnd.draw(b);
            for (auto& l:targetLbls) wnd.draw(l);
        }
        wnd.draw(currentPlayer);
        wnd.draw(roleLabel);
        wnd.draw(coinLabel);
        if (errorActive) {
            wnd.draw(errorLabel);
        }
        for (int i = 0; i < MAX_PLAYERS; ++i)
            wnd.draw(playerStatusLabels[i]);
    }
}

// helpers
void TurnController::setupForCurrentPlayer()
{
    clearUI();
    // update player name & coins
    auto* p = game.getCurrentTurn();
    currentPlayer.setString(p->getName() + "'s Turn");
    roleLabel.setString("Role: " + game.getCurrentTurn()->getClassName());
    coinLabel.setString("Coins: " + std::to_string(p->getCoins()));

    auto tb = currentPlayer.getLocalBounds();
    currentPlayer.setOrigin(tb.left + tb.width, tb.top + tb.height);
    float marginX = 20.f, marginY = 20.f;
    currentPlayer.setPosition(wnd.getSize().x - marginX, wnd.getSize().y - marginY);

    auto rl = roleLabel.getLocalBounds();
    roleLabel.setOrigin(rl.left + rl.width/2.f, rl.top  + rl.height);
    float marginY2 = 60.f;
    roleLabel.setPosition(wnd.getSize().x/2.f, wnd.getSize().y - marginY2);
}

void TurnController::updateStatusLabels()
{
    auto players = game.getPlayers();  // assume this returns a std::vector<Player*>
    const float rightMargin = 20.f;
    const float lineHeight = 22.f;     // character size + spacing
    const float startY    = 20.f;

    for (int i = 0; i < MAX_PLAYERS; ++i) {
        auto& lbl = playerStatusLabels[i];
        if (i < (int)players.size()) {
            Player* p = players[i];

            // Build the status string
            std::ostringstream ss;
            ss << "-------------------------\n"
               << p->getName() << "\n"
               << p->getClassName() << "\n"
               << (p->getStatus().isAlive ? "Alive" : "Dead") << "\n"
               << "Coins: " << p->getCoins();
            if (p->getStatus().isSanctioned)        // adjust if you have multiple effect types
                ss << "\n[sanctioned]";
            ss << "\n-------------------------\n";
            lbl.setString(ss.str());

            // position: anchored to right
            float x = wnd.getSize().x - rightMargin;
            float y = startY + i * lineHeight * 6; // 4 lines per player
            // to right-align each block, set origin to right edge:
            sf::FloatRect bounds = lbl.getLocalBounds();
            lbl.setOrigin(bounds.width, 0);
            lbl.setPosition(x, y);
        } else {
            // hide unused labels
            lbl.setString("");
        }
    }
}

void TurnController::applyPending() {
    auto* actor = game.getCurrentTurn();
    try
    {
        if (game.canPerform(actor, pendingAction, pendingTarget))
        {
            game.perform(actor, pendingAction, pendingTarget);
            pendingTarget = nullptr;
        }
    }
    catch (const std::exception& e)
    {
        showError(e.what());
        pendingTarget = nullptr;
        centerOrigin(errorLabel);
        game.getCurrentTurn()->increaseExtraTurns();
        phase = Phase::ChooseAction;
        return;
    }
}

void TurnController::showError(const std::string& msg)
{
    errorLabel.setString(msg);
    centerOrigin(errorLabel);    // re-center since string changed
    errorClock.restart();        // start timing now
    errorActive = true;          // turn it on
}

void TurnController::enterTargetMode()
{
    targetBtns.clear(); targetLbls.clear(); otherPlayers.clear();
    float x = 20.f, y = 80.f + 60.f;
    for (auto* p : game.getAlivePlayers()) {
        if (p == game.getCurrentTurn()) continue;
        otherPlayers.push_back(p);
        sf::RectangleShape b({120.f,40.f}); b.setPosition(x,y); b.setFillColor({200,80,80});
        targetBtns.push_back(b);
        sf::Text t(p->getName(), font, 18);
        t.setFillColor(sf::Color::White);
        centerTextIn(b, t);
        targetLbls.push_back(t);
        x += 140.f;
    }
}

void TurnController::clearUI()
{
    targetBtns.clear(); targetLbls.clear(); otherPlayers.clear();
}

void TurnController::centerOrigin(sf::Text& t)
{
    auto b = t.getLocalBounds();
    t.setOrigin(b.left + b.width/2.f, b.top + b.height/2.f);
}

void TurnController::centerTextIn(const sf::RectangleShape& b, sf::Text& t)
{
    auto bb = b.getGlobalBounds();
    auto tb = t.getLocalBounds();
    t.setPosition(
            bb.left + (bb.width - tb.width)/2 - tb.left,
            bb.top  + (bb.height - tb.height)/2 - tb.top
    );
}

void TurnController::finishGame() {
    // clear other UI
    btns.clear(); btnLabels.clear();
    targetBtns.clear(); targetLbls.clear(); otherPlayers.clear();
    // prepare game-over label
    std::string winner = game.getWinner()->getName();
    gameOverLabel.setString("Game Over! Winner: " + winner);
    // adjust line spacing for readability
    gameOverLabel.setLineSpacing(1.2f);
    gameOverLabel.setFont(font);
    // center the origin based on updated bounds
    centerOrigin(gameOverLabel);
    // place at window center
    gameOverLabel.setPosition(wnd.getSize().x/2.f, wnd.getSize().y/2.f);
    phase = Phase::GameOver;
}
