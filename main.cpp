//shaked1mi@gmail.com
#include <iostream>
#include <vector>
#include <string>
#include "Roles/headers/Governor.hpp"
#include "Roles/headers/Spy.hpp"
#include "Roles/headers/Baron.hpp"
#include "Roles/headers/General.hpp"
#include "Roles/headers/Judge.hpp"
#include "Roles/headers/Merchant.hpp"
#include "Logic/Game.hpp"

int main() {
    std::vector<Player*> players = {
        new Governor("Alice"),
        new Spy     ("Bob"),
        new Baron   ("Carol"),
        new Judge   ("Diana"),
        new General ("Mallory"),
        new Merchant("Trent")
    };

    Game game(players);

    vector<string> playerNames = game.getPlayersNames();

    // Expected output:
    // Alice
    // Bob
    // Carol
    // Diana
    // Mallory
    // Trent
    for(string name : playerNames){
        cout << name << endl;
    }

    // Expected output: Alice
    game.turn();

    for(auto player : game.getAlivePlayers())
    {
        if(game.canPerform(player, "Gather", nullptr)) // true
        {
            game.perform(player, "Gather", nullptr);
            cout << *player << endl;
            game.nextTurn();
        }
    }

    // Expected exception - Not spy's turn
    try{
        game.canPerform(players[1], "Gather", nullptr);
    } catch (const std::exception &e){
        std::cerr << e.what() << '\n';
    }

    game.perform(players[0], "Gather", nullptr);
    game.nextTurn();
    game.perform(players[1], "Tax", nullptr);
    // Expected exception - Judge cannot undo tax
    try{
        players[3]->abortTax(*players[0]);
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n'; // For some reason it prints the error in the console in a weird way
    }

    cout << *players[0] << endl; // Expected: 2
    cout << *players[1] << endl; // Expected: 3

    string action = "Tax";
    game.setupPendingReverse(game.getCurrentTurn(), action, nullptr);
    string reverseAction = "Prevent Tax";
    game.performPendingReverse(reverseAction); // Governor undo tax
    cout << *players[1] << endl; // Expected: 1
    game.advancePendingResponder();
    game.clearPendingReverse();

    game.nextTurn();

    players[2]->tax();
    game.nextTurn();
    players[3]->gather();
    game.nextTurn();
    players[4]->gather();
    game.nextTurn();
    players[5]->gather();
    game.nextTurn();

    players[0]->tax(); // Governor gains 3 coins on tax
    game.nextTurn();
    players[1]->arrest(*players[0]);
    game.nextTurn();
    players[2]->invest(); // Baron traded its 3 coins and got 6
    game.nextTurn();
    players[3]->gather();
    game.nextTurn();
    players[4]->gather();
    game.nextTurn();
    players[5]->gather();
    game.nextTurn();

    players[0]->gather();
    game.nextTurn();
    players[1]->gather();
    game.nextTurn();
    players[2]->investSuccess();
    cout << *players[2] << endl; // Expected: 6
    players[2]->gather();
    game.nextTurn();
    players[3]->gather();
    game.nextTurn();
    players[4]->gather();
    game.nextTurn();
    players[5]->tax();
    game.nextTurn();

    players[0]->gather();
    game.nextTurn();
    players[1]->gather();
    game.nextTurn();
    cout << *players[2] << endl;
    if(game.canPerform(players[2], "Coup", players[0]))
    {
        game.perform(players[2], "Coup", players[0]);
        game.updateCurrentTurn();
    }
    else
        game.nextTurn();

    players[3]->gather();
    game.nextTurn();
    players[4]->gather();
    game.nextTurn();


    playerNames = game.getPlayersNames();
    // Since no one blocked the Baron, the expected output is:
    // Bob
    // Carol
    // Diana
    // Mallory
    // Trent
    for (string name : playerNames) {
        cout << name << endl;
    }

    if(game.canPerform(players[5], "Coup", players[1]))
    {
        game.perform(players[2], "Coup", players[1]);
        game.setupPendingReverse(players[2], "Coup", players[1]);
        game.updateCurrentTurn();
    }
    else
        game.nextTurn();

    reverseAction = "Reverse Coup";
    cout << "Reversing Coup";
    game.performPendingReverse(reverseAction); // Governor undo tax
    cout << "Post Reverse Coup Mechant:" << *players[5] << endl;
    cout << "Post Reverse Coup General:" << *players[4] << endl;
    cout << "Post Reverse Coup Spy:" << *players[2] << endl;
    game.advancePendingResponder();
    game.clearPendingReverse();

    playerNames = game.getPlayersNames();
    // Since General blocked the Merchant, the expected output is:
    // Bob
    // Carol
    // Diana
    // Mallory
    // Trent
    for (string name : playerNames) {
        cout << name << endl;
    }
    return 0;
}
