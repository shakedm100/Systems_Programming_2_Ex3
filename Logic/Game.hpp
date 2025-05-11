#pragma once
#include "../Roles/headers/Player.hpp"
#include <vector>
#include <string>

struct PendingReverse {
    std::string actionLabel;
    Player*    actor;
    Player*    target;
    std::vector<Player*> responders;
    size_t     nextResponder = 0;
};

class Game
{
private:
    Player* current_turn;
    std::vector<Player*> players;
    std::vector<Player*> alivePlayers;
    Player* game_winner;
    PendingReverse pending;
    bool hasPending = false;
    size_t currentIndex = 0;
    size_t indexBeforeReaction = 0;


public:
    Game() : current_turn(nullptr), players(), alivePlayers(), game_winner(nullptr) {};
    explicit Game(const std::vector<Player*>& newPlayers) : current_turn(newPlayers.at(0)), players(newPlayers), alivePlayers(newPlayers), game_winner(nullptr) {};
    void turn() const;
    string winner() const;
    bool checkWinner();
    Player* getWinner() const;
    void startGame() const;
    Player * getCurrentTurn() const;
    void rotateToNextPlayer();
    void nextTurn();
    void endGame() const;
    std::vector<Player*> getPlayers();
    std::vector<string> getActionNames();
    bool actionNeedsTarget(string basicString);
    bool canPerform(Player *actor, const std::string& action, Player *pendingTarget);
    void perform(Player *pPlayer, string action, Player *pPlayer1);
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

};
