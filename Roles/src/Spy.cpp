#include "../headers/Spy.hpp"

int Spy::peek(Player& role)
{
    extraTurns++;
    return role.getCoins();
}

void Spy::blockArrest(Player& role)
{
    extraTurns++;
    role.setCanArrest() = false;
}

string Spy::getClassName()
{
    return "Spy";
}
