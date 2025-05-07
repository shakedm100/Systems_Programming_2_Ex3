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
