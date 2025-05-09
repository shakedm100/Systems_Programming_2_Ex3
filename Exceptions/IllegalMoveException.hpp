#pragma once
#include <stdexcept>

class IllegalMoveException : public std::logic_error
{
public:
    explicit IllegalMoveException(std::string message) : std::logic_error(message) { };
};
