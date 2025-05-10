#include "Game.hpp"
#include <iostream>

#include "../Exceptions/IllegalMoveException.hpp"
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

bool Game::checkWinner()
{
    if(alivePlayers.size() == 1)
    {
        game_winner = current_turn;
        return true;
    }

    return false;
}

Player * Game::getWinner() const
{
    return game_winner;
}

void Game::startGame() const
{
    *current_turn = *players.front();
}

Player* Game::getCurrentTurn() const
{
    return current_turn;
}

void Game::nextTurn()
{
    for(int i = 0; i < alivePlayers.size(); i++)
    {
        if(current_turn == alivePlayers[i] && i != alivePlayers.size() - 1)
        {
            current_turn = alivePlayers[i+1];
            if(current_turn->getStatus().selfRescue)
            {
                current_turn->setStatus().selfRescue = false;
                alivePlayers.erase(alivePlayers.begin() + i);
            }
            return;
        }
        else if(current_turn == alivePlayers[i] && i == alivePlayers.size() - 1)
        {
            current_turn = alivePlayers[0];
            if(current_turn->getStatus().selfRescue)
            {
                current_turn->setStatus().selfRescue = false;
                alivePlayers.erase(alivePlayers.begin() + i);
            }
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
    std::vector<string> actions = {"Gather", "Tax", "Bribe", "Arrest", "Sanction", "Coup", "Skip"};
    return actions;
}

bool Game::actionNeedsTarget(string action)
{
    if(action == "Gather" || action == "Tax" || action == "Bribe" || action == "Skip" ||
        action == "Invest")
        return false;
    if (action == "Arrest"   || action == "Sanction" || action == "Coup" || action == "Prevent Tax" ||
        action == "Peek" || "Prevent Arrest" || action == "Reverse Coup")
        return true;

    return false;
}

std::string Game::whyCannotPerform(Player* actor, const std::string& action, Player* pendingTarget)
{
    if(action == "Skip")
        return "";
    if (pendingTarget == nullptr) {
        if (action == "Gather") {
            if (actor->getStatus().isSanctioned)
                return "you are currently sanctioned";
            return "";  // OK
        }
        if (action == "Tax") {
            if (!actor->getStatus().canTax)
                return "you do not have the tax privilege";
            if (actor->getStatus().isSanctioned)
                return "you are currently sanctioned";
            return "";
        }
        if (action == "Bribe") {
            if (actor->getCoins() < 4)
                return "you need at least 4 coins to bribe";
            return "";
        }
        if(action == "Invest")
        {
            if(actor->getClassName() != "Baron")
                return "Actor is not a baron";
            if(actor->getCoins() < 3)
                return "you need at least 3 coins to invest";
            return "";
        }
    } else {
        if (action == "Arrest") {
            if (!actor->getStatus().canArrest)
                return "you cannot perform arrests";
            if (pendingTarget->getStatus().isArrested)
                return pendingTarget->getName() + " is already arrested";
            if (pendingTarget->getCoins() == 0)
                return pendingTarget->getName() + " has no coins to seize";
            return "";
        }
        if (action == "Sanction") {
            if (actor->getCoins() < 3)
                return "you need at least 3 coins to sanction";
            if (pendingTarget->getStatus().isSanctioned)
                return pendingTarget->getName() + " is already sanctioned";
            return "";
        }
        if (action == "Coup") {
            if (actor->getCoins() < 7)
                return "you need at least 7 coins to coup";
            if (!pendingTarget->getStatus().isAlive)
                return pendingTarget->getName() + " is already out of the game";
            return "";
        }
        if(action == "Prevent Tax")
        {
            if(!pendingTarget->getStatus().canTax)
                return "Target already can't tax";
            else if(actor->getClassName() != "Governor")
                return "Actor is not a governor";
            else
            {
                return "";
            }
        }
        if(action == "Peek")
        {
            if(actor->getClassName() != "Spy")
                return "Actor is not a spy";
            return "";
        }
        if(action == "Prevent Arrest")
        {
            if(!pendingTarget->getStatus().canArrest)
                return pendingTarget->getName() + " can't arrest already";
            if(actor->getClassName() != "Spy")
                return "Actor is not a spy";
            return "";
        }
        if(action == "Reverse Coup")
        {
            if(pendingTarget->getStatus().isAlive)
                return pendingTarget->getName() + " is still alive";
            if(!pendingTarget->getStatus().canBeResurrected)
                return pendingTarget->getName() + " can't be resurrected anymore";
            if(actor->getCoins() < 5)
                return "You need at least 5 coins to reverse coup";
            return "";
        }
    }

    return "unrecognized action \"" + action + "\"";
}


bool Game::canPerform(Player* actor, const std::string& action, Player* pendingTarget)
{
    std::string reason = whyCannotPerform(actor, action, pendingTarget);
    if (!reason.empty()) {
        throw IllegalMoveException("Illegal action: " + reason);
    }
    return true;
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
        else if(action == "Invest")
            actor->invest();
    }
    else
    {
        if(action == "Arrest")
            actor->arrest(*pendingTarget);
        else if(action == "Sanction")
        {
            actor->sanction(*pendingTarget);
            if(pendingTarget->getStatus().isInvested)
                pendingTarget->investFailure();
        }
        else if(action == "Coup")
        {
            actor->coup(*pendingTarget);

            if(pendingTarget->getClassName() == "General" && pendingTarget->getCoins() >= 5)
                pendingTarget->setStatus().canBeResurrected = true;
            else
            {
                for(int i = 0; i < alivePlayers.size(); i++)
                {
                    if(alivePlayers[i] == pendingTarget)
                    {
                        alivePlayers.erase(alivePlayers.begin() + i);
                        return;
                    }
                }
            }
        }
        if(action == "Prevent Tax")
            actor->abortTax(*pendingTarget);
        if(action == "Peek")
            actor->peek(*pendingTarget);
        if(action == "Prevent Arrest")
            actor->blockArrest(*pendingTarget);
        if(action == "Reverse Coup")
        {
            actor->reverseCoup(*pendingTarget);
            if(alivePlayers.size() < players.size())
                alivePlayers.push_back(pendingTarget);
        }
    }
}

std::vector<Player*> Game::getAlivePlayers()
{
    return alivePlayers;
}
