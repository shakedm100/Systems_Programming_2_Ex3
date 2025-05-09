#include "Game.hpp"
#include <iostream>
#include "../Exceptions/NoWinnerException.hpp"

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
            return;
        }
        else if(current_turn == players[i] && i == players.size() - 1)
        {
            current_turn = players[0];
            return;
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

std::vector<string> Game::getActionNames()
{
    std::vector<string> actions = {"Gather", "Tax", "Bribe", "Arrest", "Sanction", "Coup"};
    return actions;
}

bool Game::actionNeedsTarget(string action)
{
    if(action == "Gather" || action == "Tax" || action == "Bribe")
        return false;
    if (action == "Arrest"   || action == "Sanction" || action == "Coup")
        return true;

    return false;
}

bool Game::canPerform(Player *actor, string action, Player *pendingTarget)
{
    if(pendingTarget == nullptr)
    {
        if(action == "Gather" && !actor->getStatus().isSanctioned)
            return true;
        else if(action == "Tax" && actor->getStatus().canTax && !actor->getStatus().isSanctioned)
        {
            return true;
        }
        else if(action == "Bribe" && actor->getCoins() >= 4)
            return true;
    }
    else
    {
        if(action == "Arrest" && actor->getStatus().canArrest && !pendingTarget->getStatus().isArrested && pendingTarget->getCoins() > 0)
            return true;
        else if(action == "Sanction" && !pendingTarget->getStatus().isSanctioned && actor->getCoins() >= 3)
            return true;
        else if(action == "Coup" && actor->getCoins() >= 7 && pendingTarget->getStatus().isAlive)
            return true;
    }
    return false;
}

void Game::perform(Player *actor, string action, Player *pendingTarget)
{
    if(pendingTarget == nullptr)
    {
        if(action == "Gather")
            actor->gather();
        else if(action == "Tax")
            actor->tax();
        else if(action == "Bribe")
            actor->bribe();
    }
    else
    {
        if(action == "Arrest")
            actor->arrest(*pendingTarget);
        else if(action == "Sanction")
            actor->sanction(*pendingTarget);
        else if(action == "Coup")
            actor->coup(*pendingTarget);
    }
}
