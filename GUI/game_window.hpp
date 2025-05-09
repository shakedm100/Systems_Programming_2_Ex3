#pragma once
#include <SFML/Graphics.hpp>

// Launches the game window for `players` using `font`
// blocks until that window is closed.
void gameWindow(int players);
std::string promptPlayerName(int playerIndex);