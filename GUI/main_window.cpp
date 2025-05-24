//shaked1mi@gmail.com
#include <SFML/Graphics.hpp>
#include <iostream>
#include "game_window.hpp"

/**
 * This method receives a Font for its text and asks the player for input
 * how many players will play the next game
 * @param font a font usually found in /Assets folder
 * @return how many players will play the next game
 */
int promptPlayerCount(sf::Font& font) {
    const int minPlayers = 2, maxPlayers = 6;
    int count = minPlayers;

    // Show a small sized window
    sf::RenderWindow prompt({400, 200}, "How many players?", sf::Style::Titlebar | sf::Style::Close);
    prompt.setFramerateLimit(30);

    // Least players are 2, so set the label to this amount
    sf::Text txtCount("Players: 2", font, 30);
    txtCount.setFillColor(sf::Color::White);
    txtCount.setPosition(120, 30);

    // Buttons to decrease and increase the players
    sf::RectangleShape btnMinus({40, 40}), btnPlus({40, 40});
    btnMinus.setFillColor(sf::Color(200,0,0));
    btnPlus .setFillColor(sf::Color(0,200,0));
    btnMinus.setPosition( 60,  80);
    btnPlus .setPosition(300,  80);

    // Set the minus and plus texts
    sf::Text txtMinus("-", font, 24), txtPlus("+", font, 24);
    txtMinus.setFillColor(sf::Color::White);
    txtPlus .setFillColor(sf::Color::White);
    txtMinus.setPosition(btnMinus.getPosition() + sf::Vector2f(12,6));
    txtPlus .setPosition(btnPlus .getPosition() + sf::Vector2f(12,6));

    // Set the start button to initiate name asking prompt
    sf::RectangleShape btnStart({100, 40});
    btnStart.setFillColor(sf::Color(100,149,237));
    btnStart.setPosition(150, 140);
    sf::Text txtStart("Start", font, 24);
    txtStart.setFillColor(sf::Color::White);
    {
        auto b = btnStart.getGlobalBounds();
        auto t = txtStart.getLocalBounds();
        txtStart.setPosition(
            b.left + (b.width  - t.width )/2 - t.left,
            b.top  + (b.height - t.height)/2 - t.top
        );
    }

    while (prompt.isOpen()) {
        sf::Event e;
        while (prompt.pollEvent(e)) { // Check the current event in the window
            if (e.type == sf::Event::Closed) { // If the x button was pressed
                prompt.close(); // Close the window
                return 0;
            }
            if (e.type == sf::Event::MouseButtonPressed &&
                e.mouseButton.button == sf::Mouse::Left) // If a mouse button was pressed
            {
                sf::Vector2f m(e.mouseButton.x, e.mouseButton.y); // Get the mouse's position
                if (btnMinus.getGlobalBounds().contains(m) && count > minPlayers) { // If minus button pressed and count > 2
                    --count; // Decrease players
                }
                if (btnPlus.getGlobalBounds().contains(m) && count < maxPlayers) { // If plus button pressed and count < 6
                    ++count; // Increase players
                }
                if (btnStart.getGlobalBounds().contains(m)) { // If start button pressed
                    prompt.close(); // Close the window
                    return count; // Return how many players will play
                }
                txtCount.setString("Players: " + std::to_string(count));
            }
        }
        
        // Draw the buttons
        prompt.clear(sf::Color(50,50,50));
        prompt.draw(txtCount);
        prompt.draw(btnMinus);  
        prompt.draw(txtMinus);
        prompt.draw(btnPlus);   
        prompt.draw(txtPlus);
        prompt.draw(btnStart);  
        prompt.draw(txtStart);
        prompt.display();
    }
    return 0;
}

/**
 * This is the main function the UI game runs through
 * it creates the home window for the game where you can choose
 * either to start a game or exit
 */
int main()
{
    // Main window
    sf::RenderWindow window({800, 600}, "Coup Home", sf::Style::Close);
    window.setFramerateLimit(60);

    // Load font
    sf::Font font;
    if (!font.loadFromFile("GUI/Assets/arial.ttf")) {
        std::cerr << "Failed to load font\n";
        return 1;
    }

    // Title
    sf::Text title("COUP", font, 72);
    title.setFillColor(sf::Color::White);
    title.setPosition(
        (window.getSize().x - title.getLocalBounds().width)/2,50);

    // Buttons
    sf::Vector2f btnSize(200, 60);
    sf::RectangleShape btnStart(btnSize), btnExit(btnSize);
    btnStart.setFillColor(sf::Color(100,149,237));
    btnExit .setFillColor(sf::Color(220,20,60));
    btnStart.setPosition((800 - btnSize.x)/2, 200);
    btnExit .setPosition((800 - btnSize.x)/2, 300);

    // Button labels
    sf::Text txtStart("Start Game", font, 24), txtExit("Exit", font, 24);
    txtStart.setFillColor(sf::Color::White);
    txtExit .setFillColor(sf::Color::White);

    // Center texts in their buttons
    auto center = [&](sf::Text& txt, const sf::RectangleShape& btn){
        auto b = btn.getGlobalBounds();
        auto t = txt.getLocalBounds();
        txt.setPosition(
            b.left + (b.width  - t.width )/2 - t.left,
            b.top  + (b.height - t.height)/2 - t.top
        );
    };
    center(txtStart, btnStart);
    center(txtExit,  btnExit);

    // Main loop
    while (window.isOpen())
    {
        sf::Event evt;
        while (window.pollEvent(evt)) // Check the events in the window
        {
            if (evt.type == sf::Event::Closed) // If x button was pressed
                window.close(); // Close window

            if (evt.type == sf::Event::MouseButtonPressed &&
                evt.mouseButton.button == sf::Mouse::Left) // If mouse clicked
            {
                sf::Vector2f m(evt.mouseButton.x, evt.mouseButton.y); // Get mouse position
                if (btnStart.getGlobalBounds().contains(m)) { // If it clicked inside start button
                    int n = promptPlayerCount(font); // Get how many players will play
                    if (n > 0) {
                        window.close();
                        gameWindow(n); // Move to gameWindow
                    }
                }
                if (btnExit.getGlobalBounds().contains(m)) { // If the exit button was clicked
                    window.close();
                }
            }
        }

        // Draw all the elements
        window.clear(sf::Color(30,30,30));
        window.draw(title);
        window.draw(btnStart); window.draw(txtStart);
        window.draw(btnExit ); window.draw(txtExit);
        window.display();
    }

    return 0;
}
