//shaked1mi@gmail.com
#include "NotImplementedException.hpp"
#include <stdexcept>

NotImplementedException::NotImplementedException(std::string message) : std::logic_error(message) { };
