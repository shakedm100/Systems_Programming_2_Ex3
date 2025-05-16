# Coup Game (Systems Programming 2 — Exercise 3)

**Author:** Shaked Michael

A C++ implementation of the Coup card game, featuring:

* Core game logic (roles, actions, turn flow)
* Graphical user interface using SFML
* Custom exception handling
* Unit tests with Doctest

## Directory Structure

```text
Systems_Programming_2_Ex3/
├── makefile                # Build configuration
├── main.cpp                # Console entry point
├── Exceptions/             # Custom exceptions implementaions
├── Logic/                  # Core game logic
│   ├── Game.cpp
├── Roles/                  # Role-specific implementations and headers
│   ├── src/                # .cpp implementations
│   └── headers/            # .hpp declarations
├── GUI/                    # SFML-based graphical components
│   ├── game_window.cpp
│   ├── TurnController.cpp
│   ├── main_window.cpp
│   └── Assets/             # Fonts and other assets (arial.ttf)
├── Testing/                # Doctest unit tests (Tests.cpp)
│   ├── Tests.cpp
```

## Dependencies

1. **Compiler & Build Tools**

    * g++ with C++17 support
    * Make
    * `pkg-config`

2. **SFML**

    * On Debian/Ubuntu/WSL:

      ```bash
      sudo apt update
      sudo apt install libsfml-dev pkg-config
      ```

3. **OpenGL / Mesa** (for WSL graphics context)

    * WSL2 + WSLg (Windows 10/11)
    * On WSL:

      ```bash
      sudo apt install libgl1-mesa-dri mesa-utils
      ```

4. **Doctest** (header-only testing framework)

    * The `Testing/Tests.cpp` includes the `doctest.h` file in the repository.

## Makefile Usage

The provided `makefile` builds three executables:

* **CoupGame** – the SFML GUI game
* **CoupMain** – the console application (main.cpp)
* **CoupTest** – the Doctest test runner

### Common Targets

| Target       | Description                                        |
| ------------ | -------------------------------------------------- |
| `make all`   | Build **CoupGame**, **CoupMain**, and **CoupTest** |
| `make game`  | Build only the SFML GUI game (**CoupGame**)        |
| `make main`  | Build only the console app (**CoupMain**)          |
| `make test`  | Build only the test runner (**CoupTest**)          |
| `make clean` | Remove all object files and executables            |

### Building Everything

```bash
make clean
make all
```

### Running

* **GUI game:**

  ```bash
  ./CoupGame
  ```

  Ensure you run from the project root so it can find `GUI/Assets/arial.ttf`.

* **Console main:**

  ```bash
  ./CoupMain
  ```

* **Tests:**

  ```bash
  ./CoupTest
  ```

## Notes

* **Font path:** The GUI looks for `GUI/Assets/arial.ttf` relative to the executable. Keep `Assets/arial.ttf` in `GUI/Assets/`.