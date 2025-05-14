//shaked1mi@gmail.com
#pragma once
#include "Player.hpp"

class General : public Player
{
public:
    explicit General(const string& name) : Player(name) {};

    /**
    * @return Returns the name of the class
    */
    string getClassName() override;

    /**
    * This method reverses the alive state of a given player
    * if the current General has more than 5 coins available
    * @param save the player whose being saved
    */
    void reverseCoup(Player &save) override;
};


