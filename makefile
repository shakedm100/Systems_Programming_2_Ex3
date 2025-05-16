#———————————————————————————————————————————————————————————————
# Compiler / Flags
CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -g \
              -include vector \
              -include limits \
              -include cmath \
              -include cstdint \
              $(shell pkg-config --cflags sfml-graphics sfml-window sfml-system)
LDLIBS    := $(shell pkg-config --libs    sfml-graphics sfml-window sfml-system)

#———————————————————————————————————————————————————————————————
# Source lists

# Core logic, roles & exceptions
CORE_SRCS := \
    Logic/Game.cpp \
    Roles/src/Player.cpp \
    Roles/src/Governor.cpp \
    Roles/src/Spy.cpp \
    Roles/src/Baron.cpp \
    Roles/src/General.cpp \
    Roles/src/Judge.cpp \
    Roles/src/Merchant.cpp \
    Exceptions/IllegalMoveException.cpp \
    Exceptions/NotImplementedException.cpp \
    Exceptions/NoWinnerException.cpp \
    GUI/TurnController.cpp \
    GUI/game_window.cpp


# GUI front-end
GUI_SRCS  := \
    GUI/main_window.cpp

# Console entry-point
MAIN_SRCS := main.cpp

# Test suite (Doctest only needs its own tests file)
TEST_SRCS := Testing/Tests.cpp

#———————————————————————————————————————————————————————————————
# Object files
CORE_OBJS := $(CORE_SRCS:.cpp=.o)
GUI_OBJS  := $(GUI_SRCS:.cpp=.o)
MAIN_OBJS := $(MAIN_SRCS:.cpp=.o)
TEST_OBJS := $(TEST_SRCS:.cpp=.o)

#———————————————————————————————————————————————————————————————
.PHONY: all clean game main test

all: game main test

# 1) SFML GUI game
game: $(CORE_OBJS) $(GUI_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o CoupGame $(LDLIBS)

# 2) Console “main”
main: $(CORE_OBJS) $(MAIN_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o CoupMain $(LDLIBS)

# 3) Doctest runner
test: $(CORE_OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o CoupTest $(LDLIBS)

# Run valgrind on the test executable
valgrind: test
	valgrind --leak-check=full ./CoupTest

#———————————————————————————————————————————————————————————————
# Generic compile rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

#———————————————————————————————————————————————————————————————
clean:
	rm -f $(CORE_OBJS) $(GUI_OBJS) $(MAIN_OBJS) $(TEST_OBJS) CoupGame CoupMain CoupTest
