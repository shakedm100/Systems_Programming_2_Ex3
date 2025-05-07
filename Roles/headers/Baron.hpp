#pragma once
#include "Player.hpp"

class Baron : public Player
{
private:
    bool invested;

public:
    Baron(string name) : Player(name), invested(false) {}

    void invest() override;
    bool getInvested();
};
