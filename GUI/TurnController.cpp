//shaked1mi@gmail.com
#include "TurnController.hpp"
#include <sstream>

//TODO: Migrate to vscode and Make
//TODO: Add demo (main.cpp)
TurnController::TurnController(Game& game, sf::Font& font, sf::RenderWindow& wnd)
            : game(game), font(font), window(wnd)
{
    // prepare action buttons
    float x = 20.f, y = 20.f;
    for (const auto& name : game.getActionNames())
    {
        sf::RectangleShape currentButton({120.f, 40.f});
        currentButton.setPosition(x, y);
        currentButton.setFillColor({100,100,200});
        actions.push_back(name);
        buttons.push_back(currentButton);
        sf::Text t(name, font, 25);
        t.setFillColor(sf::Color::White);
        centerLabel(t, currentButton);
        buttonLabels.push_back(t);
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
    buttonReverse.setSize({200.f, 60.f});
    buttonReverse.setPosition(wnd.getSize().x/2.f - buttonReverse.getSize().x - 10.f,
        wnd.getSize().y/2.f - buttonReverse.getSize().y/2.f);
    buttonReverse.setFillColor({200,200,50});
    buttonReverse.setOutlineColor({100,100,20});
    buttonReverse.setOutlineThickness(4.f);

    labelReverse.setFont(font);
    labelReverse.setString("Reverse");
    labelReverse.setCharacterSize(24);
    labelReverse.setFillColor(sf::Color::Black);
    centerLabel(labelReverse, buttonReverse);

    // Pass button
    buttonPass.setSize({200.f, 60.f});
    buttonPass.setPosition(wnd.getSize().x/2.f + 10.f,wnd.getSize().y/2.f - buttonPass.getSize().y/2.f);
    buttonPass.setFillColor({200,200,50});
    buttonPass.setOutlineColor({100,100,20});
    buttonPass.setOutlineThickness(4.f);

    labelPass.setFont(font);
    labelPass.setString("Pass");
    labelPass.setCharacterSize(24);
    labelPass.setFillColor(sf::Color::Black);
    centerLabel(labelPass, buttonPass);

    peekThisTurn = false;
    pendingTarget = nullptr;
    reactionActive = false;
    errorActive = false;
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

void TurnController::handleClick(const sf::Event& event)
{
    if (phase == Phase::ReactionTime && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f m(event.mouseButton.x, event.mouseButton.y);
        if (buttonReverse.getGlobalBounds().contains(m))
        {
            executeReverseAction(labelReverse.getString());
            game.clearPendingReverse();
            hideReactionUI();
            phase = Phase::EndTurn;
        }
        else if (buttonPass.getGlobalBounds().contains(m))
        {
            if (game.advancePendingResponder())
                showReactionUI();
            else
            {
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

    if (event.type != sf::Event::MouseButtonPressed || event.mouseButton.button != sf::Mouse::Left)
        return;
    sf::Vector2f m(event.mouseButton.x, event.mouseButton.y);
    switch (phase)
    {
        case Phase::ChooseAction:
            for (size_t i = 0; i < specialButtons.size(); ++i)
            {
                if (specialButtons[i].getGlobalBounds().contains(m))
                {
                    pendingAction = specialActions[i];
                    phase = Phase::ResolveAction;
                    return;
                }
            }

            for (size_t i=0; i<buttons.size(); ++i)
            {
                if (buttons[i].getGlobalBounds().contains(m))
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
            for (size_t t=0; t<targetButtons.size(); ++t)
            {
                if (targetButtons[t].getGlobalBounds().contains(m))
                {
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
            executeAction();
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
        window.draw(gameOverLabel);
    else if (phase == Phase::ReactionTime)
    {
        window.draw(reactionTitle);
        window.draw(buttonReverse);
        window.draw(labelReverse);
        window.draw(buttonPass);
        window.draw(labelPass);
        window.draw(currentPlayer);
        window.draw(roleLabel);
        window.draw(coinLabel);
        updateStatusLabels();
    }
    else
    {
        updateStatusLabels();
        for (auto& l : playerStatusLabels)
            window.draw(l);
        for (auto& b:buttons)
            window.draw(b);
        for (auto& l:buttonLabels)
            window.draw(l);
        for (auto& b : specialButtons)
            window.draw(b);
        for (auto& t : specialLabels)
            window.draw(t);

        if (phase==Phase::ChooseTarget)
        {
            for (auto& b:targetButtons) window.draw(b);
            for (auto& l:targetLabels) window.draw(l);
        }
        window.draw(currentPlayer);
        window.draw(roleLabel);
        window.draw(coinLabel);
        if (errorActive) {
            window.draw(errorLabel);
        }
        for (int i = 0; i < MAX_PLAYERS; ++i)
            window.draw(playerStatusLabels[i]);
    }
}

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
    currentPlayer.setPosition(window.getSize().x - marginX, window.getSize().y - marginY);

    auto rl = roleLabel.getLocalBounds();
    roleLabel.setOrigin(rl.left + rl.width/2.f, rl.top  + rl.height);
    float marginY2 = 60.f;
    roleLabel.setPosition(window.getSize().x/2.f, window.getSize().y - marginY2);

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
    float downX = 20.f;
    float downY = window.getSize().y / 1.3f;    // start ~80px up from bottom
    for (auto& name : specialActions) {
        sf::RectangleShape b({200.f, 50.f});
        b.setPosition(downX, downY);
        b.setFillColor({50,180,50});
        specialButtons.push_back(b);

        sf::Text lbl(name, font, 30);
        lbl.setFillColor(sf::Color::White);
        centerLabel(lbl, b);
        specialLabels.push_back(lbl);

        downY -= 70.f;  // stack upward
    }
}

void TurnController::finishTurn()
{
    game.getCurrentTurn()->clearStatusEffects();
    peekThisTurn = false;
    game.nextTurn();
}

void TurnController::updateStatusLabels()
{
    auto players = game.getPlayers();  // assume this returns a std::vector<Player*>
    const float rightMargin = 20.f;
    const float lineHeight = 22.f;     // character size + spacing
    const float startY    = 20.f;

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        auto& currentLabel = playerStatusLabels[i];
        if (i < (int)players.size())
        {
            Player* current = players[i];

            // Build the status string
            std::ostringstream statusString;
            statusString << "-------------------------\n"
               << current->getName() << "\n"
               << current->getClassName() << "\n"
               << (current->getStatus().isAlive ? "Alive" : "Dead") << "\n";
            //ss << "Coins: " << p->getCoins() << "\n"; // for debugging purposes
            if(peekThisTurn)
                statusString << "Coins: " << current->getCoins() << "\n";
            if (current->getStatus().isSanctioned)        // adjust if you have multiple effect types
                statusString << "[Sanctioned]";
            if(current->getStatus().isInvested)
                statusString << " [Invested]";
            statusString << "\n-------------------------\n";
            currentLabel.setString(statusString.str());

            // position: anchored to right
            float x = window.getSize().x - rightMargin;
            float y = startY + i * lineHeight * 6; // About 6 lines per player
            // to right-align each block, set origin to right edge:
            sf::FloatRect bounds = currentLabel.getLocalBounds();
            currentLabel.setOrigin(bounds.width, 0);
            currentLabel.setPosition(x, y);
        } else {
            // hide unused labels
            currentLabel.setString("");
        }
    }
}

void TurnController::executeAction() {
    auto* actor = game.getCurrentTurn();
    try
    {
        if (game.canPerform(actor, pendingAction, pendingTarget))
        {
            game.perform(actor, pendingAction, pendingTarget);
            if (pendingAction == "Peek")
            {
                peekThisTurn = true;
                phase = Phase::ChooseAction;
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
    targetButtons.clear();
    targetLabels.clear();
    otherPlayers.clear();
    float x = 20.f, y = 80.f + 60.f;
    for (auto* current : game.getAlivePlayers())
    {
        if (current == game.getCurrentTurn()) continue;
        otherPlayers.push_back(current);
        sf::RectangleShape button({120.f,40.f}); button.setPosition(x,y);
        button.setFillColor({200,80,80});
        targetButtons.push_back(button);
        sf::Text text(current->getName(), font, 18);
        text.setFillColor(sf::Color::White);
        centerLabel(text, button);
        targetLabels.push_back(text);
        x += 140.f;
    }
}

void TurnController::clearUI()
{
    targetButtons.clear();
    targetLabels.clear();
    otherPlayers.clear();
    specialActions.clear();
    specialButtons.clear();
    specialLabels.clear();
}

void TurnController::centerOrigin(sf::Text& text)
{
    auto bound = text.getLocalBounds();
    text.setOrigin(bound.left + bound.width/2.f, bound.top + bound.height/2.f);
}

void TurnController::finishGame() {
    // clear other UI
    buttons.clear();
    buttonLabels.clear();
    targetButtons.clear();
    targetLabels.clear();
    otherPlayers.clear();
    // prepare game-over label
    std::string winner = game.getWinner()->getName();
    gameOverLabel.setString("Game Over! Winner: " + winner);
    // adjust line spacing for readability
    gameOverLabel.setLineSpacing(1.2f);
    gameOverLabel.setFont(font);
    // center the origin based on updated bounds
    centerOrigin(gameOverLabel);
    // place at window center
    gameOverLabel.setPosition(window.getSize().x/2.f, window.getSize().y/2.f);
    phase = Phase::GameOver;
}

void TurnController::showReactionUI()
{
    reactionActive = true;
    // set reverse label
    const std::string act = game.getPendingActionLabel();
    const std::string& text = act;
    labelReverse.setString(text);
    centerLabel(labelReverse, buttonReverse);

    currentPlayer.setString(game.getCurrentTurn()->getName() + "'s Turn");
    roleLabel.setString("Role: " + game.getCurrentTurn()->getClassName());
    coinLabel.setString("Coins: " + std::to_string(game.getCurrentTurn()->getCoins()));
}

void TurnController::hideReactionUI()
{
    // hide the reaction UI overlay
    reactionActive = false;
}

void TurnController::executeReverseAction(std::string reverseAction)
{
    game.performPendingReverse(reverseAction);
}