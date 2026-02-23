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
