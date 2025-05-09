#include "../headers/Governor.hpp"


bool Governor::tax()
{
    if(!status.isSanctioned)
    {
        coins+=2;
        return true;
    }

    return false;
}

void Governor::abortTax(Player &role)
{
    role.setStatus().canTax = false;
}

string Governor::getClassName()
{
    return "Governor";
}
