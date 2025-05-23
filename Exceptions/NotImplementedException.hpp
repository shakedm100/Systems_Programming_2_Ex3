//shaked1mi@gmail.com
#pragma once
#include <stdexcept>

/**
 * This exception is used when using a function that the inheriting son isn't implementing
 * for example if baron calls abortTax(), but baron have no implementation for this function
 * then we throws this exception
 */
class NotImplementedException : public std::logic_error
{
public:
    explicit NotImplementedException(const std::string message);
};