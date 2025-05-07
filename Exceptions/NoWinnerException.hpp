#pragma once
#include <stdexcept>

class NoWinnerException : public std::logic_error
{
public:
    NoWinnerException(const char* message) : std::logic_error(message) { };
};