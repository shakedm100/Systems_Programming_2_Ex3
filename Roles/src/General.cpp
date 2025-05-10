#include "../headers/General.hpp"

string General::getClassName()
{
    return "General";
}

void General::reverseCoup(Player &save)
{
    if(save.getStatus().canBeResurrected)
    {
        save.setStatus().isAlive = true;
        save.setStatus().canBeResurrected = false;
        coins -= 5;
    }
}
