#pragma once
#include "Player.hpp"

class Spy : public Player
{
    public:
    explicit Spy(const string& name) : Player(name) {};
    int peek(Player& role) override;
    void blockArrest(Player& role) override;
};
