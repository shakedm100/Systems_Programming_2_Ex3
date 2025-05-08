// game_window.cpp
#include "game_window.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <bits/random.h>
#include <random>     // for mt19937, uniform_int_distribution
#include "../Game.hpp"
#include "../Roles/headers/Baron.hpp"
#include "../Roles/headers/General.hpp"
#include "../Roles/headers/Governor.hpp"
#include "../Roles/headers/Judge.hpp"
#include "../Roles/headers/Merchant.hpp"
#include "../Roles/headers/Spy.hpp"


using namespace std;

// Pops up a window asking “Enter name for Player #X:”
// Returns the string (empty if closed)
std::string promptPlayerName(const sf::Font& font, int playerIndex) {
    const sf::Vector2u size{400, 150};
    sf::RenderWindow win({size.x, size.y}, "Enter Player Name", sf::Style::Close);
    win.setFramerateLimit(30);

    // Instruction text
    sf::Text prompt("Enter name for Player " + std::to_string(playerIndex) + ":", font, 20);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition(10, 10);

    // The editable text
    std::string input;
    sf::Text edit(input, font, 24);
    edit.setFillColor(sf::Color::Yellow);
    edit.setPosition(10, 50);

    while (win.isOpen()) {
        sf::Event e;
        while (win.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                win.close();
                return "";  // cancelled
            }
            if (e.type == sf::Event::TextEntered) {
                if (e.text.unicode == '\r' || e.text.unicode == '\n') {
                    // Enter pressed
                    win.close();
                    return input;
                }
                else if (e.text.unicode == 8 && !input.empty()) {
                    // Backspace
                    input.pop_back();
                }
                else if (e.text.unicode >= 32 && e.text.unicode < 128) {
                    // Visible ASCII
                    input += static_cast<char>(e.text.unicode);
                }
                edit.setString(input);
            }
        }

        win.clear(sf::Color(50,50,50));
        win.draw(prompt);
        win.draw(edit);
        win.display();
    }

    return "";
}

Game createGame(const int playersCount, const sf::Font& font)
{
    vector<Player*> players;
    players.reserve(playersCount);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dice(1,6);

    for(int i = 0; i < playersCount; i++)
    {
        std::string name = promptPlayerName(font, i+1);
        if (name.empty())
        {
            throw std::invalid_argument("Player naming cancelled!\n");
        }

        int result = dice(mt);
        switch (result)
        {
            case 1:
            {
                players.push_back(new Governor(name));
                break;
            }
            case 2:
            {
                players.push_back(new Spy(name));
                break;
            }
            case 3:
            {
                players.push_back(new Baron(name));
                break;
            }
            case 4:
            {
                players.push_back(new General(name));
                break;
            }
            case 5:
            {
                players.push_back(new Judge(name));
                break;
            }
            case 6:
            {
                players.push_back(new Merchant(name));
                break;
            }
        }
    }

    return Game(players);
}

void gameWindow(int playersCount, const sf::Font& font) {
    sf::RenderWindow wnd({1000, 800}, "Coup – Game", sf::Style::Close);
    wnd.setFramerateLimit(60);

    // --- Prepare UI Elements ---

    // 1) Action buttons at the top
    std::vector<std::string> actions = {
        "Gather", "Tax", "Bribe", "Arrest", "Sanction", "Coup"
    };
    std::vector<sf::RectangleShape> btns;
    std::vector<sf::Text>   btnLabels;
    std::vector<Player*> otherPlayers;
    bool goNextTurn = true;

    const float btnW = 140, btnH = 50, btnY = 20, spacing = 20;
    float x = spacing;

    for (auto& act : actions) {
        sf::RectangleShape btn({btnW, btnH});
        btn.setFillColor({70, 130, 180});  // steel blue
        btn.setPosition(x, btnY);
        btns.push_back(btn);

        sf::Text lbl(act, font, 20);
        lbl.setFillColor(sf::Color::White);
        auto b = btn.getGlobalBounds();
        auto t = lbl.getLocalBounds();
        lbl.setPosition(
            b.left + (b.width  - t.width )/2 - t.left,
            b.top  + (b.height - t.height)/2 - t.top
        );
        btnLabels.push_back(lbl);

        x += btnW + spacing;
    }

    // 2) Current card in bottom middle
    sf::Text currentCard("Your Card: ", font, 24);
    currentCard.setFillColor(sf::Color::White);
    {
        auto bounds = currentCard.getLocalBounds();
        currentCard.setPosition(
            (wnd.getSize().x - bounds.width)/2 - bounds.left,
            wnd.getSize().y - 80
        );
    }
    // 2a) Load the Duke image
    sf::Texture dukeTexture;
    if (!dukeTexture.loadFromFile("Assets/Duke.png")) {
        std::cerr << "Failed to load Duke.png\n";
        // you can choose to fallback or return
    }

    // 2b) Create a sprite and place it above the card text
    sf::Sprite dukeSprite(dukeTexture);
    // scale down if your image is large (optional)
    dukeSprite.setScale(0.5f, 0.5f);

    // position it so its bottom center aligns with the top center of currentCard
    {
        auto cardBounds   = currentCard.getGlobalBounds();
        auto spriteBounds = dukeSprite.getGlobalBounds();

        float spriteX = cardBounds.left + (cardBounds.width - spriteBounds.width) / 2;
        float spriteY = cardBounds.top  - spriteBounds.height - 10; // 10px padding

        dukeSprite.setPosition(spriteX, spriteY);
    }

    bool choosingTarget = false;
    enum TargetAction { None, Arrest, Sanction, Coup } targetAction = None;

    Game game = createGame(playersCount, font);

    std::vector<sf::RectangleShape> targetBtns;
    std::vector<sf::Text>          targetLbls;
    const float targetBtnW = 120, targetBtnH = 40;

    // 3) Current player in bottom right
    sf::Text currentPlayer(game.getCurrentTurn()->getName() + "'s Turn", font, 20);
    currentPlayer.setFillColor(sf::Color::White);
    {
        auto bounds = currentPlayer.getLocalBounds();
        currentPlayer.setPosition(
            wnd.getSize().x - bounds.width - 20 - bounds.left,
            wnd.getSize().y - 40
        );
    }

    // --- Main loop ---
    while (wnd.isOpen()) {
        sf::Event evt;
        if(goNextTurn)
            game.nextTurn();
        else
            goNextTurn = true;
        currentCard.setString("Your Card: " + game.getCurrentTurn()->getClassName());

        while (wnd.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) {
                wnd.close();
            }
            else if (evt.type == sf::Event::MouseButtonPressed &&
                     evt.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f m(evt.mouseButton.x, evt.mouseButton.y);
                for (size_t i = 0; i < btns.size(); ++i)
                {
                    if (btns[i].getGlobalBounds().contains(m))
                    {
                        // Handle action i
                        std::cout << "Action: " << actions[i] << "\n";
                        if(actions[i] == "Gather")
                        {
                            game.getCurrentTurn()->gather();
                        }
                        else if(actions[i] == "Tax")
                        {
                            game.getCurrentTurn()->tax();
                        }
                        else if(actions[i] == "Bribe")
                        {
                            bool success = game.getCurrentTurn()->bribe();
                            if(success)
                            {
                                goNextTurn = false;
                            }
                            else
                            {
                                //TODO: Show failed
                            }
                        }
                        else if (actions[i] == "Arrest" || actions[i] == "Sanction" || actions[i] == "Coup")
                        {
                            // 1) enter target-selection mode
                            choosingTarget = true;
                            targetBtns.clear();
                            targetLbls.clear();
                            otherPlayers.clear();

                            // set which action
                            if (actions[i] == "Arrest")    targetAction = Arrest;
                            if (actions[i] == "Sanction")  targetAction = Sanction;
                            if (actions[i] == "Coup")      targetAction = Coup;

                            // 2) create a button for each other player
                            float tx = spacing;
                            float ty = btnY + btnH + spacing;
                            std::vector<Player*> playersTemp = game.getPlayers();
                            for (auto& pPtr : game.getPlayers())
                            {
                                if (pPtr->getName() == game.getCurrentTurn()->getName()) continue;  // skip self
                                otherPlayers.push_back(pPtr);

                                sf::RectangleShape b({targetBtnW, targetBtnH});
                                b.setFillColor({200, 80, 80}); // e.g. red tint
                                b.setPosition(tx, ty);
                                targetBtns.push_back(b);

                                sf::Text lbl(pPtr->getName(), font, 18);
                                lbl.setFillColor(sf::Color::White);
                                auto bb = b.getGlobalBounds();
                                auto tb = lbl.getLocalBounds();
                                lbl.setPosition(
                                        bb.left + (bb.width  - tb.width )/2 - tb.left,
                                        bb.top  + (bb.height - tb.height)/2 - tb.top
                                );
                                targetLbls.push_back(lbl);

                                tx += targetBtnW + spacing;
                            }
                        }

                    }
                }
            }
        }

        // Draw
        wnd.clear({30, 30, 30});
        for (auto& btn: btns)        wnd.draw(btn);

        if (choosingTarget)
        {
            for (size_t t = 0; t < targetBtns.size(); ++t)
            {
                sf::Vector2f m(evt.mouseButton.x, evt.mouseButton.y);
                if (targetBtns[t].getGlobalBounds().contains(m))
                {
                    // call the appropriate game method on the victim
                    Player* victim = otherPlayers[t];

                    switch (targetAction) {
                        case Arrest:   game.getCurrentTurn()->arrest(*victim); break;
                        case Sanction: game.getCurrentTurn()->sanction(*victim); break;
                        case Coup:     game.getCurrentTurn()->coup(*victim); break;
                        default: break;
                    }
                    // exit target mode
                    choosingTarget = false;
                    targetAction = None;
                    targetBtns.clear();
                    targetLbls.clear();
                    break;
                }
            }
        }

        for (auto& lbl : btnLabels)   wnd.draw(lbl);
        wnd.draw(currentCard);
        wnd.draw(dukeSprite);
        wnd.draw(currentPlayer);
        if (choosingTarget) {
            for (auto& b : targetBtns)   wnd.draw(b);
            for (auto& l : targetLbls)   wnd.draw(l);
        }
        wnd.display();
    }
}
