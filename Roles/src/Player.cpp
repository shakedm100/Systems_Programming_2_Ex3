//shaked1mi@gmail.com
#include "../headers/Player.hpp"
#include <iostream>
#include "../../Exceptions/NotImplementedException.hpp"

void Player::gather()
{
    if(!status.isSanctioned) // If the player is not sanctioned
        coins++; // Give one coin to the current player
}

bool Player::tax()
{
    if(!status.isSanctioned) // If the player is not sanctioned
    {
        coins+=2; // Give two coins to the current player
        return true;
    }

    return false;
}

bool Player::bribe()
{
    if(coins >= 4) // Check if the current player can bribe
    {
        coins-=4; // Take 4 coins for the player
        extraTurns = 2; // Activate bribe
        return true;
    }

    return false;
}

bool Player::arrest(Player& player)
{
    if(player.getClassName() == "General") // Generals can't be arrested
        return true; //Do nothing but waste turn

    if(player.getClassName() == "Merchant") // if the arrested player is a Merchant
    {
        player.coins -= 2; // Take two coins from the player (not to yourself)
        return true;
    }

    if(player.coins > 0) // If not a General or a Merchant, check if the player has coins to take
    {
        player.coins--; // Take one coin
        this->coins++; // Give yourself
        return true;
    }

    return false;
}

bool Player::sanction(Player& player)
{
    if(player.getClassName() == "Judge")
    {
        if(this->coins >= 4) // If the sanctioned player is a judge check if the current player can sanction him
        {
            player.status.isSanctioned = true; // Change sanction status
            this->coins -= 4; // Remove coins from the current player
            return true;
        }
        else
            return false;
    }

    if(this->coins >= 3) // Check if the current player can sanction
    {
        player.status.isSanctioned = true; // Change sanction status
        this->coins -= 3; // Remove coins from the current player
        return true;
    }

    return false;
}

bool Player::coup(Player& player) const
{
    if(this->coins >= 7) // Check if the current player can coup
    {
        player.status.isAlive = false; // Remove the player from the game
        return true;
    }
    return false;
}

int Player::getCoins() const
{
    return coins;
}

int & Player::setCoins()
{
    return coins;
}

string Player::getName() const
{
    return name;
}

void Player::abortTax(Player& player)
{
    throw NotImplementedException("Not a Governor");
}

int Player::peek(Player& role)
{
    throw NotImplementedException("Not a Spy");
}

void Player::blockArrest(Player& role)
{
    throw NotImplementedException("Not a Spy");
}

void Player::invest()
{
    throw NotImplementedException("Not a Baron");
}

void Player::investSuccess()
{
    throw NotImplementedException("Not a Baron");
}

void Player::investFailure()
{
    throw NotImplementedException("Not a Baron");
}

void Player::reverseCoup(Player &save)
{
    throw NotImplementedException("Not a General");
}

void Player::preventBribe(Player &prevent)
{
    throw NotImplementedException("Not a Judge");
}

void Player::aboveThreeCoins()
{
    throw NotImplementedException("Not a Merchant");
}

Status Player::getStatus() const
{
    return status;
}

Status & Player::getStatus()
{
    return status;
}

int Player::getExtraTurns() const
{
    return extraTurns;
}

void Player::decreaseExtraTurns()
{
    extraTurns--;
}

void Player::increaseExtraTurns()
{
    extraTurns++;
}

void Player::clearStatusEffects()
{
    status.isSanctioned = false; // Remove sanction effect
    status.holdTurn = false; // Remove holdTurn effect
}

std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << player.name << "\n" << player.coins << "\n"; // Print the player's name and his current coins amount
    return os;
}

















