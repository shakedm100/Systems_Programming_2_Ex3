#include "GameSetup.hpp"
#include "Roles/headers/Governor.hpp"
#include "Roles/headers/Spy.hpp"
#include "Roles/headers/Baron.hpp"
#include "Roles/headers/General.hpp"
#include "Roles/headers/Judge.hpp"
#include "Roles/headers/Merchant.hpp"
#include <bits/random.h>
#include <random>     // for mt19937, uniform_int_distribution
#include <stdexcept>
#include "GUI/game_window.hpp"

Game createGame(const int playersCount)
{
    vector<Player*> players;
    players.reserve(playersCount);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dice(1,6);

    for(int i = 0; i < playersCount; i++)
    {
        std::string name = promptPlayerName(i+1);
        if (name.empty())
        {
            throw std::invalid_argument("Player naming cancelled!\n");
        }

        int result = dice(mt);
        switch (result)
        {
            case 1:
            {
                players.push_back(new Governor(name));
                break;
            }
            case 2:
            {
                players.push_back(new Spy(name));
                break;
            }
            case 3:
            {
                players.push_back(new Baron(name));
                break;
            }
            case 4:
            {
                players.push_back(new General(name));
                break;
            }
            case 5:
            {
                players.push_back(new Judge(name));
                break;
            }
            case 6:
            {
                players.push_back(new Merchant(name));
                break;
            }
        }
    }

    return Game(players);
}