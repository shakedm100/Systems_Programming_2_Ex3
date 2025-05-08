#include "../headers/Baron.hpp"

void Baron::invest()
{
    invested = true;
}

bool Baron::getInvested()
{
    return invested;
}

string Baron::getClassName()
{
    return "Baron";
}
