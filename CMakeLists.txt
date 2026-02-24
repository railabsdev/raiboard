# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(raiboard C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add displaylib
add_subdirectory(displaylib_16bit_PICO/library displaylib_16)

# Add LoRa library
add_subdirectory(lora)

# Add drivers
add_subdirectory(drivers)

# Add game framework
add_subdirectory(game)

# Add examples
# add_subdirectory(examples/asteroids)  # TODO: Update to use new framework
add_subdirectory(examples/game1)
