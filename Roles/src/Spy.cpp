#include "../headers/Spy.hpp"

int Spy::peek(Player& role)
{
    extraTurns++;
    return role.getCoins();
}

void Spy::blockArrest(Player& role)
{
    role.setCoins() -= 1;
}

string Spy::getClassName()
{
    return "Spy";
}
