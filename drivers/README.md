# Drivers Directory

This directory contains hardware drivers and their C++ wrappers for the RaiBoard project.

## Structure

```
drivers/
├── c_drivers/              # Low-level C drivers
│   ├── lora/              # LoRa radio driver (SX126x)
│   │   ├── lora_radio.h
│   │   └── lora_radio.c
│   └── display/           # ILI9341 TFT display driver
│       ├── ili9341_display.h
│       └── ili9341_display.c
├── cpp_wrappers/          # C++ object-oriented wrappers
│   ├── LoRa.hpp
│   ├── LoRa.cpp
│   ├── Display.hpp
│   └── Display.cpp
└── CMakeLists.txt         # Build configuration
```

## Driver Overview

### C Drivers - Low-Level Hardware Interfaces

The C drivers provide bare-bones, minimal interfaces to hardware. They're designed to be:
- **Lightweight**: Minimal overhead
- **Portable**: Easy to adapt to different platforms
- **Straightforward**: Direct hardware manipulation

#### lora_radio.h/c - LoRa SX126x Radio Driver

**Hardware**: SX126x LoRa radio module

**Features**:
- SPI communication interface
- Operating modes: IDLE, TX, RX, ERROR
- Frequency range: 150-960 MHz
- Spreading factors: SF5-SF12
- Bandwidth options: 7.8-500 MHz
- TX Power: Configurable up to 22 dBm
- DMA-based data transfer support

**Hardware Connections Required**:
```
SX126x Module → Pico
------------------
VCC → 3.3V
GND → GND
CLK → GPIO SCK (SPI)
MOSI → GPIO MOSI (SPI)
MISO → GPIO MISO (SPI)
CS → GPIO (configurable)
RST → GPIO (configurable)
INT → GPIO (configurable)
```

**Key Functions**:
- `bool lora_init_default()` - Initialize with defaults
- `bool lora_transmit(const uint8_t* data, size_t len)` - Send data
- `void lora_set_rx_mode()` - Enter receive mode
- `bool lora_get_rx_data(uint8_t* buffer, size_t* len, int16_t* rssi, int8_t* snr)` - Get received data
- `void lora_set_frequency(uint32_t freq_hz)` - Change frequency
- `void lora_set_spreading_factor(lora_spreading_factor_t sf)` - Adjust range/speed tradeoff

---

#### ili9341_display.h/c - ILI9341 TFT Display Driver

**Hardware**: ILI9341 240×320 pixel TFT display

**Features**:
- 240×320 resolution in 16-bit RGB565 color
- 4-orientation support (0°, 90°, 180°, 270°)
- Hardware-accelerated drawing primitives
- 153.6 KB frame buffer (optional)
- Low-power sleep mode

**Hardware Connections Required**:
```
ILI9341 Module → Pico
---------------------
VCC → 3.3V
GND → GND
CLK → GPIO SCK (SPI)
MOSI → GPIO MOSI (SPI)
MISO → GPIO MISO (SPI) [optional for reading]
CS → GPIO (configurable)
D/C → GPIO (configurable)
RST → GPIO (configurable)
BL → GPIO or 3.3V (for backlight)
```

**Key Functions**:
- `bool display_init_default()` - Initialize with defaults
- `void display_fill_screen(uint16_t color)` - Fill entire screen
- `void display_write_pixel(uint16_t x, uint16_t y, uint16_t color)` - Draw pixel
- `void display_write_string(uint16_t x, uint16_t y, const char* str, uint16_t fg, uint16_t bg)` - Draw text
- `void display_set_rotation(display_rotation_t rotation)` - Rotate display
- `void display_enter_sleep_mode()` - Power saving

---

### C++ Wrappers - Object-Oriented Interface

C++ wrappers provide safer, more convenient interfaces over the C drivers with:
- **RAII** - Automatic resource cleanup via destructors
- **Class Methods** - Type-safe, organized functions
- **Callbacks** - Function-based event handling for received data
- **Exception Safety** - Better error handling patterns

#### LoRa Class

```cpp
class LoRa {
    // See main README for full API documentation
};
```

**Benefits over C driver**:
- No need to manually manage state variables
- Automatic mode tracking
- Type-safe callback mechanism
- Object-oriented frequency/SF configuration

**Usage Pattern**:
```cpp
LoRa radio;  // Constructor initializes internal state
radio.init(915000000, LORA_SF_7);
radio.setRxCallback(my_callback);
radio.setRxMode();
// Callbacks invoke automatically when data arrives
// Destructor cleans up on scope exit
```

---

#### Display Class

```cpp
class Display {
    // See main README for full API documentation
};
```

**Benefits over C driver**:
- No need for manual buffer management
- Type-safe color values
- Method chaining friendly
- Automatic display deinitialization

**Usage Pattern**:
```cpp
Display screen;
screen.init(15, 16, 17, spi0, 25000000);
screen.fillScreen(COLOR_BLACK);
screen.drawString(10, 10, "Hello!", COLOR_WHITE, COLOR_BLACK);
// Destructor cleans up on scope exit
```

---

## CMake Build Configuration

The `CMakeLists.txt` defines how these drivers are compiled and linked:

### C Driver Libraries

```cmake
add_library(pico_drivers_c_lora INTERFACE)
```
- **Type**: INTERFACE library (header-only + object files)
- **Targets**: Projects that need LoRa
- **Usage**: `target_link_libraries(my_app pico_drivers_c_lora)`

```cmake
add_library(pico_drivers_c_display INTERFACE)
```
- **Type**: INTERFACE library
- **Targets**: Projects that need display
- **Usage**: `target_link_libraries(my_app pico_drivers_c_display)`

### C++ Wrapper Library

```cmake
add_library(pico_drivers_cpp STATIC ...)
```
- **Type**: Static library (compiled, not header-only)
- **Targets**: Projects needing C++ interfaces
- **Usage**: `target_link_libraries(my_app pico_drivers_cpp)`
- **Automatically links**: Both C drivers + dependencies

---

## Using These Drivers in Your Project

### Option 1: Use C++ Wrappers (Recommended)

In your `CMakeLists.txt`:
```cmake
add_executable(my_app main.cpp)

target_link_libraries(my_app
    pico_drivers_cpp    # Includes C drivers automatically!
    pico_stdlib
)

pico_enable_stdio_usb(my_app 1)
pico_add_extra_outputs(my_app)
```

In your `main.cpp`:
```cpp
#include "Display.hpp"
#include "LoRa.hpp"

int main() {
    Display screen;
    LoRa radio;
    // Use C++ class APIs
}
```

### Option 2: Use C Drivers Directly

In your `CMakeLists.txt`:
```cmake
add_executable(my_app main.c)

target_link_libraries(my_app
    pico_drivers_c_display
    pico_drivers_c_lora
    pico_stdlib
)

target_include_directories(my_app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../drivers/c_drivers/lora
    ${CMAKE_CURRENT_LIST_DIR}/../drivers/c_drivers/display
)

pico_add_extra_outputs(my_app)
```

In your `main.c`:
```c
#include "ili9341_display.h"
#include "lora_radio.h"

int main() {
    display_init_default();
    lora_init_default();
    // Use C function APIs
}
```

### Option 3: Mix Both (Advanced)

You can use C++ wrappers for convenience AND call C functions directly if needed:

```cpp
#include "Display.hpp"
#include "lora_radio.h"

// Use Display class
Display screen;

// Call C function directly
lora_transmit(data, len);
```

---

## Adding New Drivers

To add a new driver to this directory:

1. **Create driver subdirectory**:
   ```bash
   mkdir drivers/c_drivers/my_device
   ```

2. **Create C driver files**:
   - `my_device.h` - Interface/types
   - `my_device.c` - Implementation

3. **Update `drivers/CMakeLists.txt`**:
   ```cmake
   add_library(pico_drivers_c_my_device INTERFACE)
   target_sources(pico_drivers_c_my_device INTERFACE
       ${CMAKE_CURRENT_LIST_DIR}/c_drivers/my_device/my_device.c
   )
   target_include_directories(pico_drivers_c_my_device INTERFACE
       ${CMAKE_CURRENT_LIST_DIR}/c_drivers/my_device
   )
   target_link_libraries(pico_drivers_c_my_device INTERFACE
       pico_stdlib
       # Other dependencies
   )
   ```

4. **Create C++ wrapper** (optional):
   - `MyDevice.hpp` - Class declaration
   - `MyDevice.cpp` - Implementation
   - Update CMakeLists.txt to include new files in `pico_drivers_cpp`

5. **Document**:
   - Add section to `drivers/README.md`
   - Include usage examples
   - Document hardware connections

---

## Testing Drivers

To test drivers individually:

1. Create a test project: `mkdir test_lora`
2. Create `test_lora/CMakeLists.txt` linking just that driver
3. Create `test_lora/main.cpp` with test code
4. Build and verify behavior

---

## Version History

- **v1.0** (Initial Release)
  - LoRa C driver for SX126x
  - Display C driver for ILI9341
  - C++ wrappers for both drivers
  - Integrated with Pico SDK 2.2.0

---

## Maintenance Notes

- Drivers are focused on core functionality only
- Bug fixes maintain backward compatibility
- New features added via new functions, not API changes
- Documentation updated with every release

---

See the main [README.md](../README.md) for project overview and usage instructions.
