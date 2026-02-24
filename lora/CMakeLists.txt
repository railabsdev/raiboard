cmake_minimum_required(VERSION 3.12)

# initialize pico_sdk from GIT
# (note this can come from environment, CMake cache etc)
# set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(pico_lorwan)

# initialize the Pico SDK
pico_sdk_init()

set(LORAMAC_NODE_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/LoRaMac-node)

add_library(pico_lora_radio INTERFACE)

target_sources(pico_lora_radio INTERFACE
    ${LORAMAC_NODE_PATH}/src/boards/mcu/utilities.c

    ${LORAMAC_NODE_PATH}/src/radio/sx126x/sx126x.c
    ${LORAMAC_NODE_PATH}/src/radio/sx126x/radio.c

    ${LORAMAC_NODE_PATH}/src/system/delay.c
    ${LORAMAC_NODE_PATH}/src/system/gpio.c
    ${LORAMAC_NODE_PATH}/src/system/nvmm.c
    ${LORAMAC_NODE_PATH}/src/system/systime.c
    ${LORAMAC_NODE_PATH}/src/system/timer.c

    ${CMAKE_CURRENT_LIST_DIR}/src/boards/rp2040/board.c
    ${CMAKE_CURRENT_LIST_DIR}/src/boards/rp2040/delay-board.c
    ${CMAKE_CURRENT_LIST_DIR}/src/boards/rp2040/eeprom-board.c
    ${CMAKE_CURRENT_LIST_DIR}/src/boards/rp2040/gpio-board.c
    ${CMAKE_CURRENT_LIST_DIR}/src/boards/rp2040/rtc-board.c
    ${CMAKE_CURRENT_LIST_DIR}/src/boards/rp2040/spi-board.c
    ${CMAKE_CURRENT_LIST_DIR}/src/boards/rp2040/sx126x-board.c

    ${CMAKE_CURRENT_LIST_DIR}/src/radio_stream.cpp
)

target_include_directories(pico_lora_radio INTERFACE
    ${LORAMAC_NODE_PATH}/src
    ${LORAMAC_NODE_PATH}/src/boards
    ${LORAMAC_NODE_PATH}/src/radio
    ${LORAMAC_NODE_PATH}/src/system
    ${CMAKE_CURRENT_LIST_DIR}/src/include
)

target_link_libraries(pico_lora_radio INTERFACE pico_stdlib pico_unique_id hardware_spi hardware_timer)

# Examples moved to examples/lora directory
