#pragma once
#include "Player.hpp"


class General : public Player
{
public:
    explicit General(const string& name) : Player(name) {};
    string getClassName() override;
    void reverseCoup(Player &save) override;
};


