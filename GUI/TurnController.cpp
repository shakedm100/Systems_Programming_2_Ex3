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

    reactionTitle.setFont(font);
    reactionTitle.setString("Reaction Time");
    reactionTitle.setCharacterSize(48);
    reactionTitle.setFillColor(sf::Color::Yellow);
    // center top
    sf::FloatRect rt = reactionTitle.getLocalBounds();
    reactionTitle.setOrigin(rt.width/2, rt.height/2);
    reactionTitle.setPosition(wnd.getSize().x/2.f, 50.f);

    // Reverse button
    btnReverse.setSize({200.f, 60.f});
    btnReverse.setPosition(
        wnd.getSize().x/2.f - btnReverse.getSize().x - 10.f,
        wnd.getSize().y/2.f - btnReverse.getSize().y/2.f
    );
    btnReverse.setFillColor({200,200,50});
    btnReverse.setOutlineColor({100,100,20});
    btnReverse.setOutlineThickness(4.f);

    lblReverse.setFont(font);
    lblReverse.setString("Reverse");
    lblReverse.setCharacterSize(24);
    lblReverse.setFillColor(sf::Color::Black);
    centerLabel(lblReverse, btnReverse);

    // Pass button
    btnPass.setSize({200.f, 60.f});
    btnPass.setPosition(
        wnd.getSize().x/2.f + 10.f,
        wnd.getSize().y/2.f - btnPass.getSize().y/2.f
    );
    btnPass.setFillColor({200,200,50});
    btnPass.setOutlineColor({100,100,20});
    btnPass.setOutlineThickness(4.f);

    lblPass.setFont(font);
    lblPass.setString("Pass");
    lblPass.setCharacterSize(24);
    lblPass.setFillColor(sf::Color::Black);
    centerLabel(lblPass, btnPass);

    phase = Phase::StartTurn;
}

void TurnController::centerLabel(sf::Text& label, const sf::RectangleShape& button)
{
    auto bounds = label.getLocalBounds();
    label.setOrigin(bounds.left + bounds.width/2.f,
                    bounds.top  + bounds.height/2.f);
    sf::Vector2f pos = button.getPosition();
    sf::Vector2f size = button.getSize();
    label.setPosition(pos + size * 0.5f);
}

void TurnController::handleClick(const sf::Event& evt)
{
    if (phase == Phase::ReactionTime && evt.type == sf::Event::MouseButtonPressed && evt.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f m(evt.mouseButton.x, evt.mouseButton.y);
        if (btnReverse.getGlobalBounds().contains(m)) {
            performPendingReverseUI(lblReverse.getString());
            game.clearPendingReverse();
            hideReactionUI();
            phase = Phase::EndTurn;
        }
        else if (btnPass.getGlobalBounds().contains(m)) {
            if (game.advancePendingResponder()) {
                showReactionUI();
            } else {
                if(game.isPendingActionBribe())
                    phase = Phase::ReactionBribe;
                else
                    phase = Phase::EndTurn;
                game.clearPendingReverse();
                hideReactionUI();  // Also hide overlay
            }
        }
        return;
    }

    if (evt.type != sf::Event::MouseButtonPressed || evt.mouseButton.button != sf::Mouse::Left)
        return;
    sf::Vector2f m(evt.mouseButton.x, evt.mouseButton.y);
    switch (phase)
    {
        case Phase::ChooseAction:
            for (size_t i = 0; i < specialBtns.size(); ++i) {
                if (specialBtns[i].getGlobalBounds().contains(m)) {
                    pendingAction = specialActions[i];
                    // role‐specific actions might or might not need a target:
                    if (game.actionNeedsTarget(pendingAction))
                    {;
                        enterTargetMode();
                        phase = Phase::ChooseTarget;
                    } else {
                        phase = Phase::ResolveAction;
                    }
                    return;
                }
            }

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
                    phase = Phase::StartTurn;
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
            else if (game.hasPendingReverse())
                {
                phase = Phase::ReactionTime;
                showReactionUI();
            }
            else if (game.getCurrentTurn()->getExtraTurns() > 0)
            {
                game.getCurrentTurn()->decreaseExtraTurns();
                phase = Phase::StartTurn;
            }
            else
                phase = Phase::EndTurn;
            break;
        case Phase::ReactionBribe:
            if (game.getCurrentTurn()->getExtraTurns() > 0)
            {
                game.getCurrentTurn()->decreaseExtraTurns();
                phase = Phase::StartTurn;
            }
            break;
        case Phase::EndTurn:
            if (game.getCurrentTurn()->getExtraTurns() > 0)
            {
                game.getCurrentTurn()->decreaseExtraTurns();
                phase = Phase::StartTurn;
            }
            else
            {
               finishTurn();
               phase = Phase::StartTurn;
            }
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
    else if (phase == Phase::ReactionTime)
    {
        wnd.draw(reactionTitle);
        wnd.draw(btnReverse);
        wnd.draw(lblReverse);
        wnd.draw(btnPass);
        wnd.draw(lblPass);
        wnd.draw(currentPlayer);
        wnd.draw(roleLabel);
        wnd.draw(coinLabel);
        updateStatusLabels();
    }
    else
    {
        updateStatusLabels();
        for (auto& l : playerStatusLabels)
            wnd.draw(l);
        for (auto& b:btns)
            wnd.draw(b);
        for (auto& l:btnLabels)
            wnd.draw(l);
        for (auto& b : specialBtns)
            wnd.draw(b);
        for (auto& t : specialLabels)
            wnd.draw(t);

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

    auto tb = currentPlayer.getLocalBounds();
    currentPlayer.setOrigin(tb.left + tb.width, tb.top + tb.height);
    float marginX = 20.f, marginY = 20.f;
    currentPlayer.setPosition(wnd.getSize().x - marginX, wnd.getSize().y - marginY);

    auto rl = roleLabel.getLocalBounds();
    roleLabel.setOrigin(rl.left + rl.width/2.f, rl.top  + rl.height);
    float marginY2 = 60.f;
    roleLabel.setPosition(wnd.getSize().x/2.f, wnd.getSize().y - marginY2);

    auto role = p->getClassName();

    if (role == "Spy") {
        specialActions.push_back("Peek");
    }
    else if(role == "Baron")
    {
        specialActions.push_back("Invest");
        if(!game.getCurrentTurn()->getStatus().holdTurn)
        {
            if(game.getCurrentTurn()->getStatus().isInvested)
            {
                game.getCurrentTurn()->investSuccess();
            }
        }
    }

    coinLabel.setString("Coins: " + std::to_string(p->getCoins()));


    // Lay them out down the left side:
    float sx = 20.f;
    float sy = wnd.getSize().y / 1.3f;    // start ~80px up from bottom
    for (auto& name : specialActions) {
        sf::RectangleShape b({200.f, 50.f});
        b.setPosition(sx, sy);
        b.setFillColor({50,180,50});
        specialBtns.push_back(b);

        sf::Text lbl(name, font, 30);
        lbl.setFillColor(sf::Color::White);
        centerTextIn(b, lbl);
        specialLabels.push_back(lbl);

        sy -= 70.f;  // stack upward
    }
}

void TurnController::finishTurn()
{
    game.getCurrentTurn()->clearStatusEffects();
    peekTargets.clear();
    game.nextTurn();
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
               << (p->getStatus().isAlive ? "Alive" : "Dead") << "\n";
            ss << "Coins: " << p->getCoins() << "\n"; // for debugging purposes
            if(!peekTargets.empty() && std::find(peekTargets.begin(), peekTargets.end(), p) != peekTargets.end())
                ss << "Coins: " << p->getCoins() << "\n";
            if (p->getStatus().isSanctioned)        // adjust if you have multiple effect types
                ss << "[Sanctioned]";
            if(p->getStatus().isInvested)
                ss << " [Invested]";
            ss << "\n-------------------------\n";
            lbl.setString(ss.str());

            // position: anchored to right
            float x = wnd.getSize().x - rightMargin;
            float y = startY + i * lineHeight * 6; // About 6 lines per player
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
            if (pendingAction == "Peek")
            {
                peekTargets.push_back(pendingTarget);
                phase           = Phase::ChooseAction;
            }
            game.setupPendingReverse(actor, pendingAction, pendingTarget);
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
    targetBtns.clear();
    targetLbls.clear();
    otherPlayers.clear();
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
    targetBtns.clear();
    targetLbls.clear();
    otherPlayers.clear();
    specialActions.clear();
    specialBtns.clear();
    specialLabels.clear();
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

void TurnController::showReactionUI()
{
    reactionActive = true;
    // set reverse label
    std::string act = game.getPendingActionLabel();
    std::string text = act;
    lblReverse.setString(text);
    centerLabel(lblReverse, btnReverse);

    currentPlayer.setString(game.getCurrentTurn()->getName() + "'s Turn");
    roleLabel.setString("Role: " + game.getCurrentTurn()->getClassName());
    coinLabel.setString("Coins: " + std::to_string(game.getCurrentTurn()->getCoins()));
}

void TurnController::hideReactionUI()
{
    // hide the reaction UI overlay
    reactionActive = false;
}

void TurnController::performPendingReverseUI(std::string reverseAction)
{
    game.performPendingReverse(reverseAction);
}