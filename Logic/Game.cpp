//shaked1mi@gmail.com
#include "Game.hpp"
#include <iostream>
#include <algorithm>
#include "../Exceptions/IllegalMoveException.hpp"
#include "../Exceptions/NoWinnerException.hpp"
#include "../Roles/headers/Governor.hpp"
#include "../Roles/headers/Spy.hpp"
#include "../Roles/headers/Baron.hpp"
#include "../Roles/headers/General.hpp"
#include "../Roles/headers/Judge.hpp"
#include "../Roles/headers/Merchant.hpp"
#include <bits/random.h>
#include <random>     // for mt19937, uniform_int_distribution
#include <stdexcept>
#include "../GUI/game_window.hpp"

void Game::createGame(const int playersCount)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dice(1,6);

    for(int i = 0; i < playersCount; i++)
    {
        std::string name = promptPlayerName(i+1);
        if (name.empty())
        {
            throw std::invalid_argument("Player naming cancelled!\n");
        }

        int result = dice(mt);
        switch (result)
        {
            case 1:
            {
                Governor* gov = new Governor(name);
                players.push_back(gov);
                alivePlayers.push_back(gov);
                break;
            }
            case 2:
            {
                Spy* spy = new Spy(name);
                players.push_back(spy);
                alivePlayers.push_back(spy);
                break;
            }
            case 3:
            {
                Baron* baron = new Baron(name);
                players.push_back(baron);
                alivePlayers.push_back(baron);
                break;
            }
            case 4:
            {
                General* general = new General(name);
                players.push_back(general);
                alivePlayers.push_back(general);
                break;
            }
            case 5:
            {
                Judge* judge = new Judge(name);
                players.push_back(judge);
                alivePlayers.push_back(judge);
                break;
            }
            case 6:
            {
                Merchant* merchant = new Merchant(name);
                players.push_back(merchant);
                alivePlayers.push_back(merchant);
                break;
            }
        }
    }
}

std::vector<std::string> Game::getPlayersNames() const
{
    vector<string> names;
    for(auto player: players)
    {
        names.push_back(player->getName());
    }

    return names;
}

Game::Game(int playersCount) : game_winner(nullptr), hasPending(false),
                               currentIndex(0), indexBeforeReaction(0)
{
    createGame(playersCount);
    current_turn = players.at(0);
}

Game::~Game()
{
    for(auto p : players)
        delete p;
}

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

Player* Game::getCurrentTurn() const
{
    return current_turn;
}

void Game::nextTurn()
{
    currentIndex = (currentIndex + 1) % alivePlayers.size();
    // pick the player directly
    current_turn = alivePlayers[currentIndex];

    if(current_turn->getClassName() == "Merchant" && current_turn->getCoins() >= 3)
        current_turn->aboveThreeCoins();
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
        action == "Invest" || action == "Peek")
        return false;
    if (action == "Arrest" || action == "Sanction" || action == "Coup")
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
        if(action == "Peek")
        {
            if(actor->getClassName() != "Spy")
                return "Actor is not a spy";
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

void Game::perform(Player *actor, string action, Player *target)
{
    if(target == nullptr)
    {
        if(action == "Gather")
            actor->gather();
        else if(action == "Tax")
            actor->tax();
        else if(action == "Bribe")
            actor->bribe();
        else if(action == "Invest")
            actor->invest();
        if(action == "Peek")
        {
            for(int i = 0; i < alivePlayers.size(); i++)
                actor->peek(*alivePlayers[i]);
            actor->increaseExtraTurns();
        }
    }
    else
    {
        if(action == "Arrest")
            actor->arrest(*target);
        else if(action == "Sanction")
        {
            actor->sanction(*target);
            if(target->getStatus().isInvested)
                target->investFailure();
        }
        else if(action == "Coup")
        {
            actor->coup(*target);
            int i = indexOf(target);
            alivePlayers.erase(alivePlayers.begin() + i);
        }
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
    if (action == "Tax")
        reverserRole = "Governor";
    else if (action == "Arrest")
        reverserRole = "Spy";
    else if (action == "Coup")
        reverserRole = "General";
    else if (action == "Bribe")
        reverserRole = "Judge";
    else {
        hasPending = false;
        return; // not reversible
    }

    indexBeforeReaction = currentIndex;

    if(target != nullptr && target->getClassName() == "General" && action == "Coup" && target->getCoins() >= 5) //Temporary add him to give the option
        alivePlayers.push_back(target);


    PendingReverse reverse{action, actor, target, {}, 0 };
    for (size_t i = 0; i < alivePlayers.size(); ++i)
    {
        Player* current = alivePlayers[(currentIndex + 1 + i) % alivePlayers.size()];
        if (current != actor && current->getClassName() == reverserRole)
        {
            // If the action is 'General' and he doesn't have 5 coins he can't react
            if(current->getClassName() == "General" && current->getCoins() < 5)
                continue;
            reverse.responders.push_back(current);
        }
    }

    if (reverse.responders.empty())
    {
        hasPending = false;
        return;
    }

    if(target != nullptr && target->getClassName() == "General" && action == "Coup") // Erase him because he isn't really alive
        alivePlayers.erase(find(alivePlayers.begin(), alivePlayers.end(), target));

    pending = reverse;
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
        current_turn->preventBribe(*pending.actor);
}

bool Game::isPendingActionBribe() const
{
    if(pending.actionLabel == "Bribe")
        return true;
    return false;
}

