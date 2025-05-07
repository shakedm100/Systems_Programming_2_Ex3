#pragma once
#include "Player.hpp"

class Governor : Player
{

    public:
    Governor(string name) : Player(name) {}

    bool tax() override;
    void abortTax(Player &role) override;
};