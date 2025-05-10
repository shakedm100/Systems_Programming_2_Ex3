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
    player.setStatus().canTax = false;
}

string Governor::getClassName()
{
    return "Governor";
}
