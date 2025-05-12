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
    ReactionTime,
    ReactionBribe,
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
    const int MAX_PLAYERS = 6;
    bool peekThisTurn;

    // main action buttons
    std::vector<std::string> actions;
    std::vector<sf::RectangleShape> btns;
    std::vector<sf::Text>          btnLabels;

    // role-specific (special) action buttons:
    std::vector<std::string>        specialActions;
    std::vector<sf::RectangleShape> specialBtns;
    std::vector<sf::Text>           specialLabels;

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
    std::vector<sf::Text> playerStatusLabels;

    // ReactionTime UI elements
    bool reactionActive = false;
    sf::Text          reactionTitle;
    sf::RectangleShape btnReverse;
    sf::Text          lblReverse;
    sf::RectangleShape btnPass;
    sf::Text          lblPass;

    // error handling
    sf::Clock   errorClock;
    bool        errorActive = false;

public:

    TurnController(Game& game, sf::Font& font, sf::RenderWindow& wnd);
    void handleClick(const sf::Event& evt);
    void update();
    void render();

private:
    void setupForCurrentPlayer();
    void finishTurn();
    void applyPending();
    void enterTargetMode();
    void enterResurrectionTargetMode();
    void clearUI();
    void centerOrigin(sf::Text& t);
    void centerTextIn(const sf::RectangleShape& b, sf::Text& t);
    void finishGame();
    void updateStatusLabels();
    void showError(const std::string& msg);
    void showReactionUI();
    void hideReactionUI();
    void performPendingReverseUI(std::string reverseAction);
    void centerLabel(sf::Text& label, const sf::RectangleShape& button);

};

