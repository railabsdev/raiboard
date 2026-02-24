#pragma once

#include <cstdint>
#include "GameObject.hpp"
#include <cstdlib>

class Asteroid : public GameObject {
public:
    float radius;
    float fallSpeed;

    Asteroid(const Vector2& pos, float r, float speed)
        : GameObject(pos, BoxCollider(r*2.0f, r*2.0f), 0xFFFF), radius(r), fallSpeed(speed) {}

    void update(float deltaTime) override {
        // Move down by fallSpeed
        position.y += fallSpeed * deltaTime;
        // Update collider position handled by base GameObject
    }

    void render(Display& display) override {
        // Draw filled circle (simple pixel plot)
        int16_t cx = (int16_t)position.x + (int16_t)radius;
        int16_t cy = (int16_t)position.y + (int16_t)radius;
        int16_t r = (int16_t)radius;
        for (int16_t y = -r; y <= r; y++) {
            for (int16_t x = -r; x <= r; x++) {
                if (x*x + y*y <= r*r) {
                    int16_t px = cx + x;
                    int16_t py = cy + y;
                    if (px >= 0 && py >= 0 && px < display.getWidth() && py < display.getHeight()) {
                        display.drawPixel((uint16_t)px, (uint16_t)py, color);
                    }
                }
            }
        }
    }
    Type getType() const override { return Type::Asteroid; }
};
