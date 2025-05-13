#include <SFML/Graphics.hpp>
#include "../Logic/Game.hpp"
#include "../Roles/headers/Player.hpp"
#include <vector>
#include <string>

/**
 * This enum defines all the different states
 * the game can be in. Used by turn controller to manage
 * the flow of the turns.
 */
enum class Phase
{
    StartTurn,      // Initialize UI for new player
    ChooseAction,   // Pick an action button
    ChooseTarget,   // Pick a target for actions that require one
    ReactionTime,   // In this phase all relevant players can react
    ReactionBribe,  // In case a bribe is being reacted to another action it goes to ReactionBribe
    ResolveAction,  // Apply the chosen action
    EndTurn,        // Advance to next player
    GameOver        // Finish the game
};

/**
 * This class is responsible for bridging between the UI
 * and the logic of the game implemented in Game.cpp
 * This class main usage is to handle what to show when
 * and what to do when. Basically a manager class
 */
class TurnController
{
private:
    Game& game;
    sf::Font& font;
    sf::RenderWindow& window;
    Phase phase; // The current stage the game is at
    const int MAX_PLAYERS = 6;
    bool peekThisTurn;

    // main action buttons
    std::vector<std::string> actions;
    std::vector<sf::RectangleShape> buttons;
    std::vector<sf::Text> buttonLabels;

    // role-specific (special) action buttons:
    std::vector<std::string> specialActions;
    std::vector<sf::RectangleShape> specialButtons;
    std::vector<sf::Text> specialLabels;

    // target selection
    std::vector<sf::RectangleShape> targetButtons;
    std::vector<sf::Text> targetLabels;
    std::vector<Player*> otherPlayers;

    // pending action data
    std::string pendingAction;
    Player* pendingTarget;

    // labels
    sf::Text currentPlayer;
    sf::Text coinLabel;
    sf::Text errorLabel;
    sf::Text roleLabel;
    sf::Text gameOverLabel;
    std::vector<sf::Text> playerStatusLabels;

    // ReactionTime UI elements
    bool reactionActive;
    sf::Text reactionTitle;
    sf::RectangleShape buttonReverse;
    sf::Text labelReverse;
    sf::RectangleShape buttonPass;
    sf::Text labelPass;

    // error handling
    sf::Clock errorClock;
    bool errorActive;

public:
    /**
     * @param game the game class that contains the logic of the whole game
     * @param font which sf::Font to use
     * @param wnd the game window
     */
    TurnController(Game& game, sf::Font& font, sf::RenderWindow& wnd);

    /**
     * This method receives an event and check if it's a click event
     * and where the click happened (inside which button)
     * Also it changes the phase of the game accordingly
     * @param event the event that happened
     */
    void handleClick(const sf::Event& event);

    /**
     * The method is the main responsible for changing the phase of the game
     * and call action execution if needed
     */
    void update();

    /**
     * This method is responsible for drawing the screen.
     * What gets drawn is dependant of the other functions logic
     */
    void render();

private:
    /**
     * This method handles the logic behind changing turn
     * from one player to another
     */
    void setupForCurrentPlayer();

    /**
     * This method handles the logic of finishing a turn when
     * entering finished turn phase
     */
    void finishTurn();

    /**
     * This method is responsible for calling the game logic for the relevant action
     * and handling the exception if the action is illegal
     */
    void executeAction();

    /**
     * This method is responsible for showing all the available targets
     * that a player can target and then render() renders it
     */
    void enterTargetMode();

    /**
     * This method clears all the UI elements on the screen
     */
    void clearUI();

    /**
     * This method adjusts the text’s local origin so that its
     * drawing position is centered both horizontally and vertically.
     * After calling this method, setting the text’s position to (x,y)
     * will place its visual center at (x,y).
     * @param text the sf::Text whose origin will be centered
     */
    void centerOrigin(sf::Text& text);

    /**
     * This method checks if the game is finished then it changes
     * the UI to winner screen.
     */
    void finishGame();

    /**
     * This method updates the information of the 'update labels' that
     * appear on the right side of the screen.
     * It also shows the coins in case that Spy uses action 'Peek'.
     */
    void updateStatusLabels();

    /**
     * If executeAction() detects an error it calls
     * this method to handle the appearance of the error
     * on the GUI
     * @param msg the error message
     */
    void showError(const std::string& msg);

    /**
     * If the game is in reaction mode then this method
     * handles how the UI should appear in this case
     */
    void showReactionUI();

    /**
     * If the game exits reaction mode then this method
     * handles how the UI should appear in this case
     */
    void hideReactionUI();

    /**
     * This method calls game to handle the reverse action logic
     * @param reverseAction the action that's being used to counter the original action
     */
    void executeReverseAction(std::string reverseAction);

     /**
     * This method adjusts the text’s local origin so that its
     * drawing position is centered both horizontally and vertically
     * respectively to a given button.
     * @param label the label's button
     * @param button the label to center
     */
    void centerLabel(sf::Text& label, const sf::RectangleShape& button);

};

