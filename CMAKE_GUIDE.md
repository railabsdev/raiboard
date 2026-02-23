# RaiBoard CMake Build System Guide

Complete guide to understanding and working with CMake in the RaiBoard project.

---

## What is CMake?

CMake is a build configuration tool that:
1. Reads `CMakeLists.txt` files (recipes for building)
2. Generates build files for your build system (Ninja in our case)
3. Handles cross-compilation, dependencies, and platform differences

**Flow**: `CMakeLists.txt` → **CMake** → `build.ninja` → **Ninja** → Binaries

---

## Project Structure and CMake Hierarchy

```
/raiboard
├── CMakeLists.txt                # ROOT - Sets up Pico SDK, calls add_subdirectory()
├── pico_sdk_import.cmake         # Helper to find and import Pico SDK
├── CMakeLists.template.txt       # Template for new projects
│
├── /drivers
│   ├── CMakeLists.txt            # Defines driver libraries
│   ├── /c_drivers
│   │   ├── /lora
│   │   │   ├── lora_radio.h
│   │   │   └── lora_radio.c
│   │   └── /display
│   │       ├── ili9341_display.h
│   │       └── ili9341_display.c
│   └── /cpp_wrappers
│       ├── LoRa.hpp/cpp
│       └── Display.hpp/cpp
│
├── /my_project                   # Your application
│   ├── CMakeLists.txt            # Defines executable, links libraries
│   └── main.cpp
│
└── /build                        # Build output (do not commit!)
    ├── build.ninja               # Ninja build instructions
    ├── /drivers
    │   └── libpico_drivers_cpp.a # Compiled driver library
    └── /my_project
        └── my_project.elf        # Your compiled executable
```

---

## Understanding the Main CMakeLists.txt

Located: `/raiboard/CMakeLists.txt`

```cmake
# Minimum required version
cmake_minimum_required(VERSION 3.13)

# Set C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Enable compilation database (for IDE code completion)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === Pico SDK Setup ===
# This section is auto-generated and configures the Pico SDK path

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

# Pico SDK import
include(pico_sdk_import.cmake)

# === Project Definition ===
# Declares the project name and languages being used
project(raiboard C CXX ASM)

# Initialize Pico SDK with enabled features
pico_sdk_init()

# === Add Subdirectories ===
# Each add_subdirectory processes the CMakeLists.txt in that folder

add_subdirectory(drivers)      # Builds driver libraries
add_subdirectory(my_project)   # Builds your application
# add_subdirectory(my_other_app)  # Add more projects like this
```

### Key Concepts

| Concept | Meaning |
|---------|---------|
| `cmake_minimum_required()` | Minimum CMake version needed |
| `project()` | Declares project name & languages (C, CXX, ASM) |
| `set()` | Sets a variable (like `VAR = value` in bash) |
| `include()` | Includes another CMake file |
| `add_subdirectory()` | Processes CMakeLists.txt in subdirectory |

---

## Understanding drivers/CMakeLists.txt

Located: `/raiboard/drivers/CMakeLists.txt`

This file defines three libraries that can be used by applications:

### C Driver Libraries (INTERFACE)

```cmake
# Define C LoRa driver library
add_library(pico_drivers_c_lora INTERFACE)

# Where are the source files?
target_sources(pico_drivers_c_lora INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/c_drivers/lora/lora_radio.c
)

# Where are the headers for this library?
target_include_directories(pico_drivers_c_lora INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/c_drivers/lora
)

# What libraries does this depend on?
target_link_libraries(pico_drivers_c_lora INTERFACE
    pico_stdlib
    pico_lora_radio          # Hardware LoRa support from Pico SDK
)
```

**INTERFACE Library** = Library that's used only for headers and link dependencies (not compiled separately).

### C++ Wrapper Library (STATIC)

```cmake
# Define C++ wrapper library - actually gets compiled
add_library(pico_drivers_cpp STATIC
    ${CMAKE_CURRENT_LIST_DIR}/cpp_wrappers/LoRa.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cpp_wrappers/Display.cpp
)

# Header search paths when using this library
target_include_directories(pico_drivers_cpp PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/cpp_wrappers
    ${CMAKE_CURRENT_LIST_DIR}/c_drivers/lora
    ${CMAKE_CURRENT_LIST_DIR}/c_drivers/display
)

# Dependencies
target_link_libraries(pico_drivers_cpp PUBLIC
    pico_drivers_c_lora
    pico_drivers_c_display
    pico_stdlib
)

# Set C++ standard for this library specifically
set_target_properties(pico_drivers_cpp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
```

**STATIC Library** = Gets compiled to `.a` file and linked into your executable.

---

## Understanding Project CMakeLists.txt

Located: `/raiboard/my_project/CMakeLists.txt` (example)

```cmake
# Minimum required version
cmake_minimum_required(VERSION 3.12)

# Create an executable target
add_executable(my_app
    main.cpp
    src/helper.cpp
    src/config.cpp
)

# Link it with libraries
target_link_libraries(my_app
    pico_drivers_cpp      # Our driver library
    pico_stdlib           # Pico SDK standard library
    hardware_uart         # Optional: UART hardware support
)

# Where to find headers
target_include_directories(my_app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/../drivers/c_drivers
)

# Enable USB serial output (for printf)
pico_enable_stdio_usb(my_app 1)

# Enable UART serial output on GPIO 0/1
pico_enable_stdio_uart(my_app 0)

# Create .uf2, .bin, .hex, and .dis files
pico_add_extra_outputs(my_app)
```

### Key Commands

| Command | Meaning |
|---------|---------|
| `add_executable()` | Create executable/program to run |
| `add_library()` | Create library others can link to |
| `target_sources()` | Add source files to a target |
| `target_include_directories()` | Add header search paths |
| `target_link_libraries()` | Link dependencies |
| `target_compile_options()` | Add compiler flags |
| `set_target_properties()` | Set target properties |

---

## Build Variable Reference

### Variables You Can Use

```cmake
${CMAKE_CURRENT_LIST_DIR}          # Directory of current CMakeLists.txt
${CMAKE_CURRENT_SOURCE_DIR}        # Source directory being processed
${CMAKE_BINARY_DIR}                # Root build directory (/build)
${PROJECT_SOURCE_DIR}              # Root project directory
${PROJECT_BINARY_DIR}              # Root build directory (same as above)
${CMAKE_CURRENT_SOURCE_DIR}/..     # Parent directory
```

### Example Usage

```cmake
# Add all .c files in a directory
file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.c")

# Set a custom variable
set(MY_VERSION "1.0.0")

# Construct a path
set(INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/include")

# Use in target
target_include_directories(my_app PRIVATE ${INCLUDE_PATH})
```

---

## Common CMake Patterns

### Pattern 1: Simple Application

```cmake
cmake_minimum_required(VERSION 3.12)

add_executable(app main.cpp)
target_link_libraries(app pico_drivers_cpp pico_stdlib)
pico_enable_stdio_usb(app 1)
pico_add_extra_outputs(app)
```

### Pattern 2: Multi-File Application

```cmake
cmake_minimum_required(VERSION 3.12)

add_executable(app
    main.cpp
    module1.cpp
    module2.cpp
    utils/helper.cpp
)

target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/utils
)

target_link_libraries(app pico_drivers_cpp pico_stdlib)
pico_add_extra_outputs(app)
```

### Pattern 3: Using Specific Hardware Features

```cmake
cmake_minimum_required(VERSION 3.12)

add_executable(app main.cpp)

target_link_libraries(app
    pico_stdlib
    hardware_spi        # SPI communication
    hardware_i2c        # I2C communication
    hardware_gpio       # GPIO control
    hardware_pwm        # PWM output
    hardware_timer      # Timers
)

pico_add_extra_outputs(app)
```

### Pattern 4: Conditional Building

```cmake
# Only add file if condition is true
if(ENABLE_DEBUG)
    target_sources(app PRIVATE debug.c)
endif()

# Different behavior on different platforms
if(WIN32)
    target_compile_definitions(app PRIVATE -DWIN32)
else()
    target_compile_definitions(app PRIVATE -DUNIX)
endif()
```

---

## How to Add a New Project

### Step 1: Create Directory

```bash
mkdir /raiboard/my_new_project
```

### Step 2: Create CMakeLists.txt

Simplest version:
```cmake
cmake_minimum_required(VERSION 3.12)

add_executable(my_app main.cpp)
target_link_libraries(my_app pico_drivers_cpp pico_stdlib)
pico_enable_stdio_usb(my_app 1)
pico_add_extra_outputs(my_app)
```

Or copy the template:
```bash
cp /raiboard/CMakeLists.template.txt /raiboard/my_new_project/CMakeLists.txt
```

And edit the `set(PROJECT_NAME ...)` and `set(PROJECT_SOURCES ...)` variables.

### Step 3: Create Source Files

```cpp
// main.cpp
#include "pico/stdlib.h"
#include "Display.hpp"

int main() {
    stdio_init_all();
    printf("Hello from my_new_project!\n");
    
    Display screen;
    screen.init(...);
    
    return 0;
}
```

### Step 4: Register in Main CMakeLists.txt

Edit `/raiboard/CMakeLists.txt`:

```cmake
add_subdirectory(drivers)
add_subdirectory(my_new_project)  # Add this line
```

### Step 5: Rebuild

```bash
cd /raiboard/build
cmake ..
ninja
```

---

## Build Process Step-by-Step

### Step 1: CMake Configuration

```bash
cd /raiboard
mkdir build
cd build
cmake .. -GNinja
```

**What happens**:
- CMake reads all CMakeLists.txt files
- Checks dependencies and toolchain
- Generates `build.ninja`
- Creates build structure mirroring source

### Step 2: Ninja Build

```bash
ninja
```

**What happens**:
- Ninja reads `build.ninja` instructions
- Compiles C files → `.o` object files
- Compiles C++ files → `.o` object files
- Links object files → libraries (`.a`) and executables (`.elf`)
- Creates output files (`.uf2`, `.bin`, etc.)

### Step 3: Deploy to Pico

```bash
# Option A: Drag and drop
# Copy build/my_project/my_project.uf2 to Pico (after pressing BOOTSEL)

# Option B: picotool
picotool load build/my_project/my_project.uf2 -fx

# Option C: OpenOCD (with debugger)
openocd -s scripts -f interface/cmsis-dap.cfg -f target/rp2040.cfg \
  -c "program build/my_project/my_project.elf verify reset exit"
```

---

## Cleaning and Rebuilding

```bash
# Clean build artifacts
rm -rf build

# Reconfigure
mkdir build && cd build
cmake .. -GNinja

# Rebuild all
ninja

# Rebuild specific target
ninja my_project

# Verbose build (see actual commands)
ninja -v
```

---

## CMake Variables Reference

### Setting Variables

```cmake
# Simple variable
set(MY_VAR "value")

# List variable (semicolon-separated)
set(MY_LIST "item1;item2;item3")

# Path variable
set(MY_PATH "${CMAKE_CURRENT_LIST_DIR}/include")

# Cache variable (persists between runs)
set(MY_OPTION "ON" CACHE BOOL "Description")
```

### Using Variables

```cmake
# Direct substitution
target_include_directories(app PRIVATE ${MY_PATH})

# In strings
set(FULL_PATH "${MY_PATH}/file.h")

# In conditions
if(MY_OPTION)
    message("Option is enabled")
endif()

# In lists
target_link_libraries(app ${MY_LIST})
```

---

## Debugging CMake Issues

### See CMake Debug Output

```bash
cmake --debug-output .. -GNinja
```

### Print Variables

In CMakeLists.txt:
```cmake
message("My variable: ${MY_VAR}")
message("Project name: ${PROJECT_NAME}")
message("Current dir: ${CMAKE_CURRENT_LIST_DIR}")
```

### Check What Targets Are Defined

```bash
# After cmake configuration
ninja -t targets
```

### View Generated build.ninja

```bash
# See actual build commands
cat build/build.ninja | head -50
```

### Verbose Compilation

```bash
ninja -v
```

Shows actual gcc/g++ commands being executed.

---

## Advanced Topics

### Add a Global Include Directory

In main CMakeLists.txt after `project()`:
```cmake
include_directories(${CMAKE_CURRENT_LIST_DIR}/common/include)
```

All targets will then have access to headers in `common/include/`.

### Set Global Compiler Flags

```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")
```

### Link with External Libraries

```cmake
find_package(SomeLib REQUIRED)
target_link_libraries(my_app SomeLib::SomeLib)
```

### Conditional Headers

```cmake
if(ENABLE_DEBUG)
    target_compile_definitions(my_app PRIVATE DEBUG_ENABLED)
endif()
```

Then in code:
```cpp
#ifdef DEBUG_ENABLED
#define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) 
#endif
```

---

## Pico SDK-Specific Functions

These functions come from the Pico SDK:

```cmake
pico_sdk_init()                                    # Initialize Pico SDK

pico_enable_stdio_usb(target 1|0)                # Enable/disable USB serial
pico_enable_stdio_uart(target 1|0)               # Enable/disable UART serial

pico_add_extra_outputs(target)                   # Generate .uf2, .bin, .dis, .hex

pico_set_program_name(target "Name")             # Program name in binary
pico_set_program_version(target "1.0.0")         # Program version

pico_set_binary_type(target TYPE)                # Set binary type (DEFAULT, NO_FLASH, etc)
```

---

## Troubleshooting CMake

### Error: "Could not find Pico SDK"

**Cause**: PICO_SDK_PATH not set or pico_sdk_import.cmake broken

**Fix**:
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
cmake .. -GNinja
```

### Error: "Project not found in add_subdirectory"

**Cause**: Wrong path or CMakeLists.txt doesn't exist in subdir

**Fix**:
- Check path exists: `ls /raiboard/my_project/CMakeLists.txt`
- Check add_subdirectory path is relative to current CMakeLists.txt

### Error: "Target already exists"

**Cause**: Two CMakeLists.txt creating same target name

**Fix**:
- Make target names unique: `my_app1`, `my_app2`
- Or remove duplicate add_subdirectory()

### Nothing Rebuilds After CMakeLists.txt Change

**Cause**: CMake wasn't re-run

**Fix**:
```bash
cd build
cmake ..
ninja
```

---

## Summary

**Key Points**:
1. CMakeLists.txt is a recipe for building your project
2. CMake generates build.ninja from your recipes
3. Ninja actually compiles your code
4. Use `add_subdirectory()` to add projects
5. Use `target_link_libraries()` to connect dependencies
6. Use `target_include_directories()` for headers
7. Pico-specific functions handle SDK integration

**Workflow**:
```
Edit CMakeLists.txt → cmake .. → ninja → Upload to Pico
```

---

See [README.md](README.md) for overall project structure and [QUICKSTART.md](QUICKSTART.md) for getting started quickly.
