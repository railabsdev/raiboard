#include <stdio.h>
#include <vector>
#include "pico/stdlib.h"
#include "screen.hpp"
#include "sprite.hpp"
#include "assets.hpp"
#include "audio_channel.hpp"

#define BUZZER_PIN 14
#define MAX_COINS 5
#define MAX_ASTEROIDS 10
#define COIN_SPAWN_INTERVAL_MS 400
#define ASTEROID_SPAWN_INTERVAL_MS 50

struct FallingObject {
    Sprite* sprite;
    float velocity;
    bool active;
};

class Game {
private:
    Screen screen;
    AudioChannel audio;
    Sprite player;
    std::vector<FallingObject> silverCoins;
    std::vector<FallingObject> asteroids;

    static constexpr int SILVER_COIN_MOVE_SPEED = 0;
    static constexpr int ASTEROID_MOVE_SPEED = 10;
    static constexpr int PLAYER_MOVE_SPEED = 5;
    static constexpr float GRAVITY = 0.3f;
    
    uint32_t frame_count = 0;
    uint32_t silver_coin_last_spawn_time = 0;
    uint32_t asteroid_last_spawn_time = 0;
    int old_player_x = 0;
    int old_player_y = 0;
    
public:
    Game() : audio(BUZZER_PIN), player(PLAYER_SPRITE, PLAYER_WIDTH, PLAYER_HEIGHT, 0x0000) {
        silverCoins.resize(MAX_COINS);
        asteroids.resize(MAX_ASTEROIDS);
    }
    
    ~Game() {
        cleanup();
    }
    
    void init() {
        stdio_init_all();
        sleep_ms(1000);
        
        audio.init();
        ILI9341_TFT &display = screen.display();
        display.fillScreen(display.C_BLACK);
    }
    
    void start() {
        player.setPosition(120, 280);
        old_player_x = player.getX();
        old_player_y = player.getY();
        
        for (int i = 0; i < MAX_COINS; i++) {
            silverCoins[i].sprite = new Sprite(SILVER_COIN_SPRITE, SILVER_COIN_WIDTH, SILVER_COIN_HEIGHT, 0x0000);
            silverCoins[i].active = false;
            silverCoins[i].velocity = 0;
        }

        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            asteroids[i].sprite = new Sprite(ASTEROID_SPRITE, ASTEROID_WIDTH, ASTEROID_HEIGHT, 0x0000);
            asteroids[i].active = false;
            asteroids[i].velocity = 0;
        }

        frame_count = 0;
        silver_coin_last_spawn_time = 0;
        asteroid_last_spawn_time = 0;
    }
    
    void update() {
        frame_count++;
        handleInput();
        handleCollisions();
        spawnCoins();
        spawnAsteroids();
    }
    
    void render() {
        ILI9341_TFT &display = screen.display();
        player.draw(display);
    }
    
private:
    void handleInput() {
        ILI9341_TFT &display = screen.display();
        uint16_t touch_x, touch_y;
        
        if (screen.readTouch(touch_x, touch_y)) {
            display.fillRect(old_player_x, old_player_y, PLAYER_WIDTH, PLAYER_HEIGHT, display.C_BLACK);
            
            int sprite_x = player.getX();
            int sprite_y = player.getY();
            
            if (touch_x < sprite_x) sprite_x -= PLAYER_MOVE_SPEED;
            if (touch_x > sprite_x) sprite_x += PLAYER_MOVE_SPEED;
            if (touch_y < sprite_y) sprite_y -= PLAYER_MOVE_SPEED;
            if (touch_y > sprite_y) sprite_y += PLAYER_MOVE_SPEED;
            
            player.setPosition(sprite_x, sprite_y);
            old_player_x = sprite_x;
            old_player_y = sprite_y;
        }
    }
    
    void handleCollisions() {
        ILI9341_TFT &display = screen.display();
        
        for (int i = 0; i < MAX_COINS; i++) {
            if (silverCoins[i].active) {
                int old_coin_y = silverCoins[i].sprite->getY();
                
                silverCoins[i].velocity += GRAVITY;
                int new_y = old_coin_y + (int)silverCoins[i].velocity + SILVER_COIN_MOVE_SPEED;
                
                display.fillRect(silverCoins[i].sprite->getX(), old_coin_y, SILVER_COIN_WIDTH, SILVER_COIN_HEIGHT, display.C_BLACK);
                
                if (new_y > 320) {
                    silverCoins[i].active = false;
                    continue;
                }
                
                silverCoins[i].sprite->setPosition(silverCoins[i].sprite->getX(), new_y);
                
                if (player.collidesWith(*silverCoins[i].sprite)) {
                    silverCoins[i].active = false;
                    display.fillRect(silverCoins[i].sprite->getX(), silverCoins[i].sprite->getY(), SILVER_COIN_WIDTH, SILVER_COIN_HEIGHT, display.C_BLACK);
                    audio.playCoin();
                } else {
                    silverCoins[i].sprite->draw(display);
                }
            }
        }

        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (asteroids[i].active) {
                int old_asteroid_y = asteroids[i].sprite->getY();
                
                asteroids[i].velocity += GRAVITY;
                int new_y = old_asteroid_y + (int)asteroids[i].velocity + ASTEROID_MOVE_SPEED;
                
                display.fillRect(asteroids[i].sprite->getX(), old_asteroid_y, ASTEROID_WIDTH, ASTEROID_HEIGHT, display.C_BLACK);
                
                if (new_y > 320) {
                    asteroids[i].active = false;
                    continue;
                }
                
                asteroids[i].sprite->setPosition(asteroids[i].sprite->getX(), new_y);
                
                if (player.collidesWith(*asteroids[i].sprite)) {
                    asteroids[i].active = false;
                    display.fillRect(asteroids[i].sprite->getX(), asteroids[i].sprite->getY(), ASTEROID_WIDTH, ASTEROID_HEIGHT, display.C_BLACK);
                    audio.playHit();
                } else {
                    asteroids[i].sprite->draw(display);
                }
            }
        }
    }
    
    void spawnCoins() {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        if (current_time - silver_coin_last_spawn_time > COIN_SPAWN_INTERVAL_MS) {
            for (int i = 0; i < MAX_COINS; i++) {
                if (!silverCoins[i].active) {
                    int random_x = (frame_count * 37 + i * 17) % (240 - SILVER_COIN_WIDTH);
                    silverCoins[i].sprite->setPosition(random_x, 0);
                    silverCoins[i].velocity = 1.0f;
                    silverCoins[i].active = true;
                    silver_coin_last_spawn_time = current_time;
                    break;
                }
            }
        }
    }

    void spawnAsteroids() {
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        
        if (current_time - asteroid_last_spawn_time > ASTEROID_SPAWN_INTERVAL_MS) {
            for (int i = 0; i < MAX_ASTEROIDS; i++) {
                if (!asteroids[i].active) {
                    int random_x = (frame_count * 43 + i * 23) % (240 - ASTEROID_WIDTH);
                    asteroids[i].sprite->setPosition(random_x, 0);
                    asteroids[i].velocity = 1.0f;
                    asteroids[i].active = true;
                    asteroid_last_spawn_time = current_time;
                    break;
                }
            }
        }
    }

    void cleanup() {
        for (int i = 0; i < MAX_COINS; i++) {
            delete silverCoins[i].sprite;
        }
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            delete asteroids[i].sprite;
        }
    }
};