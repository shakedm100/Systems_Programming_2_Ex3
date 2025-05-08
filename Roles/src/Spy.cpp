#include "../headers/Spy.hpp"

int Spy::peek(Player& role)
{
    return role.getCoins();
}

void Spy::blockArrest(Player& role)
{
    role.setCanArrest() = false;
}

string Spy::getClassName()
{
    return "Spy";
}
