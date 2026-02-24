#pragma once

#include "../../game/Game.hpp"
#include "Asteroid.hpp"
#include "Player.hpp"
#include <cstdlib>

class AsteroidsGame : public Game {
private:
    Player* player;
    Touch* touch;
    uint32_t spawnTimer;
    uint32_t spawnInterval;
    const size_t MAX_ASTEROIDS = 5;

public:
    AsteroidsGame(Display& disp, Touch* t)
        : Game(disp), player(nullptr), touch(t), spawnTimer(0), spawnInterval(60) {}

    void onInit() override {
        // Initialize player
        auto p = std::make_unique<Player>(Vector2(110.0f, 200.0f), 120.0f, touch);
        player = p.get();
        addGameObject(std::move(p));

        // Seed random
        srand( (unsigned) 12345 );
    }

    void onUpdate(float deltaTime) override {
        // Spawn logic
        spawnTimer++;
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0;
            // Count current asteroids
            size_t count = 0;
            for (size_t i = 0; i < getGameObjectCount(); ++i) {
                GameObject* obj = getGameObjectAt(i);
                if (obj && dynamic_cast<Asteroid*>(obj)) count++;
            }
            if (count < MAX_ASTEROIDS) {
                spawnAsteroid();
            }
        }

        // Remove off-screen asteroids
        for (size_t i = 0; i < getGameObjectCount(); ++i) {
            GameObject* obj = getGameObjectAt(i);
            if (obj) {
                Asteroid* a = dynamic_cast<Asteroid*>(obj);
                if (a) {
                    if (a->getPosition().y > (float)display.getHeight()) {
                        // deactivate
                        a->setActive(false);
                    }
                }
            }
        }

        // If player inactive, stop game
        if (!player || !player->isActive()) {
            stop();
        }
    }

    void onRender() override {
        // Draw score or UI if needed
    }

    void onCollision(GameObject& a, GameObject& b) override {
        // If player collides with asteroid -> game over
        Asteroid* ast = dynamic_cast<Asteroid*>(&a);
        Player* pl = dynamic_cast<Player*>(&a);
        if (!ast) ast = dynamic_cast<Asteroid*>(&b);
        if (!pl) pl = dynamic_cast<Player*>(&b);
        if (ast && pl) {
            pl->setActive(false);
        }
    }

    void spawnAsteroid() {
        int w = display.getWidth();
        float x = (float)(rand() % (w - 20));
        float r = 6.0f + (rand() % 12);
        float speed = 40.0f + (rand() % 80);
        Vector2 pos(x, -r*2.0f);
        auto ast = std::make_unique<Asteroid>(pos, r, speed);
        addGameObject(std::move(ast));
    }
};
