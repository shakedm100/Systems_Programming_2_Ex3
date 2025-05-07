#pragma once
#include "Player.hpp"

class Spy : public Player
{
    int peek(Player& role) override;
    void blockArrest(Player& role) override;
};
