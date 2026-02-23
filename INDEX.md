# RaiBoard Documentation Index

Welcome to RaiBoard! This document helps you navigate all available documentation.

---

## 🚀 Getting Started (New Users)

Start here if you're new to the project:

1. **[QUICKSTART.md](QUICKSTART.md)** (5-minute guide)
   - Create your first project
   - Build and upload to Pico
   - Use Display and LoRa drivers
   - Common patterns and troubleshooting

2. **[README.md](README.md)** (comprehensive overview)
   - Full project documentation
   - Driver API reference (C and C++)
   - CMake build system basics
   - Creating new projects step-by-step

---

## 📚 Complete Guides

Detailed documentation for specific topics:

### Building and Compilation

- **[CMAKE_GUIDE.md](CMAKE_GUIDE.md)** - Complete CMake reference
  - How CMake works
  - CMakeLists.txt explained
  - Adding new projects
  - Build patterns and troubleshooting
  - Advanced CMake topics

- **[CMakeLists.template.txt](CMakeLists.template.txt)** - Reusable project template
  - Copy this file and customize for new projects
  - All configuration options documented
  - No need to write CMake from scratch

### IDE and Tools

- **[VSCODE_GUIDE.md](VSCODE_GUIDE.md)** - VS Code integration guide
  - Extensions needed
  - Configuration files
  - Build tasks explained
  - Debugging setup
  - Serial monitor usage
  - Keyboard shortcuts

### Hardware and Drivers

- **[drivers/README.md](drivers/README.md)** - Drivers documentation
  - LoRa SX126x radio driver
  - ILI9341 display driver
  - C code examples
  - C++ wrapper API
  - Hardware connections
  - Adding new drivers

---

## 📁 Project Structure

```
raiboard/
├── 📄 README.md                    # Main documentation (START HERE!)
├── 📄 QUICKSTART.md                # 5-minute quickstart guide
├── 📄 VSCODE_GUIDE.md              # VS Code setup and usage
├── 📄 CMAKE_GUIDE.md               # CMake build system guide
├── 📄 CMakeLists.template.txt      # Template for new projects
├── 📄 INDEX.md                     # This file
│
├── CMakeLists.txt                  # Main build file (Pico SDK setup)
├── pico_sdk_import.cmake           # Pico SDK helper
│
├── drivers/                        # Reusable drivers
│   ├── README.md                   # Drivers documentation
│   ├── CMakeLists.txt              # Driver build config
│   ├── c_drivers/                  # Low-level C drivers
│   │   ├── lora/                   # LoRa radio driver
│   │   │   ├── lora_radio.h
│   │   │   └── lora_radio.c
│   │   └── display/                # Display driver
│   │       ├── ili9341_display.h
│   │       └── ili9341_display.c
│   └── cpp_wrappers/               # C++ wrappers
│       ├── LoRa.hpp/cpp            # LoRa class
│       └── Display.hpp/cpp         # Display class
│
├── [your-project]/                 # Your application (create these)
│   ├── CMakeLists.txt              # Project build config
│   └── main.cpp                    # Your source code
│
└── build/                          # Build output (do not commit!)
    ├── build.ninja                 # Build instructions
    └── [projects]/                 # Compiled executables
```

---

## 🎯 Common Tasks

### I want to...

**...understand the project structure**
→ Read [README.md #Directory Structure](README.md#directory-structure)

**...create a new project**
→ Follow [QUICKSTART.md](QUICKSTART.md) or [README.md #Creating New Projects](README.md#creating-new-projects)

**...learn how to use the Display driver**
→ See [README.md #Display Class](README.md#display-class) + [drivers/README.md](drivers/README.md)

**...learn how to use the LoRa driver**
→ See [README.md #LoRa Class](README.md#lora-class) + [drivers/README.md](drivers/README.md)

**...understand CMake and the build system**
→ Read [CMAKE_GUIDE.md](CMAKE_GUIDE.md)

**...set up VS Code properly**
→ Follow [VSCODE_GUIDE.md](VSCODE_GUIDE.md)

**...compile and upload my project**
→ See [README.md #Building and Deployment](README.md#building-and-deployment)

**...debug a project**
→ Check [VSCODE_GUIDE.md #Debugging](VSCODE_GUIDE.md#debugging)

**...add a new hardware driver**
→ See [drivers/README.md #Adding New Drivers](drivers/README.md#adding-new-drivers)

**...understand C vs C++ APIs**
→ Read [drivers/README.md #C Drivers and C++ Wrappers](drivers/README.md#c-drivers---low-level-hardware-interfaces)

**...troubleshoot a build error**
→ Check [CMAKE_GUIDE.md #Troubleshooting CMake](CMAKE_GUIDE.md#troubleshooting-cmake) or [README.md #Troubleshooting](README.md#troubleshooting)

---

## 📖 Documentation by Audience

### For Complete Beginners

1. [QUICKSTART.md](QUICKSTART.md) - Get something working quickly
2. [README.md](README.md) - Understand what you've built
3. [drivers/README.md](drivers/README.md) - Learn the hardware drivers
4. [VSCODE_GUIDE.md](VSCODE_GUIDE.md) - Master your development environment

### For Experienced C/C++ Developers

1. [README.md #API Reference](README.md#drivers-documentation) - Quick API lookup
2. [CMAKE_GUIDE.md](CMAKE_GUIDE.md) - Understand the build system
3. [drivers/README.md](drivers/README.md) - Hardware interface details
4. [CMakeLists.template.txt](CMakeLists.template.txt) - Project template

### For Embedded Systems Engineers

1. [drivers/README.md](drivers/README.md) - Driver architecture and customization
2. [README.md #Pico SDK Configuration](README.md#pico-sdk-configuration) - Hardware configuration
3. [CMAKE_GUIDE.md #Advanced Topics](CMAKE_GUIDE.md#advanced-topics) - Complex build scenarios
4. [VSCODE_GUIDE.md #Debugging](VSCODE_GUIDE.md#debugging) - Hardware debugging setup

### For DevOps/CI-CD Automation

1. [CMAKE_GUIDE.md](CMAKE_GUIDE.md) - Build automation
2. [README.md #Manual Build Steps](README.md#manual-build-steps) - Command-line building
3. [CMakeLists.template.txt](CMakeLists.template.txt) - Standard project structure

---

## 🔍 API Quick Reference

### Display API

| Function | Purpose |
|----------|---------|
| `screen.init(...)` | Initialize display on SPI |
| `screen.fillScreen(color)` | Fill entire screen |
| `screen.drawPixel(x, y, color)` | Draw single pixel |
| `screen.drawString(x, y, text, fg, bg)` | Draw text |
| `screen.setRotation(rot)` | Rotate display |

See: [README.md #Display Class](README.md#display-class)

### LoRa API

| Function | Purpose |
|----------|---------|
| `radio.init()` | Initialize with defaults |
| `radio.init(freq, sf)` | Initialize with custom settings |
| `radio.transmit(data, len)` | Send data |
| `radio.setRxMode()` | Enter receive mode |
| `radio.setRxCallback(func)` | Set data received callback |

See: [README.md #LoRa Class](README.md#lora-class)

### CMake Commands

| Command | Purpose |
|---------|---------|
| `cmake .. -GNinja` | Generate build files |
| `ninja` | Compile project |
| `add_executable()` | Create program |
| `target_link_libraries()` | Add dependencies |
| `target_include_directories()` | Add header paths |

See: [CMAKE_GUIDE.md](CMAKE_GUIDE.md)

---

## 🛠️ Troubleshooting Guide

### Build Issues

- **"cmake: not found"** → Install CMake
- **"ninja: not found"** → Install Ninja
- **"Could not find Pico SDK"** → Set PICO_SDK_PATH
- **"Unknown target"** → Check CMakeLists.txt

See: [CMAKE_GUIDE.md #Troubleshooting CMake](CMAKE_GUIDE.md#troubleshooting-cmake)

### Upload Issues

- **"Pico not detected"** → Check USB connection
- **"picotool: command not found"** → Install picotool
- **Device doesn't run code** → Check build succeeded

See: [README.md #Troubleshooting](README.md#troubleshooting)

### Development Environment

- **IntelliSense not working** → See [VSCODE_GUIDE.md](VSCODE_GUIDE.md#intellisense-not-working)
- **Serial output not showing** → See [VSCODE_GUIDE.md](VSCODE_GUIDE.md#serial-monitor-not-opening)
- **Debugger won't connect** → See [VSCODE_GUIDE.md](VSCODE_GUIDE.md#cant-connect-to-debugger)

---

## 📋 File Summary

| File | Purpose | Audience |
|------|---------|----------|
| [README.md](README.md) | Complete project documentation | Everyone |
| [QUICKSTART.md](QUICKSTART.md) | 5-minute start guide | Beginners |
| [CMAKE_GUIDE.md](CMAKE_GUIDE.md) | Build system deep-dive | Build engineers |
| [VSCODE_GUIDE.md](VSCODE_GUIDE.md) | IDE setup and usage | VS Code users |
| [drivers/README.md](drivers/README.md) | Hardware drivers | Driver developers |
| [CMakeLists.template.txt](CMakeLists.template.txt) | Project template | Project creators |
| [INDEX.md](INDEX.md) | Documentation index | Navigation |

---

## 🔗 External Resources

### Official Documentation

- [Raspberry Pi Pico C SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [Pico SDK Git Repository](https://github.com/raspberrypi/pico-sdk)

### Tools Documentation

- [CMake Official](https://cmake.org/cmake/help/latest/)
- [Ninja Build](https://ninja-build.org/)
- [ARM GCC Toolchain](https://developer.arm.com/downloads/-/gnu-rm)
- [OpenOCD](http://openocd.org/)

### Community

- [Raspberry Pi Forums](https://forums.raspberrypi.com/)
- [Pico SDK GitHub Issues](https://github.com/raspberrypi/pico-sdk/issues)
- [Stack Overflow](https://stackoverflow.com/questions/tagged/raspberry-pi-pico)

---

## 📝 Documentation Version

- **Last Updated**: February 23, 2026
- **Pico SDK Version**: 2.2.0
- **Project Status**: Active Development

---

## 🤝 Contributing

When adding new features:

1. Update relevant documentation files
2. Add code examples to [README.md](README.md)
3. Update [drivers/README.md](drivers/README.md) if adding drivers
4. Update this index if adding new documentation

---

## 💡 Tips for Success

1. **Start with [QUICKSTART.md](QUICKSTART.md)** - Get working immediately
2. **Reference [README.md](README.md)** - Official APIs and patterns
3. **Check [CMAKE_GUIDE.md](CMAKE_GUIDE.md)** - Build system help
4. **Use [VSCODE_GUIDE.md](VSCODE_GUIDE.md)** - Development tools
5. **Read [drivers/README.md](drivers/README.md)** - Hardware details
6. **Copy [CMakeLists.template.txt](CMakeLists.template.txt)** - Start new projects

---

**Need help?** Start with [QUICKSTART.md](QUICKSTART.md) or [README.md](README.md)!
