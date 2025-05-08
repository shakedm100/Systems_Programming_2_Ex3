#pragma once
#include "Player.hpp"

class Judge : public Player
{
public:
    explicit Judge(const string& name) : Player(name) {};
    string getClassName() override;
};
