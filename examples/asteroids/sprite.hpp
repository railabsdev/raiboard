#pragma once

#include <cstdint>
#include "vector2d.hpp"
#include "collider.hpp"

class Sprite {
public:
    Sprite(const uint8_t* bitmap, int width, int height, uint16_t transparentColor = 0x0000);
    ~Sprite();
    
    void setPosition(int x, int y);
    void move(int dx, int dy);
    void draw(class Display& display);
    void drawScaled(class Display& display, float scale);
    
    int getX() const;
    int getY() const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    bool collidesWith(const Sprite& other) const;
    SquareCollider& getCollider() { return *collider; }
    
private:
    const uint8_t* bitmap_data;
    int width;
    int height;
    Vector2D<int>* position;
    SquareCollider* collider;
    uint16_t transparent_color;
};

inline int Sprite::getX() const { return position->getX(); }
inline int Sprite::getY() const { return position->getY(); }
