#pragma once
#include <stdexcept>

class NotImplementedException : public std::logic_error
{
public:
    explicit NotImplementedException(const char* message) : std::logic_error(message) { };
};