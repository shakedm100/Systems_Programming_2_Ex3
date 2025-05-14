//shaked1mi@gmail.com
#include "../headers/General.hpp"

string General::getClassName()
{
    return "General";
}

void General::reverseCoup(Player &save)
{
    if(!save.getStatus().isAlive)
    {
        save.getStatus().isAlive = true;
        coins -= 5;
    }
}
