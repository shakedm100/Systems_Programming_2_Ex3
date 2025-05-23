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

void Game::createGame(const int countPlayers)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dice(1,6); // Construct a uniform distribution object that generates between 1-6

    for(int i = 0; i < countPlayers; i++)
    {
        std::string name = promptPlayerName(i+1); // Show a screen that asks for the player's name
        if (name.empty())
        {
            throw std::invalid_argument("Player naming cancelled!\n");
        }

        int result = dice(mt); // Generate a random number between 1 and 6 using a uniform distribution
        switch (result) // Choose which type to generate based on the result
        {
            case 1:
            {
                Governor* gov = new Governor(name);
                players.push_back(gov); // Add to players
                alivePlayers.push_back(gov); // Add to alivePlayers
                break;
            }
            case 2:
            {
                Spy* spy = new Spy(name);
                players.push_back(spy); // Add to players
                alivePlayers.push_back(spy); // Add to alivePlayers
                break;
            }
            case 3:
            {
                Baron* baron = new Baron(name);
                players.push_back(baron); // Add to players
                alivePlayers.push_back(baron); // Add to alivePlayers
                break;
            }
            case 4:
            {
                General* general = new General(name);
                players.push_back(general); // Add to players
                alivePlayers.push_back(general); // Add to alivePlayers
                break;
            }
            case 5:
            {
                Judge* judge = new Judge(name);
                players.push_back(judge); // Add to players
                alivePlayers.push_back(judge); // Add to alivePlayers
                break;
            }
            case 6:
            {
                Merchant* merchant = new Merchant(name);
                players.push_back(merchant); // Add to players
                alivePlayers.push_back(merchant); // Add to alivePlayers
                break;
            }
        }
    }
}

std::vector<std::string> Game::getPlayersNames() const
{
    vector<string> names;  // Create a vector of names
    for(auto player: alivePlayers)
    {
        names.push_back(player->getName()); // Add the name to the list
    }

    return names; // Return the list
}

void Game::updateCurrentTurn()
{
    currentTurn = alivePlayers[currentIndex]; // Update the current turn based on the given index
}

Game::Game(int playersCount) : gameWinner(nullptr), hasPending(false),
                               currentIndex(0), indexBeforeReaction(0)
{
    createGame(playersCount); // Create all the players
    currentTurn = players.at(0); // Set the first player as the first turn
}

Game::~Game()
{
    for(auto p : players)
        delete p;
}

void Game::turn() const
{
    cout << *currentTurn << endl; // Print the current player
}

string Game::winner() const
{
    if(gameWinner != nullptr)
        return gameWinner->getName(); // Return the winner

    throw NoWinnerException("No winner yet in this game"); // No winner - throw exception!
}

bool Game::checkWinner()
{
    if(alivePlayers.size() == 1) // Check for winning condition
    {
        gameWinner = currentTurn; // Set the winner!
        return true;
    }

    return false;
}

Player * Game::getWinner() const
{
    return gameWinner;
}

Player* Game::getCurrentTurn() const
{
    return currentTurn;
}

void Game::nextTurn()
{
    currentIndex = (currentIndex + 1) % alivePlayers.size(); // Forward the players by one, if the current player go back to the first index
    // pick the player directly
    currentTurn = alivePlayers[currentIndex];

    if(currentTurn->getClassName() == "Merchant" && currentTurn->getCoins() >= 3) // Check the Merchant's passive
        currentTurn->aboveThreeCoins(); // Grant an extra coin
}

std::vector<Player*> Game::getPlayers()
{
    return players;
}

std::vector<string> Game::getActionNames()
{
    std::vector<string> actions = {"Gather", "Tax", "Bribe", "Arrest", "Sanction", "Coup", "Skip"}; // All the actions that show at the top side of the screen
    return actions;
}

std::string Game::getPendingActionLabel()
{
    // Return the reverse action of the current pending action
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
        return ""; // Not a reversable action
}

bool Game::actionNeedsTarget(string action)
{
    if(action == "Gather" || action == "Tax" || action == "Bribe" || action == "Skip" ||
        action == "Invest" || action == "Peek") // Actions that don't target another player
        return false;
    if (action == "Arrest" || action == "Sanction" || action == "Coup") // Actions that do target another player
        return true;

    return false;
}

std::string Game::whyCannotPerform(Player* actor, const std::string& action, Player* pendingTarget)
{
    if(actor != currentTurn) // Not the current turn
        return "Not the current player's turn";
    if(actor->getCoins() >= 10 && action != "Coup") // Must coup conditions
        return "Above 10 coins you must coup!";
    if(action == "Skip") // Player can always skip
        return "";
    if (pendingTarget == nullptr) { // An action that has no target
        if (action == "Gather") { // Player wants to gather
            if (actor->getStatus().isSanctioned) // Check sanction condition
                return "you are currently sanctioned";
            return "";  // OK
        }
        if (action == "Tax") { // Player wants to Tax
            if (actor->getStatus().isSanctioned) // Check sanction condition
                return "you are currently sanctioned";
            return "";
        }
        if (action == "Bribe") { // Player wants to Bribe
            if (actor->getCoins() < 4) // Check bribe condition
                return "you need at least 4 coins to bribe";
            return "";
        }
        if(action == "Invest") // Player wants to Invest
        {
            if(actor->getClassName() != "Baron") // Make sure it's a Baron
                return "Actor is not a baron";
            if(actor->getCoins() < 3) // // Check coins amount
                return "you need at least 3 coins to invest";
            return "";
        }
        if(action == "Peek") // Player wants to Peek
        {
            if(actor->getClassName() != "Spy") // Make sure it's a Spy
                return "Actor is not a spy";
            return "";
        }
    } else { // An action with a target
        if (action == "Arrest") { // Player wants to Arrest
            if (pendingTarget->getCoins() == 0) // Make sure the targeted player has enough coins
                return pendingTarget->getName() + " has no coins to seize";
            if(pendingTarget->getClassName() == "Merchant" && pendingTarget->getCoins() < 2) // Check Merchant specific condition
                return pendingTarget->getName() + " has not enough coins to seize";
            return "";
        }
        if (action == "Sanction") { // Player wants to Sanction
            if (actor->getCoins() < 3) // Check how many coins the player has
                return "you need at least 3 coins to sanction";
            if (pendingTarget->getStatus().isSanctioned) // Check sanction condition
                return pendingTarget->getName() + " is already sanctioned";
            return "";
        }
        if (action == "Coup") {
            if (actor->getCoins() < 7) // Player wants to Coup
                return "you need at least 7 coins to coup";
            if (!pendingTarget->getStatus().isAlive) // Make sure the target player is still alive
                return pendingTarget->getName() + " is already out of the game";
            return "";
        }
        if(action == "Peek") // Player wants to Peek
        {
            if(actor->getClassName() != "Spy") // Make sure it's a Spy
                return "Actor is not a spy";
            return "";
        }
    }

    return "unrecognized action \"" + action + "\""; // If it's not a previously known action
}


bool Game::canPerform(Player* actor, const std::string& action, Player* pendingTarget)
{
    std::string reason = whyCannotPerform(actor, action, pendingTarget); // Check if the action can be performed
    if (!reason.empty()) {
        throw IllegalMoveException("Illegal action: " + reason); // Cannot be performed + reason why
    }
    return true;
}

void Game::perform(Player *actor, string action, Player *target)
{
    if(target == nullptr) // An action that has no target
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
    else // An action that has a target
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
    // Get how many indexes are between the first index of alivePlayers and and the
    // index of p - essencialy get the index of p in the list
    return std::distance(alivePlayers.begin(),std::find(alivePlayers.begin(), alivePlayers.end(), p)); 
}


void Game::setupPendingReverse(Player* actor, const std::string& action, Player* target) {
    // Determine reverser role via if/else
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

    // If a coup was initiated on a General, add him to this list temporarily
    // to let him save himself if possible
    if(target != nullptr && target->getClassName() == "General" && action == "Coup" && target->getCoins() >= 5) //Temporary add him to give the option
        alivePlayers.push_back(target);


    // Setup reverse action
    PendingReverse reverse{action, actor, target, {}, 0 };
    for (size_t i = 0; i < alivePlayers.size(); ++i)
    {
        Player* current = alivePlayers[(currentIndex + 1 + i) % alivePlayers.size()]; // Run through all the players
        if (current != actor && current->getClassName() == reverserRole) // If the current index can Reverse the current action
        {
            // If the action is 'General' and he doesn't have 5 coins he can't react
            if(current->getClassName() == "General" && current->getCoins() < 5)
                continue;
            reverse.responders.push_back(current); // Player can react, add him to possible responders
        }
    }

    if (reverse.responders.empty()) // If there are no possible responders
    {
        hasPending = false;
        return;
    }

    // Erase him because he isn't really alive - if he saves himself then he will come back to life
    if(target != nullptr && target->getClassName() == "General" && action == "Coup") 
        alivePlayers.erase(find(alivePlayers.begin(), alivePlayers.end(), target)); 

    pending = reverse; // Finish setting up the reverse action
    hasPending = true;
    currentIndex = indexOf(pending.responders[0]); // Change the index to the first possible responder
    currentTurn = alivePlayers[currentIndex]; // Change turn
}

bool Game::hasPendingReverse() const { return hasPending; }


bool Game::advancePendingResponder()
{
    if (!hasPending) return false; // Check if there is a pending action
    ++pending.nextResponder; // Advance to next possible responder
    if (pending.nextResponder < pending.responders.size()) { // Check if there is a new responder
        currentIndex = indexOf(pending.responders[pending.nextResponder]); // Advance to that responder
        currentTurn = alivePlayers[currentIndex]; // Change to his turn
        return true;
    }
    return false;
}

void Game::clearPendingReverse()
{
    if (!hasPending) return;
    // reset pending state
    pending = PendingReverse(); // Reset pending
    hasPending = false; // No more pending turn
    // restore to exactly the actor’s index before reaction
    currentIndex = indexBeforeReaction; // Return to the index that was before the reactions
    currentTurn = alivePlayers[currentIndex]; // Return that turn
}

void Game::performPendingReverse(std::string &reverseAction)
{
    // Perform a reversiable action
    if(reverseAction == "Prevent Tax")
        currentTurn->abortTax(*pending.actor); // Cancel the player's tax
    if(reverseAction == "Prevent Arrest")
    {
        currentTurn->blockArrest(*pending.actor); // Block his arrest action
        pending.target->setCoins()++; // Return the coin to the player
    }
    if(reverseAction == "Reverse Coup")
    {
        currentTurn->reverseCoup(*pending.target);
        alivePlayers.push_back(pending.target); // Return him to alive players
    }
    if(reverseAction == "Reverse Bribe")
        currentTurn->preventBribe(*pending.actor);
}

bool Game::isPendingActionBribe() const
{
    if(pending.actionLabel == "Bribe")
        return true;
    return false;
}

