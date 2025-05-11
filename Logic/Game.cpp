#include "Game.hpp"
#include <iostream>
#include <algorithm>
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

void Game::rotateToNextPlayer()
{
    currentIndex = (currentIndex + 1) % alivePlayers.size();
}

void Game::nextTurn()
{
    rotateToNextPlayer();
    for(int i = 0; i < alivePlayers.size(); i++)
    {
        if(current_turn == alivePlayers[i] && i != alivePlayers.size() - 1)
        {
            current_turn = alivePlayers[i+1];
            break;
        }
        else if(current_turn == alivePlayers[i] && i == alivePlayers.size() - 1)
        {
            current_turn = alivePlayers[0];
            break; //Not necessary here
        }
    }

    if(current_turn->getClassName() == "Merchant" && current_turn->getCoins() >= 3)
        current_turn->aboveThreeCoins();
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

std::string Game::getPendingActionLabel()
{
    if(pending.actionLabel == "Tax")
        return "Prevent Tax";
    else if(pending.actionLabel == "Bribe")
        return "Reverse Bribe";
    else if(pending.actionLabel == "Arrest")
        return "Prevent Arrest";
    else if(pending.actionLabel == "Sanction")
        return "Prevent Sanction";
    else if(pending.actionLabel == "Coup")
        return "Reverse Coup";
    else
        return "";
}

bool Game::actionNeedsTarget(string action)
{
    if(action == "Gather" || action == "Tax" || action == "Bribe" || action == "Skip" ||
        action == "Invest")
        return false;
    if (action == "Arrest" || action == "Sanction" || action == "Coup" ||
        action == "Peek")
        return true;

    return false;
}

std::string Game::whyCannotPerform(Player* actor, const std::string& action, Player* pendingTarget)
{
    if(actor->getCoins() >= 10 && action != "Coup")
        return "Above 10 coins you must coup!";
    if(action == "Skip")
        return "";
    if (pendingTarget == nullptr) {
        if (action == "Gather") {
            if (actor->getStatus().isSanctioned)
                return "you are currently sanctioned";
            return "";  // OK
        }
        if (action == "Tax") {
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
            if (pendingTarget->getCoins() == 0)
                return pendingTarget->getName() + " has no coins to seize";
            if(pendingTarget->getClassName() == "Merchant" && pendingTarget->getCoins() < 2)
                return pendingTarget->getName() + " has not enough coins to seize";
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
        if(action == "Peek")
        {
            if(actor->getClassName() != "Spy")
                return "Actor is not a spy";
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
            int i = indexOf(pendingTarget);
            alivePlayers.erase(alivePlayers.begin() + i);
        }
        if(action == "Peek")
            actor->peek(*pendingTarget);
    }
}

std::vector<Player*> Game::getAlivePlayers()
{
    return alivePlayers;
}

size_t Game::indexOf(Player* p) const
{
    return std::distance(alivePlayers.begin(),std::find(alivePlayers.begin(), alivePlayers.end(), p));
}


void Game::setupPendingReverse(Player* actor, const std::string& action, Player* target) {
    // determine reverser role via if/else
    std::string reverserRole;
    if (action == "Tax") {
        reverserRole = "Governor";
    } else if (action == "Arrest") {
        reverserRole = "Spy";
    } else if (action == "Coup") {
        reverserRole = "General";
    } else if (action == "Bribe") {
        reverserRole = "Judge";
    } else {
        hasPending = false;
        return; // not reversible
    }

    indexBeforeReaction = currentIndex;

    if(target != nullptr && target->getClassName() == "General" && action == "Coup" && target->getCoins() >= 5) //Temporary add him to give the option
        alivePlayers.push_back(target);


    PendingReverse rev{ action, actor, target, {}, 0 };
    for (size_t i = 0; i < alivePlayers.size(); ++i) {
        Player* p = alivePlayers[(currentIndex + 1 + i) % alivePlayers.size()];
        if (p != actor && p->getClassName() == reverserRole) {
            rev.responders.push_back(p);
        }
    }
    if (rev.responders.empty()) {
        hasPending = false;
        return;
    }

    if(target != nullptr && target->getClassName() == "General" && action == "Coup") // Erase him because he isn't really alive
        alivePlayers.erase(find(alivePlayers.begin(), alivePlayers.end(), target));

    pending = rev;
    hasPending = true;
    currentIndex = indexOf(pending.responders[0]);
    current_turn = alivePlayers[currentIndex];
}

bool Game::hasPendingReverse() const { return hasPending; }


bool Game::advancePendingResponder()
{
    if (!hasPending) return false;
    ++pending.nextResponder;
    if (pending.nextResponder < pending.responders.size()) {
        currentIndex = indexOf(pending.responders[pending.nextResponder]);
        current_turn = alivePlayers[currentIndex];
        return true;
    }
    return false;
}

void Game::clearPendingReverse()
{
    if (!hasPending) return;
    // reset pending state
    pending = PendingReverse();
    hasPending = false;
    // restore to exactly the actor’s index before reaction
    currentIndex = indexBeforeReaction;
    current_turn = alivePlayers[currentIndex];
}

void Game::performPendingReverse(std::string &reverseAction)
{
    if(reverseAction == "Prevent Tax")
        current_turn->abortTax(*pending.actor);
    if(reverseAction == "Prevent Arrest")
    {
        current_turn->blockArrest(*pending.actor);
        pending.target->setCoins()++;
    }
    if(reverseAction == "Reverse Coup")
    {
        current_turn->reverseCoup(*pending.target);
        alivePlayers.push_back(pending.target);
    }
    if(reverseAction == "Reverse Bribe")
    {
        current_turn->preventBribe(*pending.actor);
    }
}

bool Game::isPendingActionBribe() const
{
    if(pending.actionLabel == "Bribe")
        return true;
    return false;
}

