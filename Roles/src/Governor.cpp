//shaked1mi@gmail.com
#include "../headers/Governor.hpp"

bool Governor::tax()
{
    if(!status.isSanctioned)
    {
        coins+=3;
        return true;
    }

    return false;
}

void Governor::abortTax(Player &player)
{
    player.setCoins() -= 2;
}

string Governor::getClassName()
{
    return "Governor";
}
