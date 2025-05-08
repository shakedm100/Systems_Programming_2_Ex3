#include "Game.hpp"
#include <iostream>
#include "Exceptions/NoWinnerException.hpp"

void Game::turn() const
{
    cout << current_turn << endl;
}

string Game::winner() const
{
    if(game_winner != nullptr)
        return game_winner->getName();

    throw NoWinnerException("No winner yet in this game");
}

void Game::startGame() const
{
    *current_turn = *players.front();
}

Player* Game::getCurrentTurn() const {
    return current_turn;
}

void Game::nextTurn()
{
    for(int i = 0; i < players.size(); i++)
    {
        if(current_turn == players[i] && i != players.size() - 1)
        {
            current_turn = players[i+1];
        }
        else if(current_turn == players[i] && i == players.size() - 1)
        {
            current_turn = players[0];
        }
    }
}

void Game::endGame() const
{
    *game_winner = *current_turn;
}

std::vector<Player*> Game::getPlayers()
{
    return players;
}




