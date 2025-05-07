#pragma once
#include "Player.hpp"

class Spy : Player
{
    int peek(Player& role) override;
    void blockArrest(Player& role) override;
};
