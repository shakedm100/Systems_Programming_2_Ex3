#include "../headers/Player.hpp"
#include <iostream>
#include "../../Exceptions/NotImplementedException.hpp"

void Player::gather()
{
    if(!status.isSanctioned)
        coins++;
}

bool Player::tax()
{
    if(!status.isSanctioned)
    {
        coins+=2;
        return true;
    }

    return false;
}

bool Player::bribe()
{
    if(coins >= 4)
    {
        coins-=4;
        extraTurns = 2;
        return true;
    }

    return false;
}

bool Player::arrest(Player& player)
{
    if(player.getClassName() == "General")
        return true; //Do nothing but waste turn

    if(player.getClassName() == "Merchant")
    {
        player.coins -= 2;
        return true;
    }

    if(player.coins > 0 && !player.status.isArrested)
    {
        player.coins--;
        this->coins++;
        return true;
    }

    return false;
}

bool Player::sanction(Player& player)
{
    if(player.getClassName() == "Judge")
    {
        if(this->coins >= 4)
        {
            player.status.isSanctioned = true;
            this->coins -= 4;
            return true;
        }
        else
            return false;
    }

    if(this->coins >= 3)
    {
        player.status.isSanctioned = true;
        this->coins -= 3;
        return true;
    }

    return false;
}

bool Player::coup(Player& player) const
{
    if(this->coins >= 7)
    {
        player.status.isAlive = false;
        player.setStatus().canBeResurrected = true;
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

bool Player::getCanArrest() const
{
    return status.canArrest;
}

bool &Player::setCanArrest()
{
    return status.canArrest;
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

void Player::preventBribe()
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

Status & Player::setStatus()
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
    status.canArrest = true;
    status.canTax = true;
    status.isArrested = false;
    status.isSanctioned = false;
    status.holdTurn = false;
}

std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << player.name << "\n";
    return os;
}

















