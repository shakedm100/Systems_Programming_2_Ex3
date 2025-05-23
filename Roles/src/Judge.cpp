//shaked1mi@gmail.com
#include "../headers/Judge.hpp"

string Judge::getClassName()
{
    return "Judge";
}

void Judge::preventBribe(Player &prevent)
{
    for(int i = 0; i < prevent.getExtraTurns(); i++) // Check if the player has more extra coins
        prevent.decreaseExtraTurns(); // reset all extra turns
}
