#pragma once
#include <string>

using namespace std;

struct Status
{
    bool isSanctioned;
    bool isArrested;
    bool canArrest;
    bool isAlive;

    Status() : isSanctioned(false), isArrested(false), canArrest(false), isAlive(false) {};
};

class Player
{
protected:
    string name;
    bool isTurn;
    int coins;
    Status status;

public:
    explicit Player(const string& name) : name(name), isTurn(false), coins(0), status() {};

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
};