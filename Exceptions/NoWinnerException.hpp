#pragma once
#include <stdexcept>

class NoWinnerException : public std::logic_error
{
public:
    explicit NoWinnerException(const char* message) : std::logic_error(message) { };
};