#include "../headers/Merchant.hpp"

string Merchant::getClassName() {
    return "Merchant";
}

void Merchant::aboveThreeCoins()
{
    if(coins >= 3)
        coins++;
}
