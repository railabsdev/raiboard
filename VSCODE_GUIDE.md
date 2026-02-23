# VS Code Configuration and Tasks

Complete guide to using VS Code with RaiBoard.

---

## Table of Contents

- [Overview](#overview)
- [Required Extensions](#required-extensions)
- [VS Code Configuration](#vs-code-configuration)
- [Build Tasks](#build-tasks)
- [Debugging](#debugging)
- [Keyboard Shortcuts](#keyboard-shortcuts)
- [Settings Explained](#settings-explained)
- [Troubleshooting](#troubleshooting)

---

## Overview

VS Code is configured with:
1. **Raspberry Pi Pico VS Code Extension** - For Pico-specific features
2. **Extension settings** - Configured in `.vscode/settings.json`
3. **Build tasks** - Defined in `.vscode/tasks.json`
4. **Launch configurations** - For debugging (if using CMSIS-DAP)

---

## Required Extensions

### Essential

- **Raspberry Pi Pico** (raspberrypi / raspberry-pi-pico)
  - Official Pico IDE support
  - Project generation
  - Building and flashing
  - Board detection

### Recommended

- **C/C++** (ms-vscode.cpptools) - IntelliSense, debugging
- **CMake** (twxs.cmake) - CMake syntax highlighting
- **CMake Tools** (ms-vscode.cmake-tools) - CMake integration
- **Cortex-Debug** (marus25.cortex-debug) - Hardware debugging
- **Serial Monitor** (ms-vscode.vscode-serial-monitor) - USB serial

### Install Extensions

1. Open **Extensions** panel (Ctrl+Shift+X)
2. Search for each extension name
3. Click "Install"
4. Reload VS Code (Ctrl+Shift+P → "Reload Window")

---

## VS Code Configuration

### Folder Structure

```
.vscode/
├── settings.json         # Project settings (workspace-specific)
├── launch.json          # Debug configurations
├── tasks.json           # Build/run tasks
├── extensions.json      # Recommended extensions (optional)
└── c_cpp_properties.json # C++ IntelliSense paths (optional)
```

### Configuration Files

#### settings.json

Controls VS Code behavior for this workspace. Example:

```json
{
    "python.linting.enabled": true,
    "editor.formatOnSave": true,
    "[c]": {
        "editor.defaultFormatter": "ms-vscode.c-cpp",
        "editor.formatOnSave": true
    },
    "[cpp]": {
        "editor.defaultFormatter": "ms-vscode.c-cpp",
        "editor.formatOnSave": true
    },
    "cmake.configureOnOpen": true,
    "cmake.sourceDirectory": "${workspaceFolder}",
    "cmake.buildDirectory": "${workspaceFolder}/build"
}
```

#### tasks.json

Defines build and utility tasks. Available tasks in workspace:

1. **Compile Project** - CMake + Ninja build
2. **Run Project** - Load via picotool
3. **Flash** - Program via OpenOCD/CMSIS-DAP
4. **Rescue Reset** - Recover from failed state
5. **RISC-V Reset (RP2350)** - For RP2350 boards only

---

## Build Tasks

### How to Run Tasks

**Method 1: Command Palette**
1. Press `Ctrl+Shift+P`
2. Type "Run Task"
3. Select task name

**Method 2: Keyboard Shortcut**
- `Ctrl+Shift+B` - Runs the default build task ("Compile Project")

**Method 3: Terminal Menu**
- Terminal → Run Task

### Task Details

#### 1. Compile Project

**Command**: `ninja -C build`

**What it does**:
- Compiles all source files
- Links libraries
- Generates `.elf`, `.uf2`, `.bin`, `.dis` files
- Outputs to `build/` directory

**Output**:
```
[1/20] Building C object ...
[2/20] Building C object ...
...
[20/20] Linking CXX executable my_project/my_project.elf
Built target my_project
```

**Key bindings**: `Ctrl+Shift+B`

---

#### 2. Run Project

**Command**: `picotool load [binary] -fx`

**What it does**:
- Loads compiled `.uf2` file onto connected Pico
- `-f` = flush output
- `-x` = exit and run

**Requirements**:
- Pico connected via USB
- picotool installed (`~/.pico-sdk/picotool/...`)
- Successful build (run "Compile Project" first)

**Output**:
```
Loading into RP2040...
Programming 1 blocks total (42816 bytes)
[2/2] @0x00000000 size 0xa710
```

**Troubleshooting**:
- "Pico not detected" - Check USB connection
- "Command not found" - Install picotool

---

#### 3. Flash

**Command**: `openocd -s scripts -f interface/cmsis-dap.cfg -f target/rp2040.cfg ...`

**What it does**:
- Programs via OpenOCD and CMSIS-DAP debugger
- Verifies programmed data
- Resets device
- Exits OpenOCD

**Requirements**:
- CMSIS-DAP compatible debugger connected (Pico + debug probe)
- OpenOCD installed
- SWD pins connected (SWCLK, SWDIO, GND)

**Output**:
```
Info : DEPRECATED! use 'adapter speed' not 'jtag_khz'
Info : DEPRECATED! use 'adapter speed' not 'jtag_khz'
adapter speed: 5000 kHz
...
```

**Used For**: Professional debugging with breakpoints and variable inspection

---

#### 4. Rescue Reset

**Command**: OpenOCD with rescue-specific config

**What it does**:
- Connects to unresponsive/crashed Pico
- Executes reset halt sequence
- Recovers device to working state

**When to use**:
- Pico won't respond
- Infinite loops without reset capability
- After failed flashing

---

#### 5. RISC-V Reset (RP2350)

**Command**: OpenOCD with RP2350-specific commands

**What it does**:
- Selects RISC-V core architecture
- Resets RISC-V cores on RP2350
- Checks core status

**Only for**: RP2350 boards when using RISC-V core

---

## Debugging

### Debugger Setup (Optional)

Requires CMSIS-DAP compatible debugger. Example setup:

1. **Hardware**:
   - Pico W with debug probe (Raspberry Pi Debug Probe recommended)
   - Or use another Pico as debug probe

2. **Connections**:
   ```
   Debug Probe → Target Pico
   SWCLK → GPIO 24
   SWDIO → GPIO 25
   GND → GND
   ```

3. **VS Code Configuration** (`.vscode/launch.json`):
   ```json
   {
       "version": "0.2.0",
       "configurations": [
           {
               "name": "Pico Debug",
               "type": "cortex-debug",
               "request": "launch",
               "cwd": "${workspaceRoot}",
               "executable": "${command:raspberry-pi-pico.launchTargetPath}",
               "servertype": "openocd",
               "searchDir": [
                   "${userHome}/.pico-sdk/openocd/0.12.0+dev/scripts"
               ],
               "configFiles": [
                   "interface/cmsis-dap.cfg",
                   "target/rp2040.cfg"
               ]
           }
       ]
   }
   ```

4. **Debug in VS Code**:
   - Press `F5` or Ctrl+Shift+D
   - Set breakpoints (click left margin)
   - Step through code
   - Inspect variables

---

## Keyboard Shortcuts

### Building and Compilation

| Shortcut | Action |
|----------|--------|
| `Ctrl+Shift+B` | Compile Project (default build task) |
| `Ctrl+Shift+P` → "Run Task" | Show all tasks |
| `Ctrl+` ` | Toggle integrated terminal |

### Navigation

| Shortcut | Action |
|----------|--------|
| `Ctrl+P` | Quick file open |
| `Ctrl+G` | Go to line |
| `Ctrl+F` | Find in file |
| `Ctrl+H` | Find and replace |
| `Ctrl+Shift+F` | Find in folder (entire workspace) |

### Code Editing

| Shortcut | Action |
|----------|--------|
| `Ctrl+/` | Toggle line comment |
| `Shift+Alt+/` | Toggle block comment |
| `F2` | Rename symbol |
| `Ctrl+Space` | Trigger IntelliSense |
| `Ctrl+D` | Select next occurrence |

### Debugging

| Shortcut | Action |
|----------|--------|
| `F5` | Start debugging |
| `F9` | Toggle breakpoint |
| `F10` | Step over |
| `F11` | Step into |
| `Shift+F11` | Step out |

---

## Settings Explained

### Editor Settings

```json
"editor.formatOnSave": true        // Auto-format on save
"editor.tabSize": 4                // Indent width
"editor.insertSpaces": true        // Use spaces not tabs
"editor.renderWhitespace": "all"   // Show spaces/tabs
"editor.rulers": [80, 120]         // Show column guides
```

### C/C++ IntelliSense

```json
"C_Cpp.intelliSenseEngine": "default"  // Use built-in engine
"C_Cpp.clangFormatStyle": "file"       // Use .clang-format file
```

### CMake/Build

```json
"cmake.configureOnOpen": true      // Auto-configure on open
"cmake.sourceDirectory": "${workspaceFolder}"
"cmake.buildDirectory": "${workspaceFolder}/build"
"cmake.generator": "Ninja"         // Use Ninja build system
```

### File Associations

```json
"files.associations": {
    "*.hpp": "cpp",
    "*.h": "c",
    "*.ld": "linkerscript"
}
```

---

## Workspace vs User Settings

**Workspace settings** (`.vscode/settings.json`):
- Apply ONLY to this project
- Committed to repository
- Everyone on project uses same settings

**User settings** (`~/.config/Code/User/settings.json` on Linux):
- Apply to ALL VS Code windows
- Personal preferences
- Not committed to repository

**Override priority**: Workspace settings override user settings

---

## Extension Installation in VS Code

### Via GUI

1. Click Extensions icon (Ctrl+Shift+X)
2. Search for extension
3. Click Install
4. Wait for installation to complete
5. Reload VS Code if prompted

### Via Command Palette

1. Press Ctrl+Shift+P
2. Type "Install Extension"
3. Type extension name
4. Click result

### Recommended Extensions Config

`.vscode/extensions.json`:
```json
{
    "recommendations": [
        "raspberrypi.raspberry-pi-pico",
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "twxs.cmake",
        "ms-vscode.vscode-serial-monitor"
    ]
}
```

When opening project, VS Code shows notification to install recommended extensions.

---

## Common Workflows

### Typical Build-and-Load Workflow

1. **Edit source code**
   - Write in `my_project/main.cpp`

2. **Build**
   - Press `Ctrl+Shift+B` (or run "Compile Project" task)
   - Wait for compilation to complete
   - Check for errors in Problems panel

3. **Load to Pico**
   - Run "Run Project" task
   - Pico device resets and runs code
   - Use Serial Monitor to view output

4. **Debug**
   - Check serial output for issues
   - Modify code and rebuild
   - Repeat

### Debugging Workflow (with CMSIS-DAP)

1. **Set Breakpoint**
   - Click left margin in code where you want to pause
   - Red dot appears

2. **Start Debugging**
   - Press `F5`
   - Code runs until first breakpoint
   - Debugger pauses, showing current line

3. **Inspect State**
   - Hover over variables to see values
   - Watch panel shows specific variables
   - Call stack shows function call chain

4. **Step Through**
   - `F10` = step over (execute line)
   - `F11` = step into (enter function)
   - `Shift+F11` = step out (exit function)

5. **Continue**
   - `F5` = resume execution
   - Stops at next breakpoint or end

---

## Serial Monitor Usage

For viewing device output via USB:

### Setup

1. Install "Serial Monitor" extension (ms-vscode.vscode-serial-monitor)
2. Device must have `stdio_init_all()` and `pico_enable_stdio_usb(1)`

### Open Monitor

- Command Palette → "Open Serial Monitor"
- Or click "Serial Monitor" in status bar

### Usage

```
// In your code
printf("Hello from Pico!\n");
```

**Output appears in Serial Monitor**:
```
[12:34:56.123] Hello from Pico!
```

---

## Formatting Code

### Automatic Formatting

If `"editor.formatOnSave": true` in settings.json:
- Press Ctrl+S (save file)
- Code automatically formatted

### Manual Formatting

- Select code
- Right-click → Format Selection (or Shift+Alt+F)
- Or Ctrl+Shift+P → Format Document

### .clang-format Configuration

Create `.clang-format` in project root for consistent formatting:

```yaml
---
Language: Cpp
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
UseTab: Never
```

---

## Troubleshooting VS Code

### "Pico extension not working"

**Cause**: Extension not installed or not auto-detected

**Fix**:
1. Install Raspberry Pi Pico extension
2. Reload VS Code (Ctrl+Shift+P → Reload Window)
3. Check extension is enabled (Extensions panel)

### "Build fails with 'cmake: not found'"

**Cause**: CMake not installed or not in PATH

**Fix**:
```bash
sudo apt-get install cmake
```

Then reload VS Code.

### "IntelliSense not working / Red squiggles everywhere"

**Cause**: C++ extension not configured properly

**Fix**:
1. Create `.vscode/c_cpp_properties.json`:
   ```json
   {
       "version": 4,
       "configurations": [
           {
               "name": "Pico",
               "includePath": [
                   "${workspaceFolder}/drivers/c_drivers/**",
                   "${workspaceFolder}/drivers/cpp_wrappers",
                   "${env:PICO_SDK_PATH}/src/**"
               ],
               "defines": [
                   "PICO_BOARD=pico"
               ],
               "compilerPath": "arm-none-eabi-gcc",
               "cStandard": "c11",
               "cppStandard": "c++17",
               "intelliSenseMode": "arm-gcc-arm"
           }
       ]
   }
   ```

2. Manually select compiler path (Ctrl+Shift+P → "Select IntelliSense Configuration")

### "Serial Monitor not opening"

**Cause**: 
- Device not recognized
- Wrong COM port selected
- USB not working

**Fix**:
1. Check USB cable (use data cable, not power-only)
2. Compile with `pico_enable_stdio_usb(1)`
3. Check Device Manager for COM ports
4. Manually select port in Serial Monitor dropdown

### "Can't connect to debugger"

**Cause**:
- Debug probe not detected
- Wrong config file
- SWD pins not connected

**Fix**:
1. Verify debug probe physically connected
2. Check SWD pin assignments
3. Run "Rescue Reset" task first
4. Check OpenOCD script location

---

## Performance Tips

### Reduce IntelliSense Blocking

In settings.json:
```json
"C_Cpp.intelliSenseEngine": "disabled"  // Only if using Clangd
```

### Faster Compilation

- Use incremental builds (Ninja automatically does this)
- Don't clean build unless necessary
- Use SSD for build directory

### Reduce Memory Usage

```json
"extensions.ignoreRecommendations": true
```

Disable unused extensions in Extensions panel.

---

## Advanced: Custom Tasks

Add tasks to `.vscode/tasks.json`:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Clean Build",
            "type": "shell",
            "command": "rm",
            "args": ["-rf", "${workspaceFolder}/build"],
            "group": "build"
        },
        {
            "label": "Full Rebuild",
            "type": "shell",
            "command": "bash",
            "args": ["-c", "rm -rf ${workspaceFolder}/build && mkdir -p ${workspaceFolder}/build && cd ${workspaceFolder}/build && cmake .. -GNinja && ninja"],
            "dependsOn": ["Compile Project"],
            "group": "build"
        }
    ]
}
```

Then run via Command Palette → "Run Task" → "Full Rebuild"

---

## Summary

**VS Code Essentials**:
1. Install Raspberry Pi Pico extension
2. Use `Ctrl+Shift+B` to compile
3. Use Tasks for build/load/flash
4. Use Serial Monitor for debugging
5. Use `F5` for hardware debugging (with CMSIS-DAP)

**Quick Reference**:
- Compile: `Ctrl+Shift+B`
- Find: `Ctrl+F`
- Rename: `F2`
- Debug: `F5`
- Breakpoint: `F9`

---

See [README.md](README.md) for overall project and [CMAKE_GUIDE.md](CMAKE_GUIDE.md) for build system details.
