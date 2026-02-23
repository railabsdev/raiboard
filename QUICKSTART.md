# RaiBoard Quick Start Guide

Get up and running with a new RaiBoard project in 5 minutes.

---

## Prerequisites

Make sure you have:
- ✅ VS Code with Raspberry Pi Pico extension installed
- ✅ Pico SDK 2.2.0 installed
- ✅ Ninja build system
- ✅ ARM GCC toolchain 14.2
- ✅ A Raspberry Pi Pico board connected

---

## Creating Your First Project

### 1. Create Project Directory

```bash
cd /home/skgandre/raiboard
mkdir my_first_app
cd my_first_app
```

### 2. Create CMakeLists.txt

Copy the template and customize:

```bash
cp ../CMakeLists.template.txt CMakeLists.txt
```

Edit `CMakeLists.txt` and change:
```cmake
set(PROJECT_NAME my_first_app)
set(PROJECT_SOURCES
    main.cpp
)
```

### 3. Create main.cpp

```bash
cat > main.cpp << 'EOF'
#include "pico/stdlib.h"
#include "Display.hpp"
#include "LoRa.hpp"

int main() {
    stdio_init_all();
    
    printf("RaiBoard Starting...\n");
    
    // Initialize Display
    Display screen;
    if (!screen.init(15, 16, 17, 18, 19, -1, spi0, 25000000)) {
        printf("Display init failed!\n");
        return -1;
    }
    
    screen.fillScreen(COLOR_BLACK);
    sleep_ms(500);
    
    // Initialize LoRa
    LoRa radio;
    if (!radio.init(915000000, LORA_SF_7)) {
        printf("LoRa init failed!\n");
        return -1;
    }
    
    printf("All systems initialized!\n");
    
    // Main loop
    while (1) {
        if (screen.isInitialized()) {
            screen.drawString(50, 150, "Hello RaiBoard!", COLOR_CYAN, COLOR_BLACK);
        }
        
        sleep_ms(1000);
    }
    
    return 0;
}
EOF
```

### 4. Register Project in Main CMakeLists.txt

Edit `/CMakeLists.txt` and add your subdirectory:

**Before:**
```cmake
add_subdirectory(drivers)
```

**After:**
```cmake
add_subdirectory(drivers)
add_subdirectory(my_first_app)
```

### 5. Build

```bash
cd /home/skgandre/raiboard
rm -rf build  # Clean if rebuilding
mkdir -p build
cd build
cmake .. -GNinja
ninja
```

You should see output like:
```
[1/20] Building C object....
...
[20/20] Linking CXX executable my_first_app/my_first_app.elf

Built target my_first_app
```

### 6. Load to Pico

**Option A: USB Drag-and-Drop (Easiest)**
1. Hold BOOTSEL on Pico while plugging into USB
2. Pico appears as mass storage (`RPI-RP2`)
3. Drag `build/my_first_app/my_first_app.uf2` to it
4. Pico reboots and runs your code

**Option B: picotool**
```bash
picotool load build/my_first_app/my_first_app.uf2 -fx
```

**Option C: VS Code**
- Press `Ctrl+Shift+B` → Run Project
- (Requires picotool configured)

---

## Project Structure After First Build

```
my_first_app/
├── CMakeLists.txt        # Project build config
└── main.cpp              # Your source code

build/my_first_app/
├── my_first_app.elf      # Debug binary (with symbols)
├── my_first_app.uf2      # Upload to Pico (drag-and-drop)
├── my_first_app.bin      # Raw binary
├── my_first_app.dis      # Disassembly (for debugging)
└── CMakeFiles/           # Build artifacts
```

---

## Using Display

```cpp
#include "Display.hpp"

Display screen;

// Initialize
screen.init(15, 16, 17, 18, 19, -1, spi0, 25000000);

// Fill screen
screen.fillScreen(COLOR_BLACK);

// Draw shapes
screen.fillRectangle(10, 10, 50, 50, COLOR_BLUE);
screen.drawCircle(100, 100, 30, COLOR_RED);

// Draw text
screen.drawString(50, 200, "Hello", COLOR_WHITE, COLOR_BLACK);

// Rotate
screen.setRotation(DISPLAY_ROTATION_90);
```

**Color Constants**: `COLOR_BLACK`, `COLOR_WHITE`, `COLOR_RED`, `COLOR_GREEN`, `COLOR_BLUE`, `COLOR_CYAN`, `COLOR_MAGENTA`, `COLOR_YELLOW`, `COLOR_GRAY`, `COLOR_ORANGE`, `COLOR_NAVY`, `COLOR_PURPLE`

---

## Using LoRa

```cpp
#include "LoRa.hpp"

LoRa radio;

// Initialize with defaults (915 MHz, SF12)
radio.init();

// Or with custom settings
radio.init(868000000, LORA_SF_7);

// Receive with callback
radio.setRxCallback([](const uint8_t* data, size_t len, int16_t rssi, int8_t snr) {
    printf("Received %d bytes, RSSI:%d, SNR:%d\n", len, rssi, snr);
});

radio.setRxMode();  // Start receiving

// Transmit
uint8_t msg[] = {0x48, 0x69};  // "Hi"
radio.transmit(msg, 2);
```

**Frequencies**: 915 MHz (US), 868 MHz (EU), 433 MHz (Asia)  
**Spreading Factors**: `LORA_SF_5` to `LORA_SF_12`  
**Bandwidths**: `LORA_BW_125` to `LORA_BW_500`

---

## Debugging

### See Serial Output

In VS Code terminal:
```bash
# Find Pico port
ls /dev/ttyACM*

# Connect (usually /dev/ttyACM0)
screen /dev/ttyACM0 115200
# or
minicom -D /dev/ttyACM0 -b 115200
```

Then use `printf()` in your code:
```cpp
printf("Display initialized\n");
printf("RSSI: %d\n", rssi);
```

### Enable Debug Output in Project

Make sure your `CMakeLists.txt` has:
```cmake
pico_enable_stdio_usb(${PROJECT_NAME} 1)
```

Then enable USB in your main:
```cpp
stdio_init_all();  // Initialize USB and UART stdio
```

### View Build Commands

To see what commands ninja is running:
```bash
cd build
ninja -v
```

### Disassembly for Hardware Debugging

The build generates `.dis` files:
```bash
cat build/my_first_app/my_first_app.dis | less
```

---

## Troubleshooting

### "cmake: not found"
```bash
sudo apt-get install cmake
```

### "ninja: not found"
```bash
sudo apt-get install ninja-build
```

### Pico doesn't appear in USB/can't upload
1. Check BOOTSEL button works
2. Try Rescue Reset task (VS Code)
3. Use different USB port/cable
4. Check permissions: `sudo usermod -a -G dialout $USER`

### "Cannot find pico_stdlib"
- Pico SDK not installed or path wrong
- Check `pico_sdk_import.cmake` exists
- Verify `PICO_SDK_PATH` environment variable

### Build errors with Display/LoRa headers
- Verify drivers/CMakeLists.txt exists
- Check main CMakeLists.txt includes drivers via `add_subdirectory(drivers)`
- Rebuild from clean state: `rm -rf build && mkdir build && cd build && cmake .. -GNinja && ninja`

### "MEMORY FAULT" on device
- Likely stack overflow or null pointer
- Enable debugging and check UART output
- Verify GPIO pin assignments
- Check SPI clock speed (25 MHz is safe)

---

## Next Steps

- 📖 Read full [README.md](../README.md) for complete API documentation
- 🎨 Check [drivers/README.md](../drivers/README.md) for hardware details
- 🔧 Review [CMakeLists.template.txt](../CMakeLists.template.txt) for build options
- 📚 Study existing drivers in `drivers/c_drivers/`

---

## Common Patterns

### Project with Multiple Files

Structure:
```
my_app/
├── CMakeLists.txt
├── main.cpp
├── display_app.cpp
├── display_app.h
├── radio_app.cpp
└── radio_app.h
```

CMakeLists.txt:
```cmake
set(PROJECT_SOURCES
    main.cpp
    display_app.cpp
    radio_app.cpp
)
```

### Project Using Only Display

CMakeLists.txt:
```cmake
target_link_libraries(${PROJECT_NAME}
    pico_drivers_c_display  # Just display, not LoRa
    pico_stdlib
)
```

### Project Using External Libraries

CMakeLists.txt:
```cmake
target_link_libraries(${PROJECT_NAME}
    pico_drivers_cpp
    hardware_pwm      # Add hardware modules
    hardware_rtc
    pico_stdlib
)
```

---

**Stuck? Check the main [README.md](../README.md) for detailed documentation!**
