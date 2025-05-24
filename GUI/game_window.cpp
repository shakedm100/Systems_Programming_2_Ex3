//shaked1mi@gmail.com
#include <SFML/Graphics.hpp>
#include "../Logic/Game.hpp"
#include <string>
#include "game_window.hpp"
#include "TurnController.hpp"


std::string promptPlayerName(int playerIndex) {
    // Create the window
    const sf::Vector2u size{400, 150};
    sf::RenderWindow win({size.x, size.y}, "Enter Player Name", sf::Style::Close);
    win.setFramerateLimit(30);

    // Load the Arial font
    sf::Font font;
    font.loadFromFile("GUI/Assets/arial.ttf");

    // Instruction text
    sf::Text prompt("Enter name for Player " + std::to_string(playerIndex) + ":", font, 20);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition(10, 10);

    // The editable text
    std::string input;
    sf::Text edit(input, font, 24);
    edit.setFillColor(sf::Color::Black); // dark text
    edit.setPosition(10, 50);

    // Add a white rectangle behind the edit field
    sf::RectangleShape inputBox;
    inputBox.setSize({380.f, 32.f});
    inputBox.setFillColor(sf::Color::White);
    inputBox.setPosition(10.f, 50.f);

    while (win.isOpen())
    {
        sf::Event e;
        while (win.pollEvent(e)) // Get the event
        {
            if (e.type == sf::Event::Closed) { // If the X button was pressed
                win.close(); // Close the window
                return "";
            }
            if (e.type == sf::Event::TextEntered) {
                if ((e.text.unicode == '\r' || e.text.unicode == '\n') && !input.empty()) { // If the player pressed enter
                    win.close();
                    return input; // Return the name
                }
                else if (e.text.unicode == 8 && !input.empty()) { // Unicode 8 is backwards
                    input.pop_back(); // Erase the last char
                }
                // Anywhere from 32 until 127 in the Unicode (or ASCII in this case) can be inputted - a-z, A-Z, space, etc...
                else if (e.text.unicode >= 32 && e.text.unicode < 128) {
                    input += static_cast<char>(e.text.unicode); //  Cast the unicode to char
                }
                edit.setString(input); // Show the current state of the input string
            }
        }
        
        // Display the screen with the elements
        win.clear(sf::Color(50,50,50));
        win.draw(prompt);
        win.draw(inputBox);   // draw white background
        win.draw(edit);       // draw text on top
        win.display();
    }

    return "";
}


int gameWindow(int players) {
    // Create the screen
    sf::RenderWindow window(sf::VideoMode(1200,900), "Coup Game");
    window.setFramerateLimit(60);
    // Load the font
    sf::Font font;
    font.loadFromFile("GUI/Assets/arial.ttf");
    // Create Game logic
    Game game = Game(players);

    // Create the game controller
    TurnController controller(game, font, window);

    while(window.isOpen()){
        sf::Event e;
        while(window.pollEvent(e)) // Get the current event
        {
            // If the window's X was pressed
            if(e.type==sf::Event::Closed)
                window.close();
            // Send the current event to handleClick to handle the logic of clicking a button
            controller.handleClick(e); 
        }
        // Update the current game Phase and execute an action if needed
        controller.update();
        window.clear({30,30,30});
        // Draw the relevant elements on the screen
        controller.render();
        // Show the updated screen
        window.display();
    }

    return 0;
}
