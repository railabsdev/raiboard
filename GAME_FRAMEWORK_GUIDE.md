# RAIBoard Game Framework - Quick Start Guide

## Introduction

The RAIBoard game framework provides an abstract, object-oriented architecture for building games on the Raspberry Pi Pico with ILI9341 display and XPT2046 touch screen.

## Architecture Overview

```
┌─────────────────────────────────────┐
│           Your Game                  │
│    (extends Game base class)         │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│         Game Framework               │
│  - Game lifecycle management         │
│  - GameObject management             │
│  - Collision detection               │
│  - Rendering pipeline                │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│         Screen + Hardware            │
│  - ILI9341 display (240x320)        │
│  - XPT2046 touch screen             │
│  - SPI communication                 │
└─────────────────────────────────────┘
```

## Step-by-Step: Creating Your First Game

### 1. Project Setup

Create a new directory under `examples/`:

```bash
cd examples
mkdir my_game
cd my_game
```

### 2. Create CMakeLists.txt

```cmake
project(my_game C CXX ASM)

add_executable(my_game 
    main.cpp
)

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
pico_enable_stdio_uart(my_game 0)

pico_add_extra_outputs(my_game)
```

### 3. Create main.cpp

```cpp
#include <stdio.h>
#include "pico/stdlib.h"
#include "Screen.hpp"
#include "Game.hpp"
#include "Sprite.hpp"
#include "Vector.hpp"

// Define a simple Player object
class Player : public GameObject {
public:
    Player(Vector2 pos) 
        : GameObject(pos, BoxCollider(32.0f, 32.0f), 0xF800) {} // Red
    
    void update(float deltaTime) override {
        // Simple physics
        GameObject::update(deltaTime);
        
        // Bounce off walls
        if (position.x < 0 || position.x > 208) velocity.x = -velocity.x;
        if (position.y < 0 || position.y > 288) velocity.y = -velocity.y;
    }
    
    void render(ILI9341_TFT& display) override {
        if (!visible) return;
        display.fillRect(
            (uint16_t)position.x, 
            (uint16_t)position.y, 
            32, 32, 
            color
        );
    }
};

// Your game class
class MyGame : public Game {
private:
    Player* player;
    
public:
    MyGame(Screen& scr) : Game(scr), player(nullptr) {}
    
    void onInit() override {
        // Create player at center of screen
        player = new Player(Vector2(104, 144));
        player->setVelocity(Vector2(50.0f, 30.0f));
        
        // Note: No need to add to gameObjects if managing manually
    }
    
    void onUpdate(float deltaTime) override {
        // Update player
        if (player) {
            player->update(deltaTime);
        }
        
        // Handle touch input
        uint16_t tx, ty;
        if (getScreen().readTouch(tx, ty)) {
            // Move player toward touch
            Vector2 target((float)tx, (float)ty);
            Vector2 dir = (target - player->getPosition()).normalized();
            player->setVelocity(dir * 100.0f);
        }
    }
    
    void onRender() override {
        // Clear screen
        getDisplay().fillScreen(0x0000);
        
        // Render player
        if (player) {
            player->render(getDisplay());
        }
        
        // Draw some text
        getDisplay().setCursor(10, 10);
        getDisplay().setTextColor(0xFFFF);
        getDisplay().print("My First Game!");
    }
    
    void onShutdown() override {
        delete player;
    }
};

int main() {
    stdio_init_all();
    sleep_ms(1000);
    
    // Create screen and game
    Screen screen;
    MyGame game(screen);
    
    // Run game loop
    game.run();
    
    return 0;
}
```

### 4. Add to Root CMakeLists.txt

Edit `/home/skgandre/raiboard/CMakeLists.txt` and add:
```cmake
add_subdirectory(examples/my_game)
```

### 5. Build and Flash

```bash
cd /home/skgandre/raiboard/build
cmake -G Ninja ..
ninja my_game

# Flash to Pico (hold BOOTSEL button while connecting USB)
# Copy build/examples/my_game/my_game.uf2 to RPI-RP2 drive
```

## Using the GameObject System

The GameObject system allows for automatic management of game entities.

### Example: Coin Collection Game

```cpp
class Coin : public GameObject {
public:
    Coin(Vector2 pos) 
        : GameObject(pos, BoxCollider(16, 16), 0xFFE0) {} // Yellow
    
    void render(ILI9341_TFT& display) override {
        display.fillCircle(
            (uint16_t)position.x + 8, 
            (uint16_t)position.y + 8, 
            8, 
            0xFFE0
        );
    }
    
    Type getType() const override { return Type::Generic; }
};

class CoinGame : public Game {
private:
    int score;
    
public:
    CoinGame(Screen& scr) : Game(scr), score(0) {}
    
    void onInit() override {
        // Spawn coins
        for (int i = 0; i < 10; i++) {
            float x = (rand() % 200) + 20.0f;
            float y = (rand() % 280) + 20.0f;
            addGameObject(std::make_unique<Coin>(Vector2(x, y)));
        }
        
        // Add player
        auto player = std::make_unique<Player>(Vector2(120, 160));
        player->setVelocity(Vector2(0, 0));
        addGameObject(std::move(player));
    }
    
    void onCollision(GameObject& a, GameObject& b) override {
        // Check if player collided with coin
        if (a.getType() == GameObject::Type::Player || 
            b.getType() == GameObject::Type::Player) {
            
            // Deactivate the coin
            if (a.getType() != GameObject::Type::Player) a.setActive(false);
            if (b.getType() != GameObject::Type::Player) b.setActive(false);
            
            score++;
        }
    }
    
    void onRender() override {
        // Draw score
        char buf[32];
        snprintf(buf, sizeof(buf), "Score: %d", score);
        getDisplay().setCursor(10, 10);
        getDisplay().print(buf);
    }
};
```

## Using Sprites

Sprites allow you to render bitmap graphics with transparency.

### 1. Define Sprite Data

Create an `assets.hpp` file:
```cpp
#pragma once
#include <cstdint>

// 16x16 sprite (RGB565 format, 2 bytes per pixel)
const uint8_t MY_SPRITE[] = {
    0xFF, 0xFF, 0xFF, 0xFF, /* ... */
};

const int MY_SPRITE_WIDTH = 16;
const int MY_SPRITE_HEIGHT = 16;
```

### 2. Use Sprite Class

```cpp
#include "Sprite.hpp"
#include "assets.hpp"

class SpriteObject : public Sprite {
public:
    SpriteObject(Vector2 pos) 
        : Sprite(MY_SPRITE, MY_SPRITE_WIDTH, MY_SPRITE_HEIGHT, pos, 0x0000) {
        // 0x0000 is transparent color (black)
    }
    
    void update(float deltaTime) override {
        // Your update logic
        Sprite::update(deltaTime);
    }
};
```

## Display & Touch API Reference

### Screen Methods
```cpp
Screen screen;

// Get underlying display
ILI9341_TFT& display = screen.display();

// Read touch coordinates
uint16_t x, y;
bool touched = screen.readTouch(x, y);

// Check if touch is pressed (without reading)
bool pressed = screen.isTouchPressed();

// Clear screen
screen.clear(0x0000); // Black
```

### ILI9341_TFT Methods
```cpp
display.fillScreen(color);
display.fillRect(x, y, w, h, color);
display.drawRect(x, y, w, h, color);
display.drawPixel(x, y, color);
display.drawLine(x1, y1, x2, y2, color);
display.fillCircle(x, y, radius, color);

// Text
display.setCursor(x, y);
display.setTextColor(color);
display.print("Hello");
```

### Color Constants (RGB565)
```cpp
0x0000  // Black
0xFFFF  // White
0xF800  // Red
0x07E0  // Green
0x001F  // Blue
0xFFE0  // Yellow
0xF81F  // Magenta
0x07FF  // Cyan
```

## Common Patterns

### Frame Rate Control
```cpp
void Game::run() {
    onInit();
    
    const uint32_t frame_time_ms = 16; // ~60 FPS
    
    while (running) {
        uint32_t start = to_ms_since_boot(get_absolute_time());
        
        update();
        render();
        
        uint32_t elapsed = to_ms_since_boot(get_absolute_time()) - start;
        if (elapsed < frame_time_ms) {
            sleep_ms(frame_time_ms - elapsed);
        }
        
        frameCount++;
    }
    
    onShutdown();
}
```

### Gravity & Physics
```cpp
void update(float deltaTime) override {
    const Vector2 gravity(0, 200.0f); // 200 pixels/sec² downward
    addForce(gravity * deltaTime);
    
    GameObject::update(deltaTime);
}
```

### Button State Tracking
```cpp
bool lastTouchState = false;

void onUpdate(float deltaTime) override {
    bool touchPressed = screen.isTouchPressed();
    
    if (touchPressed && !lastTouchState) {
        // Touch just pressed
        onTouchDown();
    } else if (!touchPressed && lastTouchState) {
        // Touch just released
        onTouchUp();
    }
    
    lastTouchState = touchPressed;
}
```

## Debugging Tips

### Serial Output
```cpp
pico_enable_stdio_usb(my_game 1)  // Enable USB serial

printf("Player pos: %.0f, %.0f\n", player->getPosition().x, player->getPosition().y);
```

### Performance Monitoring
```cpp
uint32_t frameCount = getFrameCount();
if (frameCount % 60 == 0) {
    printf("Frame: %u\n", frameCount);
}
```

### Visual Debug Overlays
```cpp
void onRender() override {
    // Draw collision boxes
    for (size_t i = 0; i < getGameObjectCount(); i++) {
        GameObject* obj = getGameObjectAt(i);
        BoxCollider& col = obj->getCollider();
        renderSpriteOutline(
            obj->getPosition().x,
            obj->getPosition().y,
            col.getWidth(),
            col.getHeight(),
            0x07E0 // Green
        );
    }
}
```

## Example Games Included

1. **game1** - Coin collection with falling obstacles
   - Location: `examples/game1/`
   - Demonstrates: Sprite rendering, collision, audio (buzzer)

2. **asteroids** - (needs update) Space shooter
   - Location: `examples/asteroids/`
   - Status: Commented out, needs refactoring to new framework

## Additional Resources

- [Hardware Pinout](wiki/Drivers.md)
- [Full API Documentation](wiki/Full-Documentation.md)
- [CMake Build Guide](wiki/CMake-Guide.md)

## Common Issues

### Display not initializing
- Check SPI pins in `Screen.cpp` match your hardware
- Verify display power is connected

### Touch not responding
- Ensure IRQ pin is connected (default: GP22)
- Check touch calibration constants in `xpt2046.h`

### Build errors
```bash
# Clean build
cd build
rm -rf *
cmake -G Ninja ..
ninja my_game
```

### Slow rendering
- Minimize `drawPixel()` calls in render loop
- Use `fillRect()` for large areas
- Consider double-buffering for complex scenes

---

Happy coding! 🎮
