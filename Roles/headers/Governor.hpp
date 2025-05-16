//shaked1mi@gmail.com
#pragma once
#include "Player.hpp"

class Governor : public Player
{

    public:
    explicit Governor(const string& name) : Player(name) {}

    /**
    * This method overrides the normal tax
    * and gives the Governor 3 coins instead of 2
    */
    bool tax() override;

    /**
    * This method aborts a tax action of a players
    * and decreases 2 of his coins
    * @param player the player whose tax is being reverted
    */
    void abortTax(Player &player) override;

    /**
    * @return Returns the name of the class
    */
    string getClassName() override;
};