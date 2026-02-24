#pragma once

#include "GameObject.hpp"
#include "../../drivers/cpp_wrappers/Touch.hpp"

class Player : public GameObject {
public:
    float speed;
    Touch* touch;

    Player(const Vector2& pos = Vector2(120.0f, 240.0f), float spd = 120.0f, Touch* t = nullptr)
        : GameObject(pos, BoxCollider(16.0f, 16.0f), 0xF800), speed(spd), touch(t) {}

    void update(float deltaTime) override {
        if (touch && touch->isTouched()) {
            Vector2 target((float)touch->getX(), (float)touch->getY());
            Vector2 dir = (target - position).normalized();
            velocity = dir * speed;
        } else {
            // No touch: stop
            velocity = Vector2::zero();
        }
        GameObject::update(deltaTime);
    }

    void render(Display& display) override {
        // Render player as filled circle
        int16_t cx = (int16_t)position.x + 8;
        int16_t cy = (int16_t)position.y + 8;
        int16_t r = 8;
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
};
