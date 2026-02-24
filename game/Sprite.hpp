#pragma once

#include <cstdint>
#include "Vector.hpp"
#include "GameObject.hpp"

class Sprite : public GameObject {
protected:
    const uint8_t* bitmapData;
    uint16_t width;
    uint16_t height;
    uint16_t transparentColor;

public:
    Sprite(const uint8_t* bitmap, uint16_t w, uint16_t h, 
           const Vector2& pos = Vector2(0.0f, 0.0f), 
           uint16_t transColor = 0x0000);
    
    virtual ~Sprite() = default;

    void render(ILI9341_TFT& display) override;
    void renderTransparent(ILI9341_TFT& display);
    
    uint16_t getWidth() const { return width; }
    uint16_t getHeight() const { return height; }
    void setBitmap(const uint8_t* bitmap) { bitmapData = bitmap; }
};
