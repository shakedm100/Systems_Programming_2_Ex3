//shaked1mi@gmail.com
#pragma once
#include "Player.hpp"

class Spy : public Player
{
    public:
    explicit Spy(const string& name) : Player(name) {};

    /**
    * This method returns the amount of coins a player have
    * after the spy used peek on him
    * @param player the target for peeking
    * @return the coins amount
    */
    int peek(Player& player) override;

    /**
    * This method blocks an arrest and takes away one coin from the player who initiated the arrest
    * @param player the player whose arrest is being blocked
    */
    void blockArrest(Player& player) override;

    /**
    * @return Returns the name of the class
    */
    string getClassName() override;
};
