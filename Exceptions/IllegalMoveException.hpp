//shaked1mi@gmail.com
#pragma once
#include <stdexcept>

class IllegalMoveException : public std::logic_error
{
public:
    explicit IllegalMoveException(std::string message);
};
