//shaked1mi@gmail.com
#pragma once
#include <stdexcept>

class NotImplementedException : public std::logic_error
{
public:
    explicit NotImplementedException(const std::string& message) : std::logic_error(message) { };
};