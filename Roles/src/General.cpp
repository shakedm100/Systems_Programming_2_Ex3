#include "../headers/General.hpp"

string General::getClassName()
{
    return "General";
}

void General::reverseCoup(Player &save)
{
    if(!save.getStatus().isAlive)
    {
        save.setStatus().isAlive = true;
        coins -= 5;
    }
}
