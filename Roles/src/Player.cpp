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

bool Player::arrest(Player& Player)
{
    if(Player.coins > 0 && !Player.status.isArrested)
    {
        Player.coins--;
        this->coins++;
        return true;
    }

    return false;
}

bool Player::sanction(Player& Player)
{
    if(this->coins >= 3)
    {
        Player.status.isSanctioned = true;
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
        return true;
    }
    return false;
}

int Player::getCoins() const
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

void Player::abortTax(Player& Player)
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

void Player::preventCoup()
{
    throw NotImplementedException("Not a General");
}

void Player::preventBribe()
{
    throw NotImplementedException("Not a Judge");
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
}

std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << player.name << "\n";
    return os;
}

















