# RAIBoard - Quick Reference Card

## 🚀 Quick Commands

```bash
# Build
cd raiboard/build
cmake -G Ninja ..
ninja game1

# Clean rebuild
rm -rf build && mkdir build && cd build && cmake -G Ninja .. && ninja game1

# Flash to Pico
# 1. Hold BOOTSEL button on Pico
# 2. Connect USB
# 3. Copy build/examples/game1/game1.uf2 to RPI-RP2 drive
```

## 📐 Display Specs

- **Resolution**: 240x320 pixels
- **Colors**: RGB565 (16-bit)
- **Controller**: ILI9341
- **Interface**: SPI

## 🎨 Color Codes (RGB565)

```cpp
0x0000  // Black
0xFFFF  // White
0xF800  // Red
0x07E0  // Green  
0x001F  // Blue
0xFFE0  // Yellow
0xF81F  // Magenta
0x07FF  // Cyan
0x7BEF  // Light Gray
```

## 🎮 Game Framework - Minimal Example

```cpp
#include "Screen.hpp"
#include "Game.hpp"

class MyGame : public Game {
public:
    MyGame(Screen& s) : Game(s) {}
    
    void onInit() override {
        // Setup
    }
    
    void onUpdate(float dt) override {
        // Logic
    }
    
    void onRender() override {
        getDisplay().fillScreen(0x0000);
        // Draw
    }
};

int main() {
    stdio_init_all();
    Screen screen;
    MyGame game(screen);
    game.run();
}
```

## 📦 CMakeLists.txt Template

```cmake
project(my_game C CXX ASM)

add_executable(my_game main.cpp)

target_include_directories(my_game PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../displaylib_16bit_PICO/include
)

target_link_libraries(my_game
    pico_stdlib
    hardware_spi
    pico_game
    displaylib_16
)

pico_enable_stdio_usb(my_game 1)
pico_add_extra_outputs(my_game)
```

## 🖥️ Display API

```cpp
Screen screen;
ILI9341_TFT& d = screen.display();

// Fill
d.fillScreen(color);
d.fillRect(x, y, w, h, color);
d.fillCircle(cx, cy, r, color);

// Draw
d.drawPixel(x, y, color);
d.drawLine(x1, y1, x2, y2, color);
d.drawRect(x, y, w, h, color);
d.drawCircle(cx, cy, r, color);

// Text
d.setCursor(x, y);
d.setTextColor(color);
d.setTextSize(size);
d.print("Hello");
```

## 🖱️ Touch API

```cpp
// Read touch
uint16_t x, y;
if (screen.readTouch(x, y)) {
    // Touch at (x, y)
}

// Check if pressed
if (screen.isTouchPressed()) {
    // Touch detected
}
```

## 📡 LoRa API

```cpp
#include "LoRa.hpp"

LoRa radio;
radio.init(915000000, LORA_SF_12);

// Send
radio.send("Hello!");

// Receive
radio.startRx();
if (radio.available()) {
    uint8_t buf[256];
    size_t len = radio.read(buf, 256);
}

// Get signal info
int16_t rssi = radio.lastRssi();
int8_t snr = radio.lastSnr();
```

## 🎯 GameObject Pattern

```cpp
class Player : public GameObject {
public:
    Player(Vector2 pos) 
        : GameObject(pos, BoxCollider(32, 32), 0xFFFF) {}
    
    void update(float dt) override {
        // Update logic
        GameObject::update(dt);
    }
    
    void render(ILI9341_TFT& display) override {
        display.fillRect(position.x, position.y, 32, 32, color);
    }
    
    Type getType() const override { return Type::Player; }
};

// In game onInit()
addGameObject(std::make_unique<Player>(Vector2(120, 160)));
```

## 🎵 Audio (Buzzer)

```cpp
#include "hardware/pwm.h"

#define BUZZER_PIN 14

void init_buzzer() {
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f);
    pwm_init(slice, &config, true);
}

void play_tone(uint16_t freq, uint32_t duration_ms) {
    uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint32_t top = 125000000 / 4 / freq;
    pwm_set_wrap(slice, top);
    pwm_set_gpio_level(BUZZER_PIN, top / 2);
    sleep_ms(duration_ms);
    pwm_set_gpio_level(BUZZER_PIN, 0);
}
```

## 🔧 Pin Defaults

```
Display:
  MOSI: GP19    DC:   GP21
  MISO: GP16    RST:  GP20
  SCK:  GP18
  CS:   GP17

Touch:
  (Shares display SPI)
  IRQ: GP22

LoRa:
  MOSI: GP11    RST:  GP14
  MISO: GP12    BUSY: GP15
  SCK:  GP10
  CS:   GP13
```

## 🐛 Debug

```cpp
// Enable serial
pico_enable_stdio_usb(target 1)

// Print
printf("Debug: %d\n", value);

// FPS counter
if (getFrameCount() % 60 == 0) {
    printf("Frame: %u\n", getFrameCount());
}
```

## 📂 File Locations

```
Framework:       game/
Drivers:         drivers/
Examples:        examples/
Display lib:     displaylib_16bit_PICO/
LoRa lib:        lora/
Docs:            *.md files in root
Build output:    build/
```

## 🎓 Key Documentation

- `MERGE_SUMMARY.md` - What changed
- `GAME_FRAMEWORK_GUIDE.md` - Tutorials
- `REFACTOR_COMPLETE.md` - Final status
- `README.md` - Project overview
- `wiki/` - Detailed docs

## ⚡ Pro Tips

1. Use `Vector2` for positions
2. Call `GameObject::update(dt)` in overrides
3. Check `isActive()` before processing
4. Use `std::make_unique<>` for game objects
5. Clear screen in `onRender()` before drawing
6. Keep update logic separate from rendering
7. Use `sleep_ms(16)` for ~60 FPS
8. Test collision logic with visual debug rects

## 🔥 Common Pitfalls

- ❌ Forgetting to call `pico_add_extra_outputs()`
- ❌ Not initializing stdio before screen
- ❌ Calling `render()` inside `update()`
- ❌ Using `new` without `delete` (use smart pointers!)
- ❌ Not clearing screen before drawing
- ❌ Forgetting to link `displaylib_16`

---

**Keep this card handy while developing!** 🚀
