cmake_minimum_required(VERSION 3.12)

# Game library (compiled STATIC)
add_library(pico_game STATIC
    ${CMAKE_CURRENT_LIST_DIR}/Game.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GameObject.cpp
)

target_include_directories(pico_game PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(pico_game PUBLIC
    pico_stdlib
    pico_drivers_cpp
)

set_target_properties(pico_game PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(pico_game PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
