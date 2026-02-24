#include "Game.hpp"
#include <algorithm>

Game::Game(Display& disp)
    : display(disp), running(true), deltaTime(0.016f), frameCount(0) {}

void Game::run() {
    onInit();

    while (running) {
        update();
        render();
        frameCount++;
    }

    onShutdown();
}

void Game::stop() {
    running = false;
}

bool Game::isRunning() const {
    return running;
}

uint32_t Game::getFrameCount() const {
    return frameCount;
}

void Game::update() {
    onUpdate(deltaTime);

    // Update all active game objects
    for (auto& obj : gameObjects) {
        if (obj->isActive()) {
            obj->update(deltaTime);
        }
    }

    // Check collisions between all active objects
    checkCollisions();

    // Remove inactive objects
    gameObjects.erase(
        std::remove_if(gameObjects.begin(), gameObjects.end(),
                       [](const std::unique_ptr<GameObject>& obj) { return !obj->isActive(); }),
        gameObjects.end()
    );
}

void Game::render() {
    // Clear display
    display.fillScreen(0x0000); // Black

    // Call user-defined rendering
    onRender();

    // Render all visible game objects
    for (auto& obj : gameObjects) {
        if (obj->isActive() && obj->isVisible()) {
            obj->render(display);
        }
    }

    // No explicit flush available on Display wrapper; assume draw calls take effect immediately
}

void Game::checkCollisions() {
    for (size_t i = 0; i < gameObjects.size(); i++) {
        for (size_t j = i + 1; j < gameObjects.size(); j++) {
            if (gameObjects[i]->isActive() && gameObjects[j]->isActive()) {
                if (gameObjects[i]->collidesWith(*gameObjects[j])) {
                    onCollision(*gameObjects[i], *gameObjects[j]);
                }
            }
        }
    }
}

void Game::renderSprite(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    display.fillRect(x, y, width, height, color);
}

void Game::renderSpriteOutline(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    display.drawRect(x, y, width, height, color);
}

void Game::renderCircle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color) {
    // Midpoint circle algorithm (outline)
    int16_t xc = (int16_t)x;
    int16_t yc = (int16_t)y;
    int16_t r = (int16_t)radius;
    int16_t dx = 0;
    int16_t dy = r;
    int16_t d = 1 - r;

    auto plot8 = [&](int16_t px, int16_t py) {
        display.drawPixel((uint16_t)px, (uint16_t)py, color);
    };

    while (dx <= dy) {
        plot8(xc + dx, yc + dy);
        plot8(xc - dx, yc + dy);
        plot8(xc + dx, yc - dy);
        plot8(xc - dx, yc - dy);
        plot8(xc + dy, yc + dx);
        plot8(xc - dy, yc + dx);
        plot8(xc + dy, yc - dx);
        plot8(xc - dy, yc - dx);

        dx++;
        if (d < 0) {
            d += 2 * dx + 1;
        } else {
            dy--;
            d += 2 * (dx - dy) + 1;
        }
    }
}

void Game::renderFilledCircle(uint16_t centerX, uint16_t centerY, uint16_t radius, uint16_t color) {
    for (int16_t y = -radius; y <= radius; y++) {
        for (int16_t x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                display.drawPixel(centerX + x, centerY + y, color);
            }
        }
    }
}

void Game::renderLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    display.drawLine(x1, y1, x2, y2, color);
}

void Game::renderBitmap(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                        const uint16_t* pixelData) {
    for (uint16_t row = 0; row < height; row++) {
        for (uint16_t col = 0; col < width; col++) {
            uint16_t color = pixelData[row * width + col];
            display.drawPixel(x + col, y + row, color);
        }
    }
}

void Game::renderBitmapTransparent(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                                   const uint16_t* pixelData, uint16_t transparentColor) {
    for (uint16_t row = 0; row < height; row++) {
        for (uint16_t col = 0; col < width; col++) {
            uint16_t color = pixelData[row * width + col];
            if (color != transparentColor) {
                display.drawPixel(x + col, y + row, color);
            }
        }
    }
}

void Game::addGameObject(std::unique_ptr<GameObject> obj) {
    gameObjects.push_back(std::move(obj));
}

GameObject* Game::findGameObject(uint32_t id) {
    for (auto& obj : gameObjects) {
        if (obj->getId() == id) {
            return obj.get();
        }
    }
    return nullptr;
}

size_t Game::getGameObjectCount() const {
    return gameObjects.size();
}

GameObject* Game::getGameObjectAt(size_t index) {
    if (index < gameObjects.size()) {
        return gameObjects[index].get();
    }
    return nullptr;
}

void Game::removeGameObject(size_t index) {
    if (index < gameObjects.size()) {
        gameObjects.erase(gameObjects.begin() + index);
    }
}

void Game::clearGameObjects() {
    gameObjects.clear();
}

Display& Game::getDisplay() {
    return display;
}
