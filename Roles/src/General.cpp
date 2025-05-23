//shaked1mi@gmail.com
#include "../headers/General.hpp"

string General::getClassName()
{
    return "General";
}

void General::reverseCoup(Player &save)
{
    if(!save.getStatus().isAlive) // Check if he can reverse the current coup
    {
        save.getStatus().isAlive = true; // Return the player to life
        coins -= 5; // Take the coins from the General
    }
}
