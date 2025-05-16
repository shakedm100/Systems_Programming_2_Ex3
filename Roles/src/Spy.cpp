//shaked1mi@gmail.com
#include "../headers/Spy.hpp"

int Spy::peek(Player& role)
{
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
