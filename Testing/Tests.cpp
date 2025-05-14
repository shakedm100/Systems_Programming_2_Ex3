//shaked1mi@gmail.com
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../Logic/Game.hpp"
#include "../Roles/headers/Player.hpp"
#include "../Exceptions/IllegalMoveException.hpp"
#include "../Exceptions/NoWinnerException.hpp"
#include "../Roles/headers/Governor.hpp"
#include "../Roles/headers/Baron.hpp"
#include "../Roles/headers/Spy.hpp"
#include "../Roles/headers/Merchant.hpp"

TEST_CASE("Player basic behavior via a concrete role (Governor)")
{
    Baron alice("Alice");
    CHECK(alice.getName() == "Alice");
    CHECK(alice.getCoins() == 0);

    alice.gather();
    CHECK(alice.getCoins() == 1);

    bool taxed = alice.tax();
    CHECK(taxed);
    CHECK(alice.getCoins() == 3);

    SUBCASE("Bribe insufficient funds")
    {
        alice.setCoins() = 3;
        CHECK(!alice.bribe());
        CHECK(alice.getExtraTurns() == 0);
    }
    SUBCASE("Bribe successful")
    {
        alice.setCoins() = 4;
        CHECK(alice.bribe());
        CHECK(alice.getCoins() == 0);
        CHECK(alice.getExtraTurns() == 2);

        alice.decreaseExtraTurns();
        CHECK(alice.getExtraTurns() == 1);
        alice.increaseExtraTurns();
        CHECK(alice.getExtraTurns() == 2);
    }

    SUBCASE("Status effects clear")
    {
        alice.getStatus().isSanctioned = true;
        alice.getStatus().holdTurn = true;
        CHECK(alice.getStatus().isSanctioned);
        CHECK(alice.getStatus().holdTurn);
        alice.clearStatusEffects();
        CHECK(!alice.getStatus().isSanctioned);
        CHECK(!alice.getStatus().holdTurn);
    }

    SUBCASE("Coup action on another Governor")
    {
        Governor bob("Bob");
        bob.setCoins() = 5; // irrelevant for coup
        alice.setCoins() = 7;
        // Coup should kill Bob
        CHECK(alice.coup(bob));
        CHECK(!bob.getStatus().isAlive);
    }
}

TEST_CASE("Game core functionality with explicit player list")
{
    // Build a deterministic two-player game
    std::vector<Player*> initPlayers;
    initPlayers.push_back(new Governor("Alice"));
    initPlayers.push_back(new Governor("Bob"));
    Game game(initPlayers);
    auto players = game.getPlayers();
    REQUIRE(players.size() == 2);

    CHECK(game.getActionNames().size() >= 1);
    CHECK_FALSE(game.actionNeedsTarget("Gather"));
    CHECK(game.actionNeedsTarget("Coup"));

    Player* actor = game.getCurrentTurn();
    REQUIRE(actor != nullptr);

    SUBCASE("Winner exception before a player won")
    {
        std::vector<Player*> freshList = { new Governor("X"), new Governor("Y") };
        Game fresh(freshList);
        CHECK_THROWS_AS(fresh.winner(), NoWinnerException);
    }

    SUBCASE("Turn progression and winning") {
        // Fund the actor for a coup
        actor->setCoins() = 7;
        Player* victim = players[1];
        CHECK_NOTHROW(game.perform(actor, "Coup", victim));
        CHECK(game.checkWinner());
        CHECK(game.getWinner() == actor);
        CHECK(game.winner() == actor->getName());
    }

    // Remark to self: no need to delete because game handles deletion of the players
}

TEST_CASE("Game illegal-move exception coverage for whyCannotPerform") {
    // Prepare players
    Governor* gov = new Governor("Gov");
    Baron* baron = new Baron("Bar");
    Spy* spy = new Spy("Spy");
    Merchant* merch = new Merchant("Mer");
    std::vector<Player*> plist = { gov, baron, spy, merch };
    Game g(plist);

    // 1) Must coup when coins >=10 and action != "Coup"
    gov->setCoins() = 10;
    CHECK_THROWS_AS(g.canPerform(gov, "Gather", nullptr), IllegalMoveException);

    // 2) Skip is always allowed
    gov->setCoins() = 0;
    CHECK_NOTHROW(g.canPerform(gov, "Skip", nullptr));

    // 3) Sanction prevents Gather and Tax
    gov->getStatus().isSanctioned = true;
    CHECK_THROWS_AS(g.canPerform(gov, "Gather", nullptr), IllegalMoveException);
    CHECK_THROWS_AS(g.canPerform(gov, "Tax", nullptr), IllegalMoveException);
    gov->getStatus().isSanctioned = false;

    // 4) Bribe failures and success
    gov->setCoins() = 3;
    CHECK_THROWS_AS(g.canPerform(gov, "Bribe", nullptr), IllegalMoveException);
    gov->setCoins() = 4;
    CHECK_NOTHROW(g.canPerform(gov, "Bribe", nullptr));

    // 5) Invest by non-Baron and Baron
    gov->setCoins() = 5;
    CHECK_THROWS_AS(g.canPerform(gov, "Invest", nullptr), IllegalMoveException);
    baron->setCoins() = 2;
    CHECK_THROWS_AS(g.canPerform(baron, "Invest", nullptr), IllegalMoveException);
    baron->setCoins() = 3;
    CHECK_NOTHROW(g.canPerform(baron, "Invest", nullptr));

    // 6) Peek by non-Spy and Spy
    gov->setCoins() = 0;
    CHECK_THROWS_AS(g.canPerform(gov, "Peek", nullptr), IllegalMoveException);
    CHECK_NOTHROW(g.canPerform(spy, "Peek", nullptr));

    // 7) Arrest on target with no coins and Merchant-specific
    gov->setCoins() = 5;
    Governor victim1("V1"); victim1.setCoins() = 0;
    CHECK_THROWS_AS(g.canPerform(gov, "Arrest", &victim1), IllegalMoveException);
    merch->setCoins() = 1;
    CHECK_THROWS_AS(g.canPerform(gov, "Arrest", merch), IllegalMoveException);

    // 8) Sanction target-specific failures
    gov->setCoins() = 2;
    CHECK_THROWS_AS(g.canPerform(gov, "Sanction", &victim1), IllegalMoveException);
    gov->setCoins() = 5;
    merch->getStatus().isSanctioned = true;
    CHECK_THROWS_AS(g.canPerform(gov, "Sanction", merch), IllegalMoveException);
    merch->getStatus().isSanctioned = false;

    // 9) Coup-specific failures
    gov->setCoins() = 6;
    CHECK_THROWS_AS(g.canPerform(gov, "Coup", &victim1), IllegalMoveException);
    gov->setCoins() = 7;
    victim1.getStatus().isAlive = false;
    CHECK_THROWS_AS(g.canPerform(gov, "Coup", &victim1), IllegalMoveException);

    // 10) Peek with target by non-Spy
    CHECK_THROWS_AS(g.canPerform(gov, "Peek", &victim1), IllegalMoveException);

    // 11) Unknown action
    gov->setCoins() = 0;
    CHECK_THROWS_AS(g.canPerform(gov, "Fly", nullptr), IllegalMoveException);
}
