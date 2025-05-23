//shaked1mi@gmail.com
#pragma once
#include <stdexcept>

/**
 * This exception is thrown if there is no current winner in the game and
 * the function winner() is called
 */
class NoWinnerException : public std::logic_error
{
public:
    explicit NoWinnerException(const std::string message);
};