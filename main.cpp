//shaked1mi@gmail.com
#include <iostream>
#include <vector>
#include <string>

#include "Exceptions/IllegalMoveException.hpp"
#include "Roles/headers/Governor.hpp"
#include "Roles/headers/Spy.hpp"
#include "Roles/headers/Baron.hpp"
#include "Roles/headers/General.hpp"
#include "Roles/headers/Judge.hpp"
#include "Roles/headers/Merchant.hpp"
#include "Logic/Game.hpp"

int main() {
    using namespace std;

    // ----------------------
    // Initialize Players
    // ----------------------
    vector<Player*> players = {
        new Governor("Alice"),
        new Spy     ("Bob"),
        new Baron   ("Carol"),
        new Judge   ("Diana"),
        new General ("Mallory"),
        new Merchant("Trent")
    };

    Game game(players);

    // -----------------------------
    // List starting player names
    // -----------------------------
    cout << "Initial players:" << endl;
    for (auto& name : game.getPlayersNames()) {
        cout << " - " << name << endl;
    }
    cout << endl;

    // -------------------------------------
    // Show available actions and targets
    // -------------------------------------
    cout << "Available actions:" << endl;
    for (auto& action : game.getActionNames()) {
        cout << " * " << action;
        if (game.actionNeedsTarget(action)) {
            cout << " [requires target]";
        }
        cout << endl;
    }
    cout << endl;

    // -----------------------------------------------
    // Demonstrate normal turns: Gather and Tax
    // -----------------------------------------------
    cout << "-- Gathering and Taxing demo --" << endl;
    game.turn();
    for (auto p : game.getAlivePlayers()) {
        auto name = p->getName();
        if (game.canPerform(p, "Gather", nullptr)) {
            game.perform(p, "Gather", nullptr);
            cout << name << " gathered -> coins=" << p->getCoins() << endl;
        }
        game.nextTurn();
    }
    cout << endl;

    // -------------------------------------------------
    // Spy peek demonstration (revealing coin state)
    // -------------------------------------------------
    cout << "-- Spy Peek demo --" << endl;
    // Advance to Bob (Spy)
    while (game.getCurrentTurn()->getName() != "Bob") {
        game.nextTurn();
    }
    try {
        if (game.canPerform(game.getCurrentTurn(), "Peek", nullptr)) {
            int hidden = game.getCurrentTurn()->peek(*game.getCurrentTurn());
            cout << "Bob peeked: alive players' coins: ";
            cout << hidden;
        }
    } catch (const exception& ex) {
        cerr << "Peek error: " << ex.what() << endl;
    }
    game.nextTurn();
    cout << endl;

    // --------------------------------
    // Merchant bribe + extra turns
    // --------------------------------
    cout << "-- Merchant Bribe demo --" << endl;
    // Advance to Trent (Merchant)
    while (game.getCurrentTurn()->getName() != "Trent") {
        game.nextTurn();
    }
    Player* merchant = game.getCurrentTurn();
    merchant->setCoins() = 4;
    cout << merchant->getName() << " coins before bribe: " << merchant->getCoins() << endl;
    if (game.canPerform(merchant, "Bribe", nullptr)) {
        game.perform(merchant, "Bribe", nullptr);
        cout << "Bribe successful! " << merchant->getName()
             << " now has coins=" << merchant->getCoins()
             << ", extra turns=" << merchant->getExtraTurns() << endl;
    }
    cout << merchant->getName() << " uses extra turn: ";
    if (game.canPerform(merchant, "Gather", nullptr)) {
        game.perform(merchant, "Gather", nullptr);
        cout << "gathered -> coins=" << merchant->getCoins() << endl;
    }
    // Reset:
    while (merchant->getExtraTurns() > 0) merchant->decreaseExtraTurns();
    game.nextTurn();
    cout << endl;

    // ----------------------------------------
    // Baron invest demonstration (doubles)
    // ----------------------------------------
    cout << "-- Baron Invest demo --" << endl;
    // Advance to Carol (Baron)
    while (game.getCurrentTurn()->getName() != "Carol") {
        game.nextTurn();
    }
    Player* baron = game.getCurrentTurn();
    baron->setCoins() = 3;
    cout << "Before invest: " << baron->getCoins() << endl;
    if (game.canPerform(baron, "Invest", nullptr)) {
        game.perform(baron, "Invest", nullptr);
        cout << "After invest: " << baron->getCoins() << endl;
    }
    game.nextTurn();
    cout << endl;

    if(game.canPerform(game.getCurrentTurn(), "Gather", nullptr))
        game.perform(game.getCurrentTurn(), "Gather", nullptr); //Judge gather
    game.nextTurn();
    if(game.canPerform(game.getCurrentTurn(), "Gather", nullptr))
        game.perform(game.getCurrentTurn(), "Gather", nullptr); // General gather
    game.nextTurn();

    // ------------------------------------------
    // Governor abortTax / reaction phase example
    // ------------------------------------------
    cout << "-- Governor AbortTax + PendingReverse demo --" << endl;

    if(game.canPerform(game.getCurrentTurn(), "Tax", nullptr))
    {
        game.perform(game.getCurrentTurn(), "Tax", nullptr); // Merchant tax
        cout << game.getCurrentTurn()->getName() << " coins before abortTax: " << game.getCurrentTurn()->getCoins() << endl;
        game.setupPendingReverse(game.getCurrentTurn(), "Tax", nullptr);
        string reverseAction = "Prevent Tax";
        game.performPendingReverse(reverseAction);
        game.clearPendingReverse(); //Clear response queue and return to Mechant
        cout << game.getCurrentTurn()->getName() << " coins after abortTax: " << game.getCurrentTurn()->getCoins() << endl;
    }
    game.nextTurn();

    // ----------------------------------------
    // Illegal Move Exception demonstration
    // ----------------------------------------
    cout << "-- IllegalMoveException demo --" << endl;
    Player* first = game.getPlayers().front();
    try {
        // Forcing a coup illegally
        first->setCoins() = 0;
        game.canPerform(first, "Coup", players[1]);
    } catch (const exception& ex) {
        cout << "Caught IllegalMoveException: " << ex.what() << endl;
    }
    cout << endl;

    // ------------------------------------
    // Everyone Coups Until One Wins
    // ------------------------------------
    cout << "-- Everyone Coups demo --" << endl;
    // give each enough coins to coup
    for (auto p : game.getAlivePlayers()) p->setCoins() = 7;

    while (game.getAlivePlayers().size() > 1) {
        Player* actor = game.getCurrentTurn();
        // choose first other alive player as target
        for (auto target : game.getAlivePlayers()) {
            if (target != actor) {
                if (game.canPerform(actor, "Coup", target)) {
                    game.perform(actor, "Coup", target);
                    cout << actor->getName() << " couped " << target->getName() << endl;
                }
                break;
            }
        }
        game.nextTurn();
    }

    if (game.checkWinner()) {
        cout << "Winner: " << game.getWinner()->getName() << endl;
    }

    return 0;
}
