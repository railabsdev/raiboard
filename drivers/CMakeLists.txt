cmake_minimum_required(VERSION 3.12)

# C drivers library
add_library(pico_drivers_c_lora INTERFACE)
target_sources(pico_drivers_c_lora INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/c_drivers/lora/lora_radio.c
)
target_include_directories(pico_drivers_c_lora INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/c_drivers/lora
)
target_link_libraries(pico_drivers_c_lora INTERFACE
    pico_stdlib
    pico_lora_radio
)

add_library(pico_drivers_c_display INTERFACE)
target_sources(pico_drivers_c_display INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/c_drivers/display/ili9341_display.c
)
target_include_directories(pico_drivers_c_display INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/c_drivers/display
)
target_link_libraries(pico_drivers_c_display INTERFACE
    pico_stdlib
    displaylib_16
    hardware_spi
)

# C++ wrappers library
add_library(pico_drivers_cpp STATIC
    ${CMAKE_CURRENT_LIST_DIR}/cpp_wrappers/LoRa.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cpp_wrappers/Display.cpp
)
target_include_directories(pico_drivers_cpp PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/cpp_wrappers
    ${CMAKE_CURRENT_LIST_DIR}/c_drivers/lora
    ${CMAKE_CURRENT_LIST_DIR}/c_drivers/display
)
target_link_libraries(pico_drivers_cpp PUBLIC
    pico_drivers_c_lora
    pico_drivers_c_display
    pico_stdlib
)

set_target_properties(pico_drivers_cpp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
