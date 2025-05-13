#include <cmath>
#include <SFML/Graphics.hpp>
#include "../Logic/Game.hpp"
#include <string>
#include "TurnController.hpp"


std::string promptPlayerName(int playerIndex) {
    const sf::Vector2u size{400, 150};
    sf::RenderWindow win({size.x, size.y}, "Enter Player Name", sf::Style::Close);
    win.setFramerateLimit(30);
    sf::Font font = sf::Font();
    font.loadFromFile("Assets/arial.ttf");
    // Instruction text
    sf::Text prompt("Enter name for Player " + std::to_string(playerIndex) + ":", font, 20);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition(10, 10);

    // The editable text
    std::string input = "";
    sf::Text edit(input, font, 24);
    edit.setFillColor(sf::Color::Yellow);
    edit.setPosition(10, 50);

    while (win.isOpen())
    {
        sf::Event e;
        while (win.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                win.close();
                return "";  // cancelled
            }
            if (e.type == sf::Event::TextEntered)
            {
                if ((e.text.unicode == '\r' || e.text.unicode == '\n') && !input.empty())
                {
                    // Enter pressed
                    win.close();
                    return input;
                }
                else if (e.text.unicode == 8 && !input.empty())
                {
                    // Backspace
                    input.pop_back();
                }
                else if (e.text.unicode >= 32 && e.text.unicode < 128)
                {
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

int gameWindow(int players) {
    sf::RenderWindow window(sf::VideoMode(1200,900), "Coup Game");
    window.setFramerateLimit(60);
    sf::Font font;
    font.loadFromFile("assets/arial.ttf");
    Game game = Game(players);

    TurnController controller(game, font, window);

    while(window.isOpen()){
        sf::Event e;
        while(window.pollEvent(e))
        {
            if(e.type==sf::Event::Closed)
                window.close();
            controller.handleClick(e);
        }
        controller.update();
        window.clear({30,30,30});
        controller.render();
        window.display();
    }

    return 0;
}
