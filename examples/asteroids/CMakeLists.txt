cmake_minimum_required(VERSION 3.12)

add_executable(asteroids
    main.cpp
    game.cpp
    audio_channel.cpp
    sprite.cpp
    collider.cpp
    screen.cpp
)

target_include_directories(asteroids PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)

target_link_libraries(asteroids PRIVATE
    pico_stdlib
    pico_game
    pico_drivers_cpp
    hardware_pwm
)

pico_enable_stdio_usb(asteroids 0)
pico_enable_stdio_uart(asteroids 0)

pico_add_extra_outputs(asteroids)
