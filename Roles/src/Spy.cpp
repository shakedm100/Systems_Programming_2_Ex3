//shaked1mi@gmail.com
#include "../headers/Spy.hpp"

int Spy::peek(Player& role)
{
    return role.getCoins();
}

void Spy::blockArrest(Player& role)
{
    role.setCoins() -= 1; // Reverse the arrest action and take the coin from the player who tried to arrest
}

string Spy::getClassName()
{
    return "Spy";
}
