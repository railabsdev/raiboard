#pragma once

#include <vector>
#include <memory>
#include <cstring>
#include <cstdint>
#include "Display.hpp"
#include "GameObject.hpp"

class Game {
protected:
    Display& display;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    bool running;
    float deltaTime;
    uint32_t frameCount;

public:
    Game(Display& disp);
    virtual ~Game() = default;

    // Main game loop - call this in your main() function
    virtual void run();

    // Stop the game loop
    void stop();

    bool isRunning() const;

    // Get frame count since game started
    uint32_t getFrameCount() const;

private:
    // Update all game objects and handle logic
    void update();

    // Render all game objects
    void render();

    // Check collisions between all active objects
    void checkCollisions();

public:
    // Render a simple rectangular sprite
    void renderSprite(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

    // Render a hollow rectangle (outline)
    void renderSpriteOutline(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

    // Render a circle
    void renderCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color);

    // Render a circle (filled)
    void renderFilledCircle(uint16_t centerX, uint16_t centerY, uint16_t radius, uint16_t color);

    // Render a line
    void renderLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);

    // Render pixel data (sprite sheet or bitmap)
    void renderBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                      const uint16_t* pixelData);

    // Render pixel data with color key (transparent color)
    void renderBitmapTransparent(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                                 const uint16_t* pixelData, uint16_t transparentColor);

    // Add a game object to the scene
    void addGameObject(std::unique_ptr<GameObject> obj);

    // Find game object by ID
    GameObject* findGameObject(uint32_t id);

    // Get total number of active game objects
    size_t getGameObjectCount() const;

    // Get game object at index
    GameObject* getGameObjectAt(size_t index);

    // Remove game object at index
    void removeGameObject(size_t index);

    // Remove all game objects
    void clearGameObjects();

    // Get reference to display
    Display& getDisplay();

    // Virtual methods for user to override

    // Called once at game startup
    virtual void onInit() {}

    // Called every frame - user game logic here
    virtual void onUpdate(float deltaTime) {}

    // Called every frame for custom rendering (before game objects render)
    virtual void onRender() {}

    // Called when two game objects collide
    virtual void onCollision(GameObject& objA, GameObject& objB) {}

    // Called when game is shutting down
    virtual void onShutdown() {}
};
