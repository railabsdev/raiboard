# .vscode Directory Configuration

Guide to VS Code workspace configuration files for RAIBoard.

---

## Overview

The `.vscode` directory contains workspace-specific VS Code configurations. These files control:

- **Build/run tasks** - Compile, load, and flash commands
- **Editor settings** - Formatting, margins, language-specific settings
- **Debug configurations** - Hardware debugging setup
- **Extension recommendations** - Suggested plugins for the project

---

## Key Files

### tasks.json - Build and Utility Tasks

**Purpose**: Defines commands that run via Ctrl+Shift+B or Task menu

**Pre-configured Tasks**:

1. **Compile Project** (Default Build Task)
   ```json
   {
       "label": "Compile Project",
       "type": "process",
       "isBuildCommand": true,
       "command": "${userHome}/.pico-sdk/ninja/v1.12.1/ninja",
       "args": ["-C", "${workspaceFolder}/build"],
       "group": "build"
   }
   ```
   - Runs: `ninja -C build`
   - Compiles all source files
   - Linked via: `Ctrl+Shift+B`

2. **Run Project**
   ```json
   {
       "label": "Run Project",
       "type": "process",
       "command": "${env:HOME}/.pico-sdk/picotool/2.2.0-a4/picotool/picotool",
       "args": ["load", "${command:raspberry-pi-pico.launchTargetPath}", "-fx"]
   }
   ```
   - Loads compiled binary to connected Pico
   - Via picotool USB interface

3. **Flash** (OpenOCD + CMSIS-DAP)
   ```json
   {
       "label": "Flash",
       "type": "process",
       "command": "${userHome}/.pico-sdk/openocd/0.12.0+dev/openocd.exe",
       "args": [
           "-s", "${userHome}/.pico-sdk/openocd/0.12.0+dev/scripts",
           "-f", "interface/cmsis-dap.cfg",
           "-f", "target/${command:raspberry-pi-pico.getTarget}.cfg",
           ...
       ]
   }
   ```
   - Professional debug probe programming
   - Requires CMSIS-DAP hardware

4. **Rescue Reset**
   - Recovers unresponsive Pico

5. **RISC-V Reset (RP2350)**
   - For RP2350 boards only

**Custom Task Variables**:
- `${userHome}` - User home directory
- `${workspaceFolder}` - Project root
- `${env:HOME}` - HOME environment variable
- `${command:...}` - Pico extension commands

---

### settings.json - VS Code Configuration

**Purpose**: Workspace-specific editor and extension settings

**Common Settings**:

```json
{
    // Editor behavior
    "editor.formatOnSave": true,
    "editor.tabSize": 4,
    "editor.insertSpaces": true,
    "editor.renderWhitespace": "all",
    
    // File associations
    "[c]": {
        "editor.defaultFormatter": "ms-vscode.c-cpp",
        "editor.formatOnSave": true
    },
    "[cpp]": {
        "editor.defaultFormatter": "ms-vscode.c-cpp",
        "editor.formatOnSave": true
    },
    
    // CMake configuration
    "cmake.configureOnOpen": true,
    "cmake.sourceDirectory": "${workspaceFolder}",
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.generator": "Ninja",
    
    // C/C++ IntelliSense
    "C_Cpp.intelliSenseEngine": "default",
    
    // Python (if used)
    "python.linting.enabled": true
}
```

**Override Priority**:
- Workspace settings (`.vscode/settings.json`) override
- User settings (`~/.config/Code/User/settings.json`)

---

### launch.json - Debug Configuration

**Purpose**: Hardware debugging with breakpoints

**Example Configuration** (requires CMSIS-DAP):

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
            ],
            "preLaunchTask": "Compile Project",
            "stopAtEntry": true,
            "showDevDebugOutput": "parsed"
        }
    ]
}
```

**Key Settings**:
- `type: "cortex-debug"` - ARM debugging
- `executable` - Path to .elf file
- `servertype: "openocd"` - Use OpenOCD
- `configFiles` - Hardware-specific configs
- `preLaunchTask` - Build before debugging
- `stopAtEntry` - Pause at `main()`

---

### c_cpp_properties.json - IntelliSense Configuration

**Purpose**: C/C++ language server configuration

**Typical Configuration**:

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

**Key Fields**:
- `includePath` - Where to find headers
- `defines` - C preprocessor defines
- `compilerPath` - ARM GCC compiler
- `intelliSenseMode` - Engine to use

---

### extensions.json - Recommended Extensions

**Purpose**: Suggest extensions when project opens

```json
{
    "recommendations": [
        "raspberrypi.raspberry-pi-pico",
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "twxs.cmake",
        "ms-vscode.vscode-serial-monitor",
        "marus25.cortex-debug"
    ]
}
```

When you open the project, VS Code shows notification to install these.

---

## Important Variable References

### VS Code Variables

| Variable | Value |
|----------|-------|
| `${workspaceFolder}` | `/home/skgandre/raiboard` |
| `${workspaceFolderBasename}` | `raiboard` |
| `${file}` | Current open file |
| `${fileDirname}` | Current file directory |
| `${fileBasename}` | Current file name only |

### Environment Variables

| Variable | Purpose |
|----------|---------|
| `${env:HOME}` | User home directory |
| `${env:PATH}` | System PATH |
| `${env:PICO_SDK_PATH}` | Pico SDK location |

### Raspberry Pi Pico Extension Variables

| Variable | Value |
|----------|-------|
| `${command:raspberry-pi-pico.launchTargetPath}` | Compiled .uf2 file path |
| `${command:raspberry-pi-pico.getTarget}` | Target board (rp2040, rp2350, etc) |
| `${command:raspberry-pi-pico.getChip}` | Chip type |

---

## Creating Custom Tasks

Add new tasks to `tasks.json`:

### Example: Clean Build Task

```json
{
    "label": "Clean Build",
    "type": "shell",
    "command": "rm",
    "args": ["-rf", "${workspaceFolder}/build"],
    "group": "build",
    "presentation": {
        "reveal": "always"
    }
}
```

### Example: Format Code Task

```json
{
    "label": "Format Code",
    "type": "shell",
    "command": "find",
    "args": [
        "${workspaceFolder}",
        "-name", "*.cpp",
        "-o", "-name", "*.hpp",
        "-o", "-name", "*.c",
        "-o", "-name", "*.h"
    ],
    "group": "build"
}
```

Then run via: `Ctrl+Shift+P` → "Run Task" → Pick task name

---

## VS Code Keyboard Shortcuts for This Project

### Build and Compilation

| Shortcut | Command | Task |
|----------|---------|------|
| `Ctrl+Shift+B` | Run Build Task | Compile Project |
| `Ctrl+Shift+P` → Task | Show Tasks | All available tasks |
| `` Ctrl+` `` | Toggle Terminal | Edit in terminal |

### Debug (with launch.json)

| Shortcut | Command |
|----------|---------|
| `F5` | Start Debugging |
| `F9` | Toggle Breakpoint |
| `F10` | Step Over |
| `F11` | Step Into |
| `Shift+F11` | Step Out |

### Navigation

| Shortcut | Command |
|----------|---------|
| `Ctrl+P` | Quick File Open |
| `Ctrl+G` | Go to Line |
| `F2` | Rename Symbol |

---

## Troubleshooting Configuration

### "Build task not working"

**Check**:
```bash
# Verify paths exist
ls ${userHome}/.pico-sdk/ninja/v1.12.1/ninja
ls ${userHome}/.pico-sdk/picotool/2.2.0-a4/picotool/picotool

# Run manually
cd /home/skgandre/raiboard/build
ninja
```

**Fix**: Update paths in `tasks.json` if your installation differs

### "IntelliSense shows errors"

**Check**: `c_cpp_properties.json` paths exist

**Fix**: Run command palette:
1. Ctrl+Shift+P
2. "C/C++: Select IntelliSense Configuration"
3. Choose correct compiler path

### "Recommend extensions popup"

**Appears when**: `extensions.json` exists

**Dismiss**: Click "X" or install suggested extensions

To stop recommendations, delete `extensions.json`

### "Can't connect debugger"

**Verify**:
1. CMSIS-DAP hardware connected
2. OpenOCD installed: `which openocd`
3. Paths in `launch.json` are correct
4. Run "Compile Project" first

---

## File Maintenance

### When to Update Configuration

- **New extension** → Add to `extensions.json`
- **New build tool** → Update `tasks.json`
- **Different toolchain** → Update `c_cpp_properties.json`
- **New project layout** → Update `settings.json`

### When to Leave As-Is

- Don't edit `tasks.json` paths unless your SDK moved
- Don't commit personal preferences to `settings.json` (only workspace settings)
- Don't change `launch.json` unless debugging setup changes

### Version Control

What to commit:
- ✅ `tasks.json` - Build tasks (shared)
- ✅ `settings.json` - Project preferences (shared)
- ✅ `extensions.json` - Recommended extensions (shared)
- ✅ `c_cpp_properties.json` - IntelliSense config (shared)

What to ignore:
- ❌ Personal user settings
- ❌ Debug launch history
- ❌ Temporary files

---

## Advanced Configuration

### Multi-Target Building

For projects with multiple executables, add tasks for each:

```json
{
    "label": "Build DisplayApp",
    "type": "shell",
    "command": "ninja",
    "args": ["-C", "${workspaceFolder}/build", "display_app"],
    "group": "build"
},
{
    "label": "Build LoRaApp",
    "type": "shell",
    "command": "ninja",
    "args": ["-C", "${workspaceFolder}/build", "lora_app"],
    "group": "build"
}
```

### Conditional Task Running

Run tasks in sequence:

```json
{
    "label": "Full Build and Load",
    "type": "shell",
    "command": "echo 'Building...'",
    "dependsOn": ["Compile Project", "Run Project"],
    "group": "build"
}
```

## Summary

The `.vscode` configuration enables:

✅ **One-click compiling** - `Ctrl+Shift+B`  
✅ **Easy loading** - Run Project task  
✅ **Code formatting** - Format on Save  
✅ **Code intelligence** - IntelliSense  
✅ **Hardware debugging** - F5 with breakpoints  
✅ **IDE recommendations** - Extension suggestions  

**These files make development smooth and efficient!**

---

See [VSCODE_GUIDE.md](VSCODE_GUIDE.md) for complete VS Code usage guide.
