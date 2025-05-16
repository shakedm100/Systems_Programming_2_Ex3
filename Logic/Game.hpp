//shaked1mi@gmail.com
#pragma once
#include "../Roles/headers/Player.hpp"
#include <vector>
#include <string>

struct PendingReverse
{
    std::string actionLabel;
    Player* actor;
    Player* target;
    std::vector<Player*> responders;
    size_t nextResponder = 0;
};

/**
* This class is responsible for all the heavy logic of the game for example
* whose turn is it, whose alive, if the action can be executed etc.
*/
class Game
{
private:
    Player* currentTurn;
    std::vector<Player*> players;
    std::vector<Player*> alivePlayers;
    Player* gameWinner;
    PendingReverse pending;
    bool hasPending;
    size_t currentIndex;
    size_t indexBeforeReaction;


public:
    explicit Game(int playersCount);
    Game(std::vector<Player *> players) : currentTurn(players.at(0)), players(players),
    alivePlayers(players), gameWinner(nullptr), hasPending(false), currentIndex(0), indexBeforeReaction(0) {}
    ~Game();

     /**
     * Prints the current turn, useful for debugging
     */
    void turn() const;

    /**
     * The method prints the current game winner and throws an exception
     * if there is no winner yet
     * @return
     */
    string winner() const;

    /**
     * This method checks if the game has reached winning state
     * @return true if the game is finished, false otherwise
     */
    bool checkWinner();

    /**
     * Returns the player that is currently the winner
     * of the game, if there is none it returns nullptr
     * @return the current winner or nullptr
     */
    Player* getWinner() const;

    /**
     * The method returns the player that's current playing
     * @return the current player
     */
    Player* getCurrentTurn() const;

    /**
     * This method is responsible for the logic behind changing
     * the turn of the current player
     */
    void nextTurn();

    /**
     * @return a list if all the players
     */
    std::vector<Player*> getPlayers();

    /**
     * @return a list of all the base actions
     */
    std::vector<string> getActionNames();

    /**
     * Checks if a given action's name needs a target to activate
     * @param action the action's name
     * @return true if the action needs a target, false otherwise
     */
    bool actionNeedsTarget(string action);

    /**
     * This method handles if a given action can be performed by the
     * current player and on the given target
     * @param actor the player whose playing the action
     * @param action the action that's being played
     * @param pendingTarget the target of the action
     * @return true if it can be performed, false otherwise
     */
    bool canPerform(Player *actor, const std::string& action, Player *pendingTarget);

    /**
     * This method performs a given action on the target
     * @param actor the player that's doing the action
     * @param action the action to perform
     * @param target the target that's being actioned on, nullptr if the action is with a target
     */
    void perform(Player *actor, string action, Player *target);

    /**
     * @return all the players who are currently alive
     */
    std::vector<Player*> getAlivePlayers();

    /**
     * This method setups a new pending action if the current action
     * has a reaction option and another player who can react
     * to that action
     * @param actor the player that's doing the action
     * @param action the given action that's being checked
     * @param target the target that's being actioned on
     */
    void setupPendingReverse(Player* actor, const std::string& action, Player* target);

    /**
     * @return true if there is a pending action that can be reacted to,
     * false otherwise
     */
    bool hasPendingReverse() const;

    /**
     * If there is another player that can react to this action then\
     * advance to that player and return true. False otherwise.
     * @return true if another pending player exists. False otherwise.
     */
    bool advancePendingResponder();

    /**
     * If there is a pending action then clear it.
     */
    void clearPendingReverse();

    /**
     * @return the action name that's pending for a reaction action
     */
    std::string getPendingActionLabel();

    /**
     * This method is responsible for executing a given reaction action
     * @param reverseAction the reaction action to execute
     */
    void performPendingReverse(std::string &reverseAction);

    /**
     * Checks if the pending action is bribe. Helps handling bribe better.
     * @return
     */
    bool isPendingActionBribe() const;

    /**
    * @returns a vector with all the players names
    */
    std::vector<std::string> getPlayersNames() const;

    /**
    * This method is just for main.cpp because normally
    * TurnController handles it with turn phases
    */
    void updateCurrentTurn();

private:
    /**
     * This helper method determines if an action can be executed and
     * return why it can't be performed or an empty string if the action
     * can be performed
     * @param actor the player whose playing the action
     * @param action the action that's being played
     * @param pendingTarget the target of the action
     * @return
     */
    std::string whyCannotPerform(Player* actor, const std::string& action, Player* pendingTarget);

    /**
     * Given a player, this method returns the index of the player in alivePlayers vector
     * @param p the player to find
     * @return the index of the player in alivePlayers
     */
    size_t indexOf(Player* p) const;

    /**
     * This method changes the index for the current player
     */
    void rotateToNextPlayer();

    /**
    * This helper function is given how many players
    * and assigns each player a random role
    */
    void createGame(const int countPlayers);

};
