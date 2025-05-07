#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    // 1) Create the window
    sf::RenderWindow window({800, 600}, "Coup Home", sf::Style::Close);
    window.setFramerateLimit(60);

    // 2) Load a font
    sf::Font font;
    if (!font.loadFromFile("Assets/arial.ttf")) {
        std::cerr << "Failed to load font\n";
        return 1;
    }

    // 3) Title text: "COUP"
    sf::Text title("COUP", font, 72);
    title.setFillColor(sf::Color::White);
    title.setPosition(
            (window.getSize().x - title.getLocalBounds().width) / 2,
            50
    );

    // 4) Button shapes
    sf::Vector2f btnSize(200, 60);
    sf::RectangleShape btnStart(btnSize);
    sf::RectangleShape btnExit(btnSize);

    btnStart.setPosition(
            (window.getSize().x - btnSize.x) / 2,
            200
    );
    btnExit.setPosition(
            (window.getSize().x - btnSize.x) / 2,
            300
    );

    btnStart.setFillColor(sf::Color(100, 149, 237)); // CornflowerBlue
    btnExit .setFillColor(sf::Color(220, 20, 60));   // Crimson

    // 5) Button labels
    sf::Text txtStart("Start Game", font, 24);
    sf::Text txtExit ("Exit",       font, 24);

    txtStart.setFillColor(sf::Color::White);
    txtExit .setFillColor(sf::Color::White);

    // center texts in buttons
    auto centerText = [&](sf::Text& txt, sf::RectangleShape& btn) {
        sf::FloatRect b = btn.getGlobalBounds();
        sf::FloatRect t = txt.getLocalBounds();
        txt.setPosition(
                b.left + (b.width  - t.width ) / 2 - t.left,
                b.top  + (b.height - t.height) / 2 - t.top
        );
    };
    centerText(txtStart, btnStart);
    centerText(txtExit,  btnExit);

    // 6) Event loop
    while (window.isOpen())
    {
        sf::Event evt;
        while (window.pollEvent(evt))
        {
            if (evt.type == sf::Event::Closed)
                window.close();

            if (evt.type == sf::Event::MouseButtonPressed &&
                evt.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mouse(evt.mouseButton.x, evt.mouseButton.y);
                if (btnStart.getGlobalBounds().contains(mouse)) {
                    std::cout << "Start Game clicked!\n";
                    // TODO: transition to game state…
                }
                if (btnExit.getGlobalBounds().contains(mouse)) {
                    window.close();
                }
            }
        }

        // 7) Draw everything
        window.clear(sf::Color(30, 30, 30)); // dark background
        window.draw(title);
        window.draw(btnStart);
        window.draw(txtStart);
        window.draw(btnExit);
        window.draw(txtExit);
        window.display();
    }

    return 0;
}

