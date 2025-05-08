#pragma once
#include "Player.hpp"

class Baron : public Player
{
private:
    bool invested;

public:
    explicit Baron(const string& name) : Player(name), invested(false) {}

    void invest() override;
    bool getInvested();
    string getClassName() override;
};
