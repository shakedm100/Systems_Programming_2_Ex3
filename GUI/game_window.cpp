#include <SFML/Graphics.hpp>
#include "../Game.hpp"
#include "../Roles/headers/Player.hpp"
#include <vector>
#include <string>
#include "../GameSetup.hpp"

// --- Game Phases ---
enum class Phase {
    StartTurn,    // initialize UI for new player
    ChooseAction, // pick an action button
    ChooseTarget, // pick a target for actions that require one
    ResolveAction,// apply the chosen action
    EndTurn       // advance to next player
};

// --- Turn Controller ---
class TurnController {
public:
    TurnController(Game& game, sf::Font& font, sf::RenderWindow& wnd)
            : game(game), font(font), wnd(wnd)
    {
        // prepare action buttons
        float x = 20.f, y = 20.f;
        for (const auto& name : game.getActionNames()) {
            sf::RectangleShape b({120.f, 40.f});
            b.setPosition(x, y);
            b.setFillColor({100,100,200});
            actions.push_back(name);
            btns.push_back(b);
            sf::Text t(name, font, 18);
            t.setFillColor(sf::Color::White);
            centerTextIn(b, t);
            btnLabels.push_back(t);
            x += 140.f;
        }
        // current-player label
        currentPlayer.setFont(font);
        currentPlayer.setCharacterSize(20);
        currentPlayer.setFillColor(sf::Color::White);
        // coin label
        coinLabel.setFont(font);
        coinLabel.setCharacterSize(18);
        coinLabel.setFillColor(sf::Color::Yellow);
        coinLabel.setPosition(20.f, wnd.getSize().y - 40.f);
        // error label
        errorLabel.setFont(font);
        errorLabel.setCharacterSize(24);
        errorLabel.setFillColor(sf::Color::Red);
        errorLabel.setString("");
        centerOrigin(errorLabel);
        errorLabel.setPosition(wnd.getSize().x/2.f, wnd.getSize().y/2.f);

        phase = Phase::StartTurn;
    }

    void handleClick(const sf::Event& evt) {
        if (evt.type != sf::Event::MouseButtonPressed || evt.mouseButton.button != sf::Mouse::Left)
            return;
        sf::Vector2f m(evt.mouseButton.x, evt.mouseButton.y);
        switch (phase) {
            case Phase::ChooseAction:
                for (size_t i=0; i<btns.size(); ++i) {
                    if (btns[i].getGlobalBounds().contains(m)) {
                        pendingAction = actions[i];
                        if (game.actionNeedsTarget(pendingAction)) {
                            enterTargetMode();
                            phase = Phase::ChooseTarget;
                        } else {
                            phase = Phase::ResolveAction;
                        }
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

    void update() {
        switch (phase) {
            case Phase::StartTurn:
                setupForCurrentPlayer();
                phase = Phase::ChooseAction;
                break;

            case Phase::ResolveAction:
                applyPending();
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

    void render() {
        // action row
        for (auto& b : btns)      wnd.draw(b);
        for (auto& l : btnLabels) wnd.draw(l);
        // target row if needed
        if (phase == Phase::ChooseTarget) {
            for (auto& b : targetBtns) wnd.draw(b);
            for (auto& l : targetLbls) wnd.draw(l);
        }
        // status texts
        wnd.draw(currentPlayer);
        wnd.draw(coinLabel);
        if (!errorLabel.getString().isEmpty())
            wnd.draw(errorLabel);
    }

private:
    Game& game;
    sf::Font& font;
    sf::RenderWindow& wnd;
    Phase phase;

    // main action buttons
    std::vector<std::string> actions;
    std::vector<sf::RectangleShape> btns;
    std::vector<sf::Text>          btnLabels;

    // target selection
    std::vector<sf::RectangleShape> targetBtns;
    std::vector<sf::Text>           targetLbls;
    std::vector<Player*>            otherPlayers;

    // pending action data
    std::string pendingAction;
    Player*     pendingTarget = nullptr;

    // labels
    sf::Text currentPlayer;
    sf::Text coinLabel;
    sf::Text errorLabel;

    // helpers
    void setupForCurrentPlayer() {
        clearUI();
        // update player name & coins
        auto* p = game.getCurrentTurn();
        currentPlayer.setString(p->getName() + "'s Turn");
        centerOrigin(currentPlayer);
        currentPlayer.setPosition(wnd.getSize().x/2.f, 80.f);
        coinLabel.setString("Coins: " + std::to_string(p->getCoins()));
        errorLabel.setString("");
    }

    void applyPending() {
        auto* actor = game.getCurrentTurn();
        if (game.canPerform(actor, pendingAction, pendingTarget)) {
            game.perform(actor, pendingAction, pendingTarget);
        } else {
            errorLabel.setString("Illegal move!");
            centerOrigin(errorLabel);
            phase = Phase::ChooseAction;
            return;
        }
    }

    void enterTargetMode() {
        targetBtns.clear(); targetLbls.clear(); otherPlayers.clear();
        float x = 20.f, y = 80.f + 60.f;
        for (auto* p : game.getPlayers()) {
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

    void clearUI() {
        targetBtns.clear(); targetLbls.clear(); otherPlayers.clear();
    }

    void centerOrigin(sf::Text& t) {
        auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width/2.f, b.top + b.height/2.f);
    }

    void centerTextIn(const sf::RectangleShape& b, sf::Text& t) {
        auto bb = b.getGlobalBounds();
        auto tb = t.getLocalBounds();
        t.setPosition(
                bb.left + (bb.width - tb.width)/2 - tb.left,
                bb.top  + (bb.height - tb.height)/2 - tb.top
        );
    }
};

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

int gameWindow(int players) {
    sf::RenderWindow window(sf::VideoMode(800,600), "Coup Game");
    sf::Font font;
    font.loadFromFile("assets/arial.ttf");
    Game game = createGame(players);

    TurnController controller(game, font, window);

    while (window.isOpen()) {
        sf::Event evt;
        while (window.pollEvent(evt)) {
            if (evt.type == sf::Event::Closed)
                window.close();
            controller.handleClick(evt);
        }

        controller.update();

        window.clear({30,30,30});
        controller.render();
        window.display();
    }

    return 0;
}
