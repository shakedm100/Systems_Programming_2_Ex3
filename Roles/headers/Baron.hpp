#pragma once
#include "Player.hpp"

class Baron : public Player
{
private:

public:
    explicit Baron(const string& name) : Player(name) {}

    void invest() override;
    string getClassName() override;
    void investSuccess() override;
    void investFailure() override;
};
