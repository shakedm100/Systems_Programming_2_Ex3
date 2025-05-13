#pragma once
#include <SFML/Graphics.hpp>

/**
 * Launches the main game window
 * Creates game and TurnController that handles the logic and
 * the connection between the logic and the UI
 * @param players how many players there are in the current game
 */
void gameWindow(int players);

/**
 * This method shows a prompt window that asks for the player's name
 * @param playerIndex the player's index you want to know his name
 * @return the player's input
 */
std::string promptPlayerName(int playerIndex);