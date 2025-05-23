//shaked1mi@gmail.com
#include "../headers/Baron.hpp"

void Baron::invest()
{
    if(coins >= 3) // If he can invest
    {
        coins -= 3; // Take the coins
        status.isInvested = true; // Set the status to investments
        status.holdTurn = true; // Helps with the turn flow
    }
}

string Baron::getClassName()
{
    return "Baron";
}

void Baron::investSuccess()
{
    status.isInvested = false; // Remove the investment status
    coins += 6; // Give the coins to the baron
}

void Baron::investFailure()
{
    status.isInvested = false; // Remove the investment status
    coins++; // Give one pity prize coin
}
