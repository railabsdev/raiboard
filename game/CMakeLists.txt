cmake_minimum_required(VERSION 3.12)

# Game library (compiled STATIC)
add_library(pico_game STATIC
    ${CMAKE_CURRENT_LIST_DIR}/Game.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GameObject.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Screen.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Sprite.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Joystick.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AudioChannel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xpt2046.c
)

target_include_directories(pico_game PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../displaylib_16bit_PICO/include
)

target_link_libraries(pico_game PUBLIC
    pico_stdlib
    hardware_spi
    hardware_adc
    hardware_pwm
    displaylib_16
)

set_target_properties(pico_game PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
