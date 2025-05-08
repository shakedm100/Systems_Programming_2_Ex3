#include "../headers/Governor.hpp"


bool Governor::tax()
{
    if(!isSanctioned)
    {
        coins+=2;
        return true;
    }

    return false;
}

/**
 * TODO: Implement this function
 * @param role
 */
void Governor::abortTax(Player &role)
{
    Player::abortTax(role);
}

string Governor::getClassName()
{
    return "Governor";
}
