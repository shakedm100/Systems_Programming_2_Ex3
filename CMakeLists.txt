cmake_minimum_required(VERSION 3.15)
project(Coup LANGUAGES CXX)

# 1) C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2) Where SFML lives
set(SFML_ROOT   "C:/SFML")
set(SFML_INCLUDE_DIR "${SFML_ROOT}/include")
set(SFML_LIBRARY_DIR "${SFML_ROOT}/lib")

# 3) Tell the compiler/linker about SFML
include_directories(
        ${SFML_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/headers
)
link_directories(
        ${SFML_LIBRARY_DIR}
)

# 4) Your source files
set(SRC_FILES
        Logic/Game.cpp
        Roles/src/Governor.cpp
        Roles/src/Spy.cpp
        Roles/src/General.cpp
        Roles/src/Judge.cpp
        Roles/src/Merchant.cpp
        Roles/src/Baron.cpp
        Roles/src/Player.cpp
        GUI/main_window.cpp
        GUI/game_window.cpp
        GUI/game_window.hpp
        Logic/GameSetup.cpp
        Logic/GameSetup.hpp
        GUI/TurnController.cpp
        GUI/TurnController.hpp
        Exceptions/IllegalMoveException.hpp
        Roles/headers/Player.hpp
)

# 5) Build the executable
add_executable(${PROJECT_NAME}
        ${SRC_FILES}
)

# 6) Link against SFML’s .a/.dll.a libraries by name
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
)

# 7) Post‐build: copy the DLLs next to your .exe so you can just Run in CLion
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SFML_ROOT}/bin/sfml-graphics-2.dll"
        "${SFML_ROOT}/bin/sfml-window-2.dll"
        "${SFML_ROOT}/bin/sfml-system-2.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/GUI/Assets"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets
)