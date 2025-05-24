//shaked1mi@gmail.com
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../Logic/Game.hpp"
#include "../Roles/headers/Player.hpp"
#include "../Exceptions/IllegalMoveException.hpp"
#include "../Exceptions/NoWinnerException.hpp"
#include "../Roles/headers/Governor.hpp"
#include "../Roles/headers/Baron.hpp"
#include "../Roles/headers/General.hpp"
#include "../Roles/headers/Judge.hpp"
#include "../Roles/headers/Spy.hpp"
#include "../Roles/headers/Merchant.hpp"
#include <algorithm>

/**
 * This Test is responsible for testing the basics of player functionality
 */
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

/**
 * This test case tests the basic functionality of the logic layer, Game class
 */
TEST_CASE("Game core functionality with explicit player list")
{
    // Build a two-player game
    std::vector<Player*> initPlayers;
    initPlayers.push_back(new Governor("Alice"));
    initPlayers.push_back(new Governor("Bob"));
    Game game(initPlayers);
    auto players = game.getPlayers();
    REQUIRE(players.size() == 2); //Make sure there are 2 players in the game

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

    SUBCASE("Perform Actions")
    {
        int size = 6;
        Governor* gov = new Governor("Gov");
        Baron* baron = new Baron("Bar");
        Spy* spy = new Spy("Spy");
        Merchant* merch = new Merchant("Mer");
        Judge* judge = new Judge("Judge");
        General* general = new General("Gen");
        std::vector<Player*> playersList = { gov, baron, spy, merch, judge, general };
        Game subGame(playersList);
        // All players gather
        for(int i = 0; i < size; i++)
        {
            subGame.perform(subGame.getCurrentTurn(), "Gather", nullptr);
            subGame.nextTurn();
        }
        // Check gather worked on all players
        for(int i = 0; i < size; i++)
        {
            CHECK(subGame.getCurrentTurn()->getCoins() == 1);
            subGame.nextTurn();
        }
        // All players tax
        for(int i = 0; i < size; i++)
        {
            subGame.perform(subGame.getCurrentTurn(), "Tax", nullptr);
            subGame.nextTurn();
        }
        // Check tax worked on all players
        for(int i = 0; i < size; i++)
        {
            // Governor gains 3 coins from tax and Merchant get a bonus coin for being above 3 coins
            if(subGame.getCurrentTurn()->getClassName() == "Governor" || subGame.getCurrentTurn()->getClassName() == "Merchant")
            {
                CHECK(subGame.getCurrentTurn()->getCoins() == 4);
                subGame.nextTurn();
            }
            else
            {
                CHECK(subGame.getCurrentTurn()->getCoins() == 3);
                subGame.nextTurn();
            }
        }

        // Go to Baron
        subGame.nextTurn();
        subGame.perform(subGame.getCurrentTurn(), "Invest", nullptr);
        subGame.getCurrentTurn()->investSuccess();
        CHECK(subGame.getCurrentTurn()->getCoins() == 6); // Check successful investment

        subGame.perform(subGame.getCurrentTurn(), "Invest", nullptr);
        subGame.getCurrentTurn()->investFailure(); // Check failed investment
        CHECK(subGame.getCurrentTurn()->getCoins() == 4);

        subGame.nextTurn(); // Go to spy
        // Get the coin from the baron
        subGame.perform(subGame.getCurrentTurn(), "Arrest", baron);
        CHECK(subGame.getCurrentTurn()->getCoins() == 4);
        CHECK(baron->getCoins() == 3);

        // Perform sanction tests
        subGame.nextTurn(); // Go to Merchant
        subGame.perform(subGame.getCurrentTurn(), "Sanction", spy);
        CHECK(subGame.getCurrentTurn()->getCoins() == 2);
        CHECK(spy->getCoins() == 4);
        subGame.perform(spy, "Gather", nullptr); // Can't gather
        subGame.perform(spy, "Tax", nullptr); // Can't tax
        CHECK(spy->getCoins() == 4); // Same amount
        subGame.getCurrentTurn()->setCoins() = 3;
        subGame.perform(subGame.getCurrentTurn(), "Sanction", judge);
        CHECK(subGame.getCurrentTurn()->getCoins() == 3);
        CHECK(!judge->getStatus().isSanctioned);
        subGame.getCurrentTurn()->setCoins() = 5;
        subGame.perform(subGame.getCurrentTurn(), "Sanction", judge);
        CHECK(subGame.getCurrentTurn()->getCoins() == 1);
        CHECK(judge->getStatus().isSanctioned);

        // Bribe functionality in perform
        subGame.getCurrentTurn()->setCoins() = 4;
        subGame.perform(subGame.getCurrentTurn(), "Bribe", nullptr);
        CHECK(subGame.getCurrentTurn()->getExtraTurns() == 2);
        subGame.getCurrentTurn()->decreaseExtraTurns();
        CHECK(subGame.getCurrentTurn()->getExtraTurns() == 1);

        // Coup is tested in the next Subcase along with winning
    }

    SUBCASE("Turn progression and winning")
    {
        // Fund the actor for a coup
        CHECK(game.getAlivePlayers().size() == 2);
        actor->setCoins() = 7;
        Player* victim = players[1];
        CHECK_NOTHROW(game.perform(actor, "Coup", victim));
        CHECK(game.getAlivePlayers().size() == 1);
        CHECK(game.checkWinner());
        CHECK(game.getWinner() == actor);
        CHECK(game.winner() == actor->getName());
    }

    // Remark to self: no need to delete because game handles deletion of the players
}

TEST_CASE("Game illegal-move exception coverage for whyCannotPerform")
{
    // Prepare players
    Governor* gov = new Governor("Gov");
    Baron* baron = new Baron("Bar");
    Spy* spy = new Spy("Spy");
    Merchant* merch = new Merchant("Mer");
    std::vector<Player*> plist = { gov, baron, spy, merch };
    Game game(plist);

    // Must coup when coins >=10 and action != "Coup"
    gov->setCoins() = 10;
    CHECK_THROWS_AS(game.canPerform(gov, "Gather", nullptr), IllegalMoveException);

    // Skip is always allowed
    gov->setCoins() = 0;
    CHECK_NOTHROW(game.canPerform(gov, "Skip", nullptr));

    // Sanction prevents Gather and Tax
    gov->getStatus().isSanctioned = true;
    CHECK_THROWS_AS(game.canPerform(gov, "Gather", nullptr), IllegalMoveException);
    CHECK_THROWS_AS(game.canPerform(gov, "Tax", nullptr), IllegalMoveException);
    gov->getStatus().isSanctioned = false;

    // Bribe failures and success
    gov->setCoins() = 3;
    CHECK_THROWS_AS(game.canPerform(gov, "Bribe", nullptr), IllegalMoveException);
    gov->setCoins() = 4;
    CHECK_NOTHROW(game.canPerform(gov, "Bribe", nullptr));

    // Invest by non-Baron and Baron
    gov->setCoins() = 5;
    CHECK_THROWS_AS(game.canPerform(gov, "Invest", nullptr), IllegalMoveException);
    baron->setCoins() = 2;
    game.nextTurn();
    CHECK_THROWS_AS(game.canPerform(baron, "Invest", nullptr), IllegalMoveException);
    baron->setCoins() = 3;
    CHECK_NOTHROW(game.canPerform(baron, "Invest", nullptr));

    // Peek by non-Spy and Spy
    gov->setCoins() = 0;
    CHECK_THROWS_AS(game.canPerform(gov, "Peek", nullptr), IllegalMoveException);
    game.nextTurn();
    CHECK_NOTHROW(game.canPerform(spy, "Peek", nullptr));

    // Arrest on target with no coins and Merchant-specific
    gov->setCoins() = 5;
    Governor victim1("V1"); victim1.setCoins() = 0;
    game.nextTurn();
    game.nextTurn();
    CHECK_THROWS_AS(game.canPerform(gov, "Arrest", &victim1), IllegalMoveException);
    merch->setCoins() = 1;
    CHECK_THROWS_AS(game.canPerform(gov, "Arrest", merch), IllegalMoveException);

    // Sanction target-specific failures
    gov->setCoins() = 2;
    CHECK_THROWS_AS(game.canPerform(gov, "Sanction", &victim1), IllegalMoveException);
    gov->setCoins() = 5;
    merch->getStatus().isSanctioned = true;
    CHECK_THROWS_AS(game.canPerform(gov, "Sanction", merch), IllegalMoveException);
    merch->getStatus().isSanctioned = false;

    // Coup-specific failures
    gov->setCoins() = 6;
    CHECK_THROWS_AS(game.canPerform(gov, "Coup", &victim1), IllegalMoveException);
    gov->setCoins() = 7;
    victim1.getStatus().isAlive = false;
    CHECK_THROWS_AS(game.canPerform(gov, "Coup", &victim1), IllegalMoveException);

    // Peek with target by non-Spy
    CHECK_THROWS_AS(game.canPerform(gov, "Peek", &victim1), IllegalMoveException);

    // Unknown action
    gov->setCoins() = 0;
    CHECK_THROWS_AS(game.canPerform(gov, "Fly", nullptr), IllegalMoveException);
}

/**
 * This test case checks that reaction time logic
 * is working correctly
 */
TEST_CASE("Game Reaction Actions logic")
{
    SUBCASE("No matching responders still no pending") {
        Spy* actor = new Spy("Spy");
        Judge* judge = new Judge("Judge");
        std::vector<Player*> players = { actor, judge };
        Game game(players);

        game.setupPendingReverse(actor, "Tax", nullptr);
        CHECK_FALSE(game.hasPendingReverse());
    }

    SUBCASE("Single responder is found for Tax")
    {
        Spy* actor= new Spy("Spy");
        Governor* gov = new Governor("Governor");
        std::vector<Player*> players = { actor, gov };
        Game game(players);

        game.setupPendingReverse(actor, "Tax", nullptr);
        CHECK(game.hasPendingReverse());
        CHECK(game.getCurrentTurn() == gov);
        CHECK_FALSE(game.isPendingActionBribe());
    }

    SUBCASE("advancePendingResponder cycles through multiple responders") {
        Spy* actor = new Spy("Spy");
        Governor* gov1 = new Governor("Governor");
        Governor* gov2 = new Governor("Governor");
        std::vector<Player*> players = { actor, gov1, gov2 };
        Game game(players);

        game.setupPendingReverse(actor, "Tax", nullptr);
        // first responder must be gov1
        REQUIRE( game.getCurrentTurn() == gov1);

        // advance to second
        CHECK(game.advancePendingResponder());
        CHECK(game.getCurrentTurn() == gov2);

        // no more, so advancePendingResponder() false, turn stays on last
        CHECK_FALSE(game.advancePendingResponder());
        CHECK(game.getCurrentTurn() == gov2);
    }

    SUBCASE("clearPendingReverse restores original turn and clears state") {
        Spy* actor = new Spy("Spy");
        Governor* gov = new Governor("Governor");
        std::vector<Player*> players = { actor, gov };
        Game game(players);

        // initial turn
        REQUIRE(game.getCurrentTurn() == actor);

        game.setupPendingReverse(actor, "Tax", nullptr);
        REQUIRE(game.hasPendingReverse());
        REQUIRE(game.getCurrentTurn() == gov);

        game.clearPendingReverse();
        CHECK_FALSE(game.hasPendingReverse());
        CHECK(game.getCurrentTurn() == actor);
    }

    SUBCASE("performPendingReverse Prevent Tax triggers abortTax") {
        Spy* actor = new Spy("Spy");
        Governor* gov = new Governor("Governor");
        std::vector<Player*> players = { actor, gov };
        Game game(players);

        game.perform(actor, "Tax", nullptr);
        CHECK(actor->getCoins() == 2);
        game.setupPendingReverse(actor, "Tax", nullptr);
        std::string action = "Prevent Tax";
        game.performPendingReverse(action);

        CHECK(actor->getCoins() == 0);
    }

    SUBCASE("performPendingReverse Prevent Arrest blocks and refunds one coin") {
        Governor* actor = new Governor("Governor");
        Spy* spy = new Spy("Spy");
        Baron* target = new Baron("Baron");
        target->setCoins() = 3; // Allow arrest

        std::vector<Player*> players = { actor, spy };
        Game game(players);

        game.perform(actor, "Arrest", target);
        CHECK(target->getCoins() == 2);
        CHECK(actor->getCoins() == 1);
        game.setupPendingReverse(actor, "Arrest", target);
        std::string action = "Prevent Arrest";
        game.performPendingReverse(action);

        // Spy should have blocked the arrest
        CHECK(target->getCoins() == 3);
        CHECK(actor->getCoins() == 0);

        delete target;
    }

    SUBCASE("performPendingReverse Reverse Coup restores target and calls reverseCoup") {
        Spy* actor = new Spy("Spy");
        General* general = new General("General");
        general->setCoins() = 5; // Allow save
        actor->setCoins() = 7; // Allow coup

        // Start with only the actor alive
        std::vector<Player*> players = { actor, general };
        Game game(players);

        game.perform(actor, "Coup", general);
        CHECK(!general->getStatus().isAlive);
        game.setupPendingReverse(actor, "Coup", general);
        std::string action = "Reverse Coup";
        game.performPendingReverse(action);

        CHECK(general->getStatus().isAlive);

        // and the general should now be back among the alive players
        auto alive = game.getAlivePlayers();
        auto foundGeneral = std::find(alive.begin(), alive.end(), general);
        CHECK(foundGeneral != alive.end());
    }

    SUBCASE("isPendingActionBribe only true for Bribe") {
        Spy* actor = new Spy("Spy");
        Judge* judge = new Judge("Judge");
        std::vector<Player*> players = { actor, judge };
        Game game(players);
        actor->setCoins() = 4;

        game.perform(actor, "Bribe", nullptr);
        CHECK(actor->getExtraTurns() == 2);
        game.setupPendingReverse(actor, "Bribe", nullptr);
        CHECK(game.isPendingActionBribe());

        game.clearPendingReverse();
        game.setupPendingReverse(actor, "Tax", nullptr);
        CHECK_FALSE( game.isPendingActionBribe() );
    }

    SUBCASE("Reverse bribe using Judge") {
        Spy* actor = new Spy("Spy");
        Judge* judge = new Judge("Judge");
        std::vector<Player*> players = { actor, judge };
        Game game(players);
        actor->setCoins() = 4;

        game.perform(actor, "Bribe", nullptr);
        CHECK(actor->getExtraTurns() == 2);
        game.setupPendingReverse(actor, "Bribe", nullptr);
        std::string action = "Reverse Bribe";
        game.performPendingReverse(action);
        CHECK(actor->getExtraTurns() == 0);
    }
}