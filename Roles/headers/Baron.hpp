#pragma once
#include "Player.hpp"

class Baron : public Player
{
public:
    explicit Baron(const string& name) : Player(name) {}

    /**
    * This method activates the Baron's invest status
    */
    void invest() override;

    /**
    * @return Returns the name of the class
    */
    string getClassName() override;

    /**
    * Activates successful investment and gives 6 coins
    */
    void investSuccess() override;

    /**
    * Activates failure investment and gives 1 coin
    */
    void investFailure() override;
};
