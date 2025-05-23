//shaked1mi@gmail.com
#include "../headers/Merchant.hpp"

string Merchant::getClassName() {
    return "Merchant";
}

void Merchant::aboveThreeCoins()
{
    if(coins >= 3) // Check if the Merchants bonus can be activated
        coins++; // Give one extra coin
}
