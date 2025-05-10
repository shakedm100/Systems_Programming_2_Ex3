#pragma once
#include "Player.hpp"

class Merchant : public Player
{
public:
    explicit Merchant(const string& name) : Player(name) {};
    string getClassName() override;
    void aboveThreeCoins() override;
};


