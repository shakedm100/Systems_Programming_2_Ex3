//shaked1mi@gmail.com
#pragma once
#include <stdexcept>

/**
 * This class is used for throwing an exception if the player
 * does an Illegal move
 */
class IllegalMoveException : public std::logic_error
{
public:
    explicit IllegalMoveException(std::string message);
};
