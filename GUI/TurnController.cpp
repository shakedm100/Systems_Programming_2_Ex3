//shaked1mi@gmail.com
#include "TurnController.hpp"
#include <sstream>

// Build the basic game window
TurnController::TurnController(Game& game, sf::Font& font, sf::RenderWindow& wnd)
            : game(game), font(font), window(wnd)
{
    // Prepare action buttons
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
    // Current-player label
    currentPlayer.setFont(font);
    currentPlayer.setCharacterSize(28);
    currentPlayer.setFillColor(sf::Color::White);

    // Coin label
    coinLabel.setFont(font);
    coinLabel.setCharacterSize(28);
    coinLabel.setFillColor(sf::Color::Yellow);
    coinLabel.setPosition(20.f, wnd.getSize().y - 40.f);

    // Role label
    roleLabel.setFont(font);
    roleLabel.setCharacterSize(40);
    roleLabel.setFillColor(sf::Color::White);

    // Error label
    errorLabel.setFont(font);
    errorLabel.setCharacterSize(40);
    errorLabel.setFillColor(sf::Color::Red);
    errorLabel.setString("");
    centerOrigin(errorLabel);
    errorLabel.setPosition(wnd.getSize().x/2.f, wnd.getSize().y/2.f);

    // Set the player labels on the right
    playerStatusLabels.resize(MAX_PLAYERS);
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        auto& lbl = playerStatusLabels[i];
        lbl.setFont(font);
        lbl.setCharacterSize(18);
        lbl.setFillColor(sf::Color::White);
    }

    // Create reaction time title
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

    // Set game status's bools
    peekThisTurn = false;
    pendingTarget = nullptr;
    reactionActive = false;
    errorActive = false;
    phase = Phase::StartTurn; // Set the current Phase to game start
}

void TurnController::centerLabel(sf::Text& label, const sf::RectangleShape& button)
{
    auto bounds = label.getLocalBounds();
    label.setOrigin(bounds.left + bounds.width/2.f,
                    bounds.top  + bounds.height/2.f); // Middle of the label
    sf::Vector2f pos = button.getPosition();
    sf::Vector2f size = button.getSize();
    label.setPosition(pos + size * 0.5f); // Center inside the button
}

void TurnController::handleClick(const sf::Event& event)
{
    // Check if the game is in reaction Phase and a mouse was clicked
    if (phase == Phase::ReactionTime && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2f m(event.mouseButton.x, event.mouseButton.y); // Get mouse position
        if (buttonReverse.getGlobalBounds().contains(m)) // Check if the mouse click position was inside reverse button
        {
            executeReverseAction(labelReverse.getString()); // Execute the reverse action
            game.clearPendingReverse(); // Finished reacting - clear the pending action
            hideReactionUI(); // Return to normal window
            phase = Phase::EndTurn; // Exit reaction Phase
        }
        else if (buttonPass.getGlobalBounds().contains(m)) // If the mouse clicked on 'Pass' button
        {
            if (game.advancePendingResponder()) // Go to the next if exist and show the UI again
                showReactionUI();
            else
            {
                if(game.isPendingActionBribe()) // Return to bribe turn if the original action was a bribe
                    phase = Phase::ReactionBribe;
                else // Go to next turn of the original player
                    phase = Phase::EndTurn;
                game.clearPendingReverse();
                hideReactionUI();  // Hide reaction overlay
            }
        }
        return;
    }

    if (event.type != sf::Event::MouseButtonPressed || event.mouseButton.button != sf::Mouse::Left)  // If a mouse wasn't clicked - return
        return;
    sf::Vector2f m(event.mouseButton.x, event.mouseButton.y); // Get the current mouse position
    switch (phase) // Check all of the other phases
    {
        case Phase::ChooseAction: // If in choose action phase
            for (size_t i = 0; i < specialButtons.size(); ++i)
            {
                if (specialButtons[i].getGlobalBounds().contains(m)) // Check if a special action button was pressed
                {
                    pendingAction = specialActions[i];
                    phase = Phase::ResolveAction; // Go to resolve action Phase
                    return;
                }
            }

            for (size_t i=0; i<buttons.size(); ++i)
            {
                if (buttons[i].getGlobalBounds().contains(m)) // Check if a 'nornal' action button was pressed
                {
                    pendingAction = actions[i]; 
                    if (game.actionNeedsTarget(pendingAction)) // If it's an action that requires a target
                    {
                        enterTargetMode(); // Show targets
                        phase = Phase::ChooseTarget; // Move to choose target Phase
                    }
                    else // Otherwise just go to resolve action Phase
                        phase = Phase::ResolveAction; 

                    return;
                }
            }
            break;

        case Phase::ChooseTarget: // Choose target Phase
            for (size_t t=0; t<targetButtons.size(); ++t) // Go over all of the possible targets buttons
            {
                if (targetButtons[t].getGlobalBounds().contains(m)) // Check if the click occured in this button
                {
                    pendingTarget = otherPlayers[t]; // get the player that's been chosen
                    phase = Phase::ResolveAction; // Move to Resolve action phase
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
    switch (phase) // Check which phase the game currently in
    {
        case Phase::StartTurn: // If it's in StartTurn phase
            setupForCurrentPlayer(); // Set the basic game screen
            phase = Phase::ChooseAction; // Then move to choose action phase and wait for the player's decision
            break;
        case Phase::ResolveAction: // If it's in ResolveAction phase
            executeAction(); // Then execute the chosen action
            if (game.checkWinner()) // If the action is a coup and winning condition are met
                finishGame(); // Then finish the game
            else if (game.hasPendingReverse()) // If there is a reverse action waiting
            {
                phase = Phase::ReactionTime; // Go to ReactionTime phase
                showReactionUI(); // Show the reaction UI
            }
            else if (game.getCurrentTurn()->getExtraTurns() > 0) // If the current player has more then one turn to play
            {
                game.getCurrentTurn()->decreaseExtraTurns(); // Decrease the extra turns
                phase = Phase::StartTurn; // Let him play again
            }
            else
                phase = Phase::EndTurn; // Finish the turn
            break;
        case Phase::ReactionBribe: // If the game is in ReactionBribe phase
            if (game.getCurrentTurn()->getExtraTurns() > 0) // If the current player has more then one turn to play
            {
                game.getCurrentTurn()->decreaseExtraTurns(); // Decrease the extra turns
                phase = Phase::StartTurn; // Let him play again
            }
            break;
        case Phase::EndTurn: // If the game is in EndTurn phase
            if (game.getCurrentTurn()->getExtraTurns() > 0) // If the current player has an extra turn
            {
                game.getCurrentTurn()->decreaseExtraTurns(); // Decrease the extra turns
                phase = Phase::StartTurn; // Let him play again
            }
            else
            {
               finishTurn(); // Move to next player
               phase = Phase::StartTurn; // Change phase
            }
            break;
        default:
            break;
    }
}

void TurnController::render()
{
    // If there is an error show it for one second
    if (errorActive && errorClock.getElapsedTime().asSeconds() > 1.f) 
        errorActive = false; // Deactivate error message

    // draw only; clearing&display done in main
    if (phase==Phase::GameOver) // If the game is over
        window.draw(gameOverLabel); // Then show the finished game screen
    else if (phase == Phase::ReactionTime) // If the game is in ReactionTime
    {
        // Draw reaction time elements
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
    else // Not in reaction time
    {
        // Draw non reaction time elements
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

        if (phase==Phase::ChooseTarget) // If the player needs to choose a target
        {
            // Print the target's buttons and labels
            for (auto& b:targetButtons) window.draw(b); 
            for (auto& l:targetLabels) window.draw(l);
        }
        // Draw the current player's stats
        window.draw(currentPlayer);
        window.draw(roleLabel);
        window.draw(coinLabel);
        // If error bool is active then draw it
        if (errorActive) {
            window.draw(errorLabel);
        }
        for (int i = 0; i < MAX_PLAYERS; ++i)
            window.draw(playerStatusLabels[i]); // Draw player stats labels
    }
}

void TurnController::setupForCurrentPlayer()
{
    clearUI(); // Clear all the current UI elements
    // Update player name & coins
    auto* p = game.getCurrentTurn();
    currentPlayer.setString(p->getName() + "'s Turn");
    roleLabel.setString("Role: " + game.getCurrentTurn()->getClassName());

    // Set the 'currentPlayer' label location
    auto tb = currentPlayer.getLocalBounds();
    currentPlayer.setOrigin(tb.left + tb.width, tb.top + tb.height);
    float marginX = 20.f, marginY = 20.f;
    currentPlayer.setPosition(window.getSize().x - marginX, window.getSize().y - marginY);

    // Set the 'roleLabel' location
    auto rl = roleLabel.getLocalBounds();
    roleLabel.setOrigin(rl.left + rl.width/2.f, rl.top  + rl.height);
    float marginY2 = 60.f;
    roleLabel.setPosition(window.getSize().x/2.f, window.getSize().y - marginY2);

    auto role = p->getClassName();

    // If there is a special button to show then prepare it
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

    // Show the player's coins
    coinLabel.setString("Coins: " + std::to_string(p->getCoins()));


    // Lay them out down the left side:
    float downX = 20.f;
    float downY = window.getSize().y / 1.3f;    // start ~80px up from bottom
    for (auto& name : specialActions) { // Set the special buttons & labels location and color
        sf::RectangleShape b({200.f, 50.f});
        b.setPosition(downX, downY); // Bottom left
        b.setFillColor({50,180,50}); // Greenish color
        specialButtons.push_back(b);

        sf::Text lbl(name, font, 30);
        lbl.setFillColor(sf::Color::White);
        centerLabel(lbl, b); // Center at button location
        specialLabels.push_back(lbl);

        downY -= 70.f;  // stack upward if there is more then one (Shouldn't happen)
    }
}

void TurnController::finishTurn()
{
    // Finish the current turn logic
    game.getCurrentTurn()->clearStatusEffects(); // Remove all current status effects from the current player
    peekThisTurn = false; // If the spy peeked, disable the effect
    game.nextTurn(); // Go to next turn
}

void TurnController::updateStatusLabels()
{
    auto players = game.getPlayers();  // Get the player's vector
    const float rightMargin = 20.f; // Close to the right
    const float lineHeight = 22.f; // Character size + spacing
    const float startY    = 20.f;// Start at the top

    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        auto& currentLabel = playerStatusLabels[i];
        if (i < (int)players.size()) // Iterate through all the players
        {
            Player* current = players[i];

            // Build the status string
            std::ostringstream statusString;
            statusString << "-------------------------\n"
               << current->getName() << "\n"
               << current->getClassName() << "\n"
               << (current->getStatus().isAlive ? "Alive" : "Dead") << "\n";
            //ss << "Coins: " << p->getCoins() << "\n"; // For debugging purposes
            if(peekThisTurn)
                statusString << "Coins: " << current->getCoins() << "\n";
            if (current->getStatus().isSanctioned) // Adjust if you have multiple effect types
                statusString << "[Sanctioned]";
            if(current->getStatus().isInvested)
                statusString << " [Invested]";
            statusString << "\n-------------------------\n";
            currentLabel.setString(statusString.str());

            // Position: anchored to right
            float x = window.getSize().x - rightMargin;
            float y = startY + i * lineHeight * 6; // About 6 lines per player
            // To right-align each block, set origin to right edge:
            sf::FloatRect bounds = currentLabel.getLocalBounds();
            currentLabel.setOrigin(bounds.width, 0);
            currentLabel.setPosition(x, y);
        } else {
            // Hide unused labels
            currentLabel.setString("");
        }
    }
}

void TurnController::executeAction() {
    auto* actor = game.getCurrentTurn();
    try
    {
        if (game.canPerform(actor, pendingAction, pendingTarget)) // Check if the action is legal
        {
            game.perform(actor, pendingAction, pendingTarget); // If it is, then do it
            if (pendingAction == "Peek") // If the action is peeking
            {
                peekThisTurn = true; // Set the bool to show all the coins in the right labels
                phase = Phase::ChooseAction; // Free action - go back to choose action phase
            }
            game.setupPendingReverse(actor, pendingAction, pendingTarget); // Check if a reaction to the action is possible
            pendingTarget = nullptr;
        }
    }
    catch (const std::exception& e) // If an action occured - then the action was illegal
    {
        showError(e.what()); // Show the error message to the player
        pendingTarget = nullptr;
        centerOrigin(errorLabel);
        game.getCurrentTurn()->increaseExtraTurns(); // Give the player another turn to try another action
        phase = Phase::ChooseAction; // Change to choose action phase
    }
}

void TurnController::showError(const std::string& msg)
{
    errorLabel.setString(msg);
    centerOrigin(errorLabel); // Re-center since string changed
    errorClock.restart(); // Start timing now
    errorActive = true; // Turn it on
}

void TurnController::enterTargetMode()
{
    // Clear all previous data
    targetButtons.clear();
    targetLabels.clear();
    otherPlayers.clear();
    float x = 20.f, y = 80.f + 60.f;
    for (auto* current : game.getAlivePlayers()) // Iterate through all the living players
    {
        if (current == game.getCurrentTurn()) continue; // Don't show the current player
        otherPlayers.push_back(current);
        // Setup the target buttons and labels
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
    // Clear all UI elements (excluding reaction elements)
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
    // Clear other UI
    buttons.clear();
    buttonLabels.clear();
    targetButtons.clear();
    targetLabels.clear();
    otherPlayers.clear();
    // Prepare game-over label
    std::string winner = game.getWinner()->getName();
    gameOverLabel.setString("Game Over! Winner: " + winner);
    // Adjust line spacing for readability
    gameOverLabel.setLineSpacing(1.2f);
    gameOverLabel.setFont(font);
    // Center the origin based on updated bounds
    centerOrigin(gameOverLabel);
    // Place at window center
    gameOverLabel.setPosition(window.getSize().x/2.f, window.getSize().y/2.f);
    phase = Phase::GameOver;
}

void TurnController::showReactionUI()
{
    reactionActive = true;
    // Set reverse label
    const std::string act = game.getPendingActionLabel();
    const std::string& text = act;
    labelReverse.setString(text);
    centerLabel(labelReverse, buttonReverse);
    // Show the reacting player's stats
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
    game.performPendingReverse(reverseAction); // Execute the reaction action
}