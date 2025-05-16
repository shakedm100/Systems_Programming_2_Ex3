#include "NoWinnerException.hpp"
#include <stdexcept>

NoWinnerException::NoWinnerException(std::string message) : std::logic_error(message) { };
