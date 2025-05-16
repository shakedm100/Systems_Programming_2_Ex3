//shaked1mi@gmail.com
#pragma once
#include "Player.hpp"

class Judge : public Player
{
public:
    explicit Judge(const string& name) : Player(name) {};

    /**
    * @return Returns the name of the class
    */
    string getClassName() override;

    /**
    * This method reverts a bribe done by another player
    * and takes away all of his extra turns
    * @param prevent the player whose bribe is being reverted
    */
    void preventBribe(Player &prevent) override;
};
