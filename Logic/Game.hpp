#pragma once
#include "../Roles/headers/Player.hpp"
#include <vector>
#include <string>

class Game
{
private:
    Player* current_turn;
    std::vector<Player*> players;
    std::vector<Player*> alivePlayers;
    Player* game_winner;

public:
    Game() : current_turn(nullptr), players(), alivePlayers(), game_winner(nullptr) {};
    explicit Game(const std::vector<Player*>& newPlayers) : current_turn(newPlayers.back()), players(newPlayers), alivePlayers(newPlayers), game_winner(nullptr) {};
    void turn() const;
    string winner() const;
    bool checkWinner();
    Player* getWinner() const;
    void startGame() const;
    Player * getCurrentTurn() const;
    void nextTurn();
    void endGame() const;
    std::vector<Player*> getPlayers();
    std::vector<string> getActionNames();
    bool actionNeedsTarget(string basicString);
    bool canPerform(Player *actor, string basicString, Player *pendingTarget);
    void perform(Player *pPlayer, string basicString, Player *pPlayer1);
    std::vector<Player*> getAlivePlayers();

};
