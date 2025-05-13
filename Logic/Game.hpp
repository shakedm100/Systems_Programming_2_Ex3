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

class Game
{
private:
    Player* current_turn;
    std::vector<Player*> players;
    std::vector<Player*> alivePlayers;
    Player* game_winner;
    PendingReverse pending;
    bool hasPending;
    size_t currentIndex;
    size_t indexBeforeReaction;


public:
    Game() : current_turn(nullptr), players(), alivePlayers(), game_winner(nullptr), hasPending(false),
    currentIndex(0), indexBeforeReaction(0) {};
    explicit Game(const std::vector<Player*>& newPlayers) : current_turn(newPlayers.at(0)), players(newPlayers),
    alivePlayers(newPlayers), game_winner(nullptr), hasPending(false), currentIndex(0), indexBeforeReaction(0) {};

    /**
     * Prints the current turn, useful for debugging
     */
    void turn() const;

    /**
     * The method prints the current game winner and throws an exception
     * if there is not winner yet
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
     * TODO: Continue here
     * @param actor
     * @param action
     * @param target
     */
    void perform(Player *actor, string action, Player *target);
    std::vector<Player*> getAlivePlayers();
    void setupPendingReverse(Player* actor, const std::string& action, Player* target);
    bool hasPendingReverse() const;
    bool advancePendingResponder();
    void clearPendingReverse();
    std::string getPendingActionLabel();
    void performPendingReverse(std::string &reverseAction);
    bool isPendingActionBribe() const;

private:
    std::string whyCannotPerform(Player* actor, const std::string& action, Player* pendingTarget);
    size_t indexOf(Player* p) const;

    /**
     * This method changes the index for the current player
     */
        void rotateToNextPlayer();
};
