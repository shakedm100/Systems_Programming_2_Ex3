//shaked1mi@gmail.com
#include "IllegalMoveException.hpp"
#include <stdexcept>

IllegalMoveException::IllegalMoveException(std::string message) : std::logic_error(message) { };
