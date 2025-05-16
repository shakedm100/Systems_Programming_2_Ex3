//shaked1mi@gmail.com
#pragma once
#include "Player.hpp"

class Merchant : public Player
{
public:
    explicit Merchant(const string& name) : Player(name) {};

    /**
    * @return Returns the name of the class
    */
    string getClassName() override;

    /**
    * This method gives one extra coins if a player has
    * more than 3 coins at the start of a turn
    */
    void aboveThreeCoins() override;
};


