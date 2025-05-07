#pragma once
#include "Roles/headers/Player.hpp"
#include <vector>
#include <string>

using namespace std;

class Game
{
private:
    Player* current_turn;
    vector<Player> players;
    Player* game_winner;

public:
    Game() : current_turn(nullptr), players(), game_winner(nullptr) {};
    explicit Game(vector<Player>& newPlayers) : current_turn(&players.back()), players(newPlayers), game_winner(nullptr) {};
    void turn() const;
    string winner() const;

};
