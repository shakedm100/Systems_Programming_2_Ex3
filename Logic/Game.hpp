#pragma once
#include "../Roles/headers/Player.hpp"
#include <vector>
#include <string>

class Game
{
private:
    Player* current_turn;
    std::vector<Player*> players;
    Player* game_winner;

public:
    Game() : current_turn(nullptr), players(), game_winner(nullptr) {};
    explicit Game(const std::vector<Player*>& newPlayers) : current_turn(newPlayers.back()), players(newPlayers), game_winner(nullptr) {};
    void turn() const;
    string winner() const;
    void startGame() const;
    Player * getCurrentTurn() const;
    void nextTurn();
    void endGame() const;
    std::vector<Player*> getPlayers();
    std::vector<string> getActionNames();

    bool actionNeedsTarget(string basicString);

    bool canPerform(Player *pPlayer, string basicString, Player *pPlayer1);

    void perform(Player *pPlayer, string basicString, Player *pPlayer1);
};
