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

// Helper to look up a live player by name
Player* findPlayerByName(Game& game, const std::string& name) {
    for (Player* p : game.getAlivePlayers()) {
        if (p->getName() == name) return p;
    }
    return nullptr;
}

int main() {
    // 1) Create four players with fixed roles & names
    std::vector<Player*> players = {
        new Governor("Alice"),
        new Spy     ("Bob"),
        new Baron   ("Carol"),
        new Judge   ("Diana")
    };

    // 2) Initialize game with our vector of pointers
    Game game(players);
    // — if you don’t yet have this ctor, add to Game.hpp:
    //   explicit Game(const std::vector<Player*>& v)
    //     : players(v), alivePlayers(v),
    //       current_turn(v.front()), game_winner(nullptr),
    //       hasPending(false), currentIndex(0), indexBeforeReaction(0)
    //   {}

    // 3) Define a fixed “script” of turns
    struct Turn { std::string actor, action, target; };
    std::vector<Turn> script = {
        // actor   action      target (empty = no target)
        {"Alice",  "Gather",    ""       },
        {"Bob",    "Tax",       ""       },
        {"Carol",  "Bribe",     "Bob"    },
        {"Diana",  "Prevent Bribe", "Carol"},
        {"Alice",  "Skip",      ""       },
        {"Bob",    "Gather",    ""       },
        {"Carol",  "Coup",      "Bob"    }
        // …add as many as you like
    };

    // 4) Play out the script
    for (auto& t : script)
    {
        Player* actor  = findPlayerByName(game, t.actor);
        Player* target = t.target.empty() ? nullptr : findPlayerByName(game, t.target);

        if (!actor) {
            std::cerr << "Error: actor '" << t.actor << "' not found\n";
            continue;
        }
        try {
            game.perform(actor, t.action, target);
        }
        catch (const std::exception& e) {
            std::cout << "Illegal move: " << e.what() << "\n";
        }
        if (game.checkWinner()) break;
    }

    // 5) Announce the winner
    try {
        std::cout << "Winner: " << game.winner() << "\n";
    }
    catch (...) {
        std::cout << "No winner at end of script.\n";
    }

    // 6) Clean up
    // (Game::~Game will delete the pointers if you wired it that way)
    return 0;
}
