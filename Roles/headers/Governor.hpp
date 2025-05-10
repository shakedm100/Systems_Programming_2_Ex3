#pragma once
#include "Player.hpp"

class Governor : public Player
{

    public:
    explicit Governor(const string& name) : Player(name) {}

    bool tax() override;
    void abortTax(Player &player) override;
    string getClassName() override;
};