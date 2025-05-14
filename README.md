# Systems Programming 2 — Exercise 3

**Course**: Systems Programming 2

**Exercise**: 3

**Author**: Shaked Michael

## Description

This repository contains a C++ implementation of the **Coup** card game. It features:

* Core game logic (roles, actions, turn flow)
* Graphical user interface using SFML
* Custom exception handling
* Unit tests for validating game mechanics

## Directory Structure

```text
Systems_Programming_2_Ex3/
├── makefile              # Build configuration
├── main.cpp              # Program entry point
├── Exceptions/           # Custom exception classes
├── Logic/                # Game logic implementation
├── Roles/                # Role-specific classes (Governor, Spy, etc.)
├── GUI/                  # SFML-based graphical components
└── Testing/              # Unit tests
```

## Prerequisites

* Makefile
* C++17 compatible compiler (GCC, Clang, MSVC)
* SFML (>= 2.5)
* Doctest (for unit tests)

## Building

```bash

```

## Running

After building, run the executable (replace `<executable>` with the actual name):

```bash
./<executable>
```

## Testing

To execute unit tests:

```bash
make test
./test
```
