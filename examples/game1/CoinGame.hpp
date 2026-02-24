#include "Game.hpp"
#include "Sprite.hpp"
#include "Screen.hpp"
#include "Joystick.hpp"
#include "AudioChannel.hpp"
#include "assets.hpp"
#include "Vector.hpp"
#include <vector>
#include <cstdlib>

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

class CoinGame : public Game {
private:
    Joystick joystick;
    AudioChannel audio;
    Sprite player;
    std::vector<FallingObject> silverCoins;
    std::vector<FallingObject> asteroids;
    Screen& gameScreen;

    static constexpr float GRAVITY = 0.3f;
    static constexpr int SILVER_COIN_MOVE_SPEED = 0;
    static constexpr int ASTEROID_MOVE_SPEED = 10;
    
    uint32_t frame_count = 0;
    uint32_t silver_coin_last_spawn_time = 0;
    uint32_t asteroid_last_spawn_time = 0;
    Vector2 old_player_pos;
    
public:
    CoinGame(Screen& scr) 
        : Game(scr), 
          audio(BUZZER_PIN), 
          player(PLAYER_SPRITE, PLAYER_WIDTH, PLAYER_HEIGHT),
          gameScreen(scr)
    {
        silverCoins.resize(MAX_COINS);
        asteroids.resize(MAX_ASTEROIDS);
    }
    
    ~CoinGame() override {
        cleanup();
    }
    
    void onInit() override {
        audio.init();
        getScreen().display().fillScreen(getScreen().display().C_BLACK);
        
        // Initialize player position
        player.setPosition(Vector2(120, 280));
        old_player_pos = player.getPosition();
        
        for (int i = 0; i < MAX_COINS; i++) {
            silverCoins[i].sprite = new Sprite(SILVER_COIN_SPRITE, SILVER_COIN_WIDTH, SILVER_COIN_HEIGHT);
            silverCoins[i].active = false;
            silverCoins[i].velocity = 0;
        }

        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            asteroids[i].sprite = new Sprite(ASTEROID_SPRITE, ASTEROID_WIDTH, ASTEROID_HEIGHT);
            asteroids[i].active = false;
            asteroids[i].velocity = 0;
        }

        frame_count = 0;
        silver_coin_last_spawn_time = 0;
        asteroid_last_spawn_time = 0;
    }
    
    void onUpdate(float deltaTime) override {
        frame_count++;
        handleInput();
        handleCollisions();
        spawnCoins();
        spawnAsteroids();
    }
    
    void onRender() override {
        player.render(getScreen().display());
    }
    
    void onShutdown() override {
        cleanup();
    }
    
private:
    void handleInput() {
        ILI9341_TFT &display = getScreen().display();
        
        // Get joystick direction with sensitivity
        int16_t dx = 0, dy = 0;
        joystick.getDirection(dx, dy, 5.0f);  // 5 pixels per frame max
        
        if (dx != 0 || dy != 0) {
            Vector2 pos = player.getPosition();
            display.fillRect((int)pos.x, (int)pos.y, PLAYER_WIDTH, PLAYER_HEIGHT, display.C_BLACK);
            
            int sprite_x = (int)pos.x + dx;
            int sprite_y = (int)pos.y + dy;
            
            // Keep player on screen
            sprite_x = std::max(0, std::min(sprite_x, 240 - PLAYER_WIDTH));
            sprite_y = std::max(0, std::min(sprite_y, 320 - PLAYER_HEIGHT));
            
            player.setPosition(Vector2(sprite_x, sprite_y));
            old_player_pos = player.getPosition();
        }
    }
    
    void handleCollisions() {
        ILI9341_TFT &display = getScreen().display();
        
        // Update coins
        for (int i = 0; i < MAX_COINS; i++) {
            if (silverCoins[i].active) {
                Vector2 pos = silverCoins[i].sprite->getPosition();
                int coin_x = (int)pos.x;
                int coin_y = (int)pos.y;
                
                silverCoins[i].velocity += GRAVITY;
                int new_y = coin_y + (int)silverCoins[i].velocity + SILVER_COIN_MOVE_SPEED;
                
                display.fillRect(coin_x, coin_y, SILVER_COIN_WIDTH, SILVER_COIN_HEIGHT, display.C_BLACK);
                
                if (new_y > 320) {
                    silverCoins[i].active = false;
                    continue;
                }
                
                silverCoins[i].sprite->setPosition(Vector2(coin_x, new_y));
                
                if (player.collidesWith(*silverCoins[i].sprite)) {
                    silverCoins[i].active = false;
                    display.fillRect(coin_x, new_y, SILVER_COIN_WIDTH, SILVER_COIN_HEIGHT, display.C_BLACK);
                    audio.playCoin();
                } else {
                    silverCoins[i].sprite->render(display);
                }
            }
        }

        // Update asteroids
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (asteroids[i].active) {
                Vector2 pos = asteroids[i].sprite->getPosition();
                int ast_x = (int)pos.x;
                int ast_y = (int)pos.y;
                
                asteroids[i].velocity += GRAVITY;
                int new_y = ast_y + (int)asteroids[i].velocity + ASTEROID_MOVE_SPEED;
                
                display.fillRect(ast_x, ast_y, ASTEROID_WIDTH, ASTEROID_HEIGHT, display.C_BLACK);
                
                if (new_y > 320) {
                    asteroids[i].active = false;
                    continue;
                }
                
                asteroids[i].sprite->setPosition(Vector2(ast_x, new_y));
                
                if (player.collidesWith(*asteroids[i].sprite)) {
                    asteroids[i].active = false;
                    display.fillRect(ast_x, new_y, ASTEROID_WIDTH, ASTEROID_HEIGHT, display.C_BLACK);
                    audio.playHit();
                } else {
                    asteroids[i].sprite->render(display);
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
                    silverCoins[i].sprite->setPosition(Vector2(random_x, 0));
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
                    asteroids[i].sprite->setPosition(Vector2(random_x, 0));
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
            if (silverCoins[i].sprite) {
                delete silverCoins[i].sprite;
                silverCoins[i].sprite = nullptr;
            }
        }
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (asteroids[i].sprite) {
                delete asteroids[i].sprite;
                asteroids[i].sprite = nullptr;
            }
        }
    }
};
