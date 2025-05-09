#include <SFML/Graphics.hpp>
#include "../Logic/Game.hpp"
#include "../Roles/headers/Player.hpp"
#include <vector>
#include <string>

    // --- Game Phases ---
enum class Phase
{
    StartTurn,    // initialize UI for new player
    ChooseAction, // pick an action button
    ChooseTarget, // pick a target for actions that require one
    ResolveAction,// apply the chosen action
    EndTurn,       // advance to next player
    GameOver        // Finish the game
};

class TurnController
{
private:
    Game& game;
    sf::Font& font;
    sf::RenderWindow& wnd;
    Phase phase;
    bool gameFinished;

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
    sf::Text roleLabel;
    sf::Text gameOverLabel;

public:

    TurnController(Game& game, sf::Font& font, sf::RenderWindow& wnd);
    void handleClick(const sf::Event& evt);
    void update();
    void render();

private:
    void setupForCurrentPlayer();
    void applyPending();
    void enterTargetMode();
    void clearUI();
    void centerOrigin(sf::Text& t);
    void centerTextIn(const sf::RectangleShape& b, sf::Text& t);
    void finishGame();

};

