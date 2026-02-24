# RAIBoard - Raspberry Pi Pico Game Development Framework

> **Note**: This project has been recently refactored and merged. See [MERGE_SUMMARY.md](MERGE_SUMMARY.md) for complete details.

A clean, modular game development framework for the Raspberry Pi Pico with ILI9341 display (240x320), XPT2046 touch screen, and SX126x LoRa radio support.

## 🎮 Features

- **Abstract Game Framework** - Object-oriented architecture with lifecycle hooks
- **GameObject System** - Automatic entity management and collision detection  
- **Hardware Abstraction** - Clean APIs for display, touch, and LoRa
- **Working Examples** - Complete game samples (game1, LoRa demos)
- **Modular Build System** - CMake + Ninja with proper dependency management

## 🚀 Quick Start

### Build & Flash

```bash
# Configure build
mkdir build && cd build
cmake -G Ninja ..

# Build example game
ninja game1

# Flash to Pico (hold BOOTSEL while connecting USB)
# Copy build/examples/game1/game1.uf2 to RPI-RP2 drive
```

## 📖 Key Documentation

- **[MERGE_SUMMARY.md](MERGE_SUMMARY.md)** ⭐ - Complete refactoring changelog
- **[GAME_FRAMEWORK_GUIDE.md](GAME_FRAMEWORK_GUIDE.md)** - Step-by-step game tutorials
- **[Wiki](https://github.com/railabsdev/raiboard/wiki)** - Full documentation

## 🎯 Creating Your First Game

```cpp
#include "Screen.hpp"
#include "Game.hpp"

class MyGame : public Game {
public:
    MyGame(Screen& screen) : Game(screen) {}
    
    void onInit() override { /* Initialize game */ }
    void onUpdate(float deltaTime) override { /* Update logic */ }
    void onRender() override { /* Draw game */ }
};

int main() {
    stdio_init_all();
    Screen screen;
    MyGame game(screen);
    game.run();
    return 0;
}
```

## 📁 Project Structure

```
raiboard/
├── game/                   # Abstract game framework ⭐
├── drivers/               # Hardware drivers (display, touch, LoRa)
├── examples/
│   ├── game1/            # Working game example ✅
│   └── lora/             # LoRa radio examples
├── displaylib_16bit_PICO/ # ILI9341 display library
└── lora/                  # SX126x LoRa library
```

## 🌟 Project Status

- ✅ Display library working
- ✅ Touch input working
- ✅ LoRa library functional
- ✅ Game framework complete
- ✅ Example game builds successfully
- 🔄 Hardware testing recommended

---

# RaiBoard

A modular Raspberry Pi Pico game development and LoRa communication platform with LoRa radio and ILI9341 display support.

---

## 📚 Documentation

**All documentation is hosted on the [GitHub Wiki](https://github.com/railabsdev/raiboard/wiki)**

### Quick Links

- 🚀 **[Quick Start](https://github.com/railabsdev/raiboard/wiki/QUICKSTART)** - Get up and running in 5 minutes
- 📖 **[Full Documentation](https://github.com/railabsdev/raiboard/wiki/README_FULL)** - Complete API reference
- 💾 **[Project Template](https://github.com/railabsdev/raiboard/wiki/CMakeLists-Template)** - Start a new project
- 📝 **[Code Examples](https://github.com/railabsdev/raiboard/wiki/EXAMPLES)** - Working project examples

### Specific Guides

- 🛠️ **[CMake Build Guide](https://github.com/railabsdev/raiboard/wiki/CMAKE-GUIDE)** - Build system reference
- 💻 **[VS Code Setup](https://github.com/railabsdev/raiboard/wiki/VSCODE-GUIDE)** - IDE configuration
- 🔌 **[Driver Documentation](https://github.com/railabsdev/raiboard/wiki/Drivers)** - Hardware drivers
- 📋 **[Doc Index](https://github.com/railabsdev/raiboard/wiki)** - Full wiki home

---

## Project Structure

```
raiboard/
├── README.md               # This file
├── CMakeLists.txt          # Main build configuration
├── pico_sdk_import.cmake   # Pico SDK import helper
├── .gitignore
├── .vscode/                # VS Code configuration
├── build/                  # Build output (generated)
├── drivers/                # Reusable drivers and wrappers
│   ├── CMakeLists.txt
│   ├── c_drivers/          # Low-level C drivers
│   │   ├── lora/           # LoRa radio (SX126x)
│   │   └── display/        # Display (ILI9341)
│   └── cpp_wrappers/       # C++ wrappers
└── [your-projects]/        # Your application projects
```

---

## Hardware

- **Microcontroller**: Raspberry Pi Pico (RP2040)
- **Pico SDK**: 2.2.0
- **Toolchain**: ARM GCC 14.2

### Included Drivers

- **LoRa Radio**: SX126x wireless module (915 MHz / 868 MHz / 433 MHz)
- **Display**: ILI9341 TFT (240×320, RGB565)

---

## Quick Start

```bash
# Clone the repo
git clone https://github.com/railabsdev/raiboard.git
cd raiboard

# Create your first project
mkdir my_project
cd my_project

# Copy CMakeLists template (from wiki)
# Create main.cpp (see examples in wiki)

# Build
cd ../build
cmake .. -GNinja
ninja

# Upload to Pico (drag .uf2 file with BOOTSEL held)
# Or use: picotool load build/my_project/my_project.uf2 -fx
```

**Full instructions**: [Quick Start Wiki](https://github.com/railabsdev/raiboard/wiki/QUICKSTART)

---

## Contributing

When adding documentation:
1. Edit files in the local wiki repo (cloned from `https://github.com/railabsdev/raiboard.wiki.git`)
2. Push to the wiki repository
3. Keep only source code and build files in this repo

---

## License

Project maintained by RaiLabs - February 23, 2026
