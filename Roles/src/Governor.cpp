//shaked1mi@gmail.com
#include "../headers/Governor.hpp"

bool Governor::tax()
{
    if(!status.isSanctioned) // Check if the Governor can use Tax
    {
        coins+=3; // Governor benefit - take the an extra coin
        return true;
    }

    return false; // The player is sanctioned
}

void Governor::abortTax(Player &player)
{
    player.setCoins() -= 2; // Execute tax abortion and take two coins from the player (not to himself)
}

string Governor::getClassName()
{
    return "Governor";
}
