// game_window.cpp
#include "game_window.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <bits/random.h>

#include "../Game.hpp"
#include "../Roles/headers/Baron.hpp"
#include "../Roles/headers/General.hpp"
#include "../Roles/headers/Governor.hpp"
#include "../Roles/headers/Judge.hpp"
#include "../Roles/headers/Merchant.hpp"
#include "../Roles/headers/Spy.hpp"


using namespace std;

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
    sf::Text currentCard("Your Card: Duke", font, 24);
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

    // 3) Current player in bottom right
    sf::Text currentPlayer("Player 1's turn", font, 20);
    currentPlayer.setFillColor(sf::Color::White);
    {
        auto bounds = currentPlayer.getLocalBounds();
        currentPlayer.setPosition(
            wnd.getSize().x - bounds.width - 20 - bounds.left,
            wnd.getSize().y - 40
        );
    }

    vector<Player*> players;
    players.reserve(playersCount);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dice(1,6);

    for(int i = 0; i < playersCount; i++)
    {
        int result = dice(mt);
        switch (result)
        {
            case 1:
            {
                players.push_back(new Governor("name"));
                break;
            }
            case 2:
            {
                players.push_back(new Spy("name"));
                break;
            }
            case 3:
            {
                players.push_back(new Baron("name"));
                break;
            }
            case 4:
            {
                players.push_back(new General("name"));
                break;
            }
            case 5:
            {
                players.push_back(new Judge("name"));
            }
            case 6:
            {
                players.push_back(new Merchant("name"));
            }
        }
    }

    Game game = Game(players);

    // --- Main loop ---
    while (wnd.isOpen()) {
        sf::Event evt;
        while (wnd.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed) {
                wnd.close();
            }
            else if (evt.type == sf::Event::MouseButtonPressed &&
                     evt.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f m(evt.mouseButton.x, evt.mouseButton.y);
                for (size_t i = 0; i < btns.size(); ++i) {
                    if (btns[i].getGlobalBounds().contains(m)) {
                        // Handle action i
                        std::cout << "Action: " << actions[i] << "\n";
                    }
                }
            }
        }

        // Draw
        wnd.clear({30, 30, 30});
        for (auto& btn : btns)        wnd.draw(btn);
        for (auto& lbl : btnLabels)   wnd.draw(lbl);
        wnd.draw(currentCard);
        wnd.draw(dukeSprite);
        wnd.draw(currentPlayer);
        wnd.display();
    }
}
