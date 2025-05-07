#pragma once
#include "Player.hpp"

class Baron : Player
{
private:
    bool invested;

public:
    Baron(string name) : Player(name) {}

    void invest() override;
    bool getInvested();
};
