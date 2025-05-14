//shaked1mi@gmail.com
#include "../headers/Baron.hpp"

void Baron::invest()
{
    if(coins >= 3)
    {
        coins -= 3;
        status.isInvested = true;
        status.holdTurn = true;
    }
}

string Baron::getClassName()
{
    return "Baron";
}

void Baron::investSuccess()
{
    status.isInvested = false;
    coins += 6;
}

void Baron::investFailure()
{
    status.isInvested = false;
    coins++;
}
