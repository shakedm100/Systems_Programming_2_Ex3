#pragma once
#include <string>

using namespace std;

struct Status
{
    bool isSanctioned;
    bool isArrested;
    bool canArrest;
    bool isAlive;
    bool canTax;
    bool canRevert; // Name is not intuitive.

    Status() : isSanctioned(false), isArrested(false), canArrest(true), isAlive(true),
    canTax(true) {};
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

    void gather();
    virtual bool tax();
    bool bribe();
    bool arrest(Player& role);
    bool sanction(Player& role);
    bool coup(Player& player) const;
    int getCoins() const;
    bool getCanArrest() const;
    bool& setCanArrest();
    string getName() const;
    virtual void abortTax(Player& role); //Governor only action
    virtual int peek(Player& role); //Spy only action
    virtual void blockArrest(Player& role); //Spy only action
    virtual void invest(); //Baron only action
    virtual void preventCoup(); //General only action
    virtual void preventBribe(); //Judge only action
    friend ostream& operator<<(ostream& os, const Player& player);
    virtual std::string getClassName() = 0;
    Status getStatus() const;
    Status& setStatus();
    int getExtraTurns() const;
    void decreaseExtraTurns();
    void increaseExtraTurns();
    void clearStatusEffects();
};