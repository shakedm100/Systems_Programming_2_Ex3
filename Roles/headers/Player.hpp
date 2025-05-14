#pragma once
#include <string>

using namespace std;

struct Status
{
    bool isSanctioned;
    bool isAlive;
    bool isInvested;
    bool holdTurn; // Helper bool

    Status() : isSanctioned(false), isAlive(true),
    isInvested(false), holdTurn(false){};
};

class Player
{
protected:
    string name;
    int coins;
    Status status;
    int extraTurns;

public:
    explicit Player(const string& name) : name(name), coins(0), status(), extraTurns(0) {};

    virtual ~Player() = default;

    /**
     * If the player isn't sanctioned the method gives him 1 coin
     */
    void gather();

    /**
     * If the player isn't sanctioned the method gives him 2 coins
     * (ignoring other assumptions)
     */
    virtual bool tax();

    /**
     * If the player has more than 4 coins give him two extra turns
     * because in the current turn logic Bribe is technically a turn
     * but in the game's logic it isn't
     * @return true if the bribe succeeded, false otherwise
     */
    bool bribe();

    /**
     * If a player has more than 0 coins then take one of his coins to yourself
     * also if the player is General then do nothing and if the player is a
     * Merchant then take two but don't take anything for yourself
     * @param player the player being arrested
     * @return true if succeeded, false otherwise
     */
    bool arrest(Player& player);

    /**
     * This method makes a target player sanctioned if the current player
     * has more than 3 coins or 4 if the target is a Judge
     * @param player the player being targeted for sanction
     * @return true if succeeded, false otherwise
     */
    bool sanction(Player& player);

    /**
     * This method coups a target if the current player has more than 7 coins
     * setting his status 'dead'
     * @param player the target player for coup
     * @return true if succeeded, false otherwise
     */
    bool coup(Player& player) const;

    /**
     * 
     * @return the coins amount of the current player
     */
    int getCoins() const;

    /**
     * Returns the coins amount of the current player
     * allowing for modification of the field
     * @return the coins amount
     */
    int& setCoins();

    /**
     * 
     * @return the name of the current player
     */
    string getName() const;

   /**
    * This method aborts a tax action of a players
    * and decreases 2 of his coins
    * @param player the player whose tax is being reverted
    */
    virtual void abortTax(Player& player); //Governor only action

     /**
     * This method returns the amount of coins a player have
     * after the spy used peek on him
     * @param player the target for peeking
     * @return the coins amount
     */
    virtual int peek(Player& player); //Spy only action

    /**
    * This method blocks an arrest and takes away one coin from the player who initiated the arrest
    * @param player the player whose arrest is being blocked
    */
    virtual void blockArrest(Player& player); //Spy only action

    /**
    * This method activates the Baron's invest status
    */
    virtual void invest(); //Baron only action

    /**
    * Activates successful investment and gives 6 coins
    */
    virtual void investSuccess(); //Baron only action

    /**
    * Activates failure investment and gives 1 coin
    */
    virtual void investFailure(); //Baron only action

     /**
     * This method reverses the alive state of a given player
     * if the current General has more than 5 coins available
     * @param save the player who's being saved
     */
    virtual void reverseCoup(Player &save); //General only action

     /**
     * This method reverts a bribe done by another player
     * and takes away all of his extra turns
     * @param prevent the player whose bribe is being reverted
     */
    virtual void preventBribe(Player &prevent); //Judge only action

    /**
    * This method gives one extra coins if a player has
    * more than 3 coins at the start of a turn
    */
    virtual void aboveThreeCoins(); //Merchant only action

    /**
     * This method overrides the operator << for Player
     * and allows to print player using cout easily
     * @param os the normal ostream
     * @param player the player to print his name
     * @return ostream to print
     */
    friend ostream& operator<<(ostream& os, const Player& player);

    /**
    * Pure virtual function in Player, implemented by it's
    * inheriting classes
    * @return Returns the name of the class
    */
    virtual std::string getClassName() = 0;

    /**
     * 
     * @return the status of the current player
     */
    Status getStatus() const;

    /**
     * Returns the current status of the player by reference
     * to allow changing his current status
     * @return the status of the current player
     */
    Status& getStatus();

    /**
     * 
     * @return how many extra turns the player has
     */
    int getExtraTurns() const;

    /**
     * Decreases by one the field extra turns
     * used when a player used an extra turn after bribing
     */
    void decreaseExtraTurns();

   /**
    * Increases by one the field extra turns
    * used when a player after a player bribes (usually twice)
    */
    void increaseExtraTurns();

    /**
     * Clears all the players status effects and
     * returns him to normal state
     */
    void clearStatusEffects();
};