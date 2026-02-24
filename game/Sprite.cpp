#include "Sprite.hpp"

Sprite::Sprite(const uint8_t* bitmap, uint16_t w, uint16_t h, 
               const Vector2& pos, uint16_t transColor)
    : GameObject(pos, BoxCollider((float)w, (float)h)), 
      bitmapData(bitmap), width(w), height(h), transparentColor(transColor) {
}

void Sprite::render(ILI9341_TFT& display) {
    if (!visible) return;
    
    uint16_t x = (uint16_t)position.x;
    uint16_t y = (uint16_t)position.y;
    
    for (uint16_t py = 0; py < height; py++) {
        for (uint16_t px = 0; px < width; px++) {
            int index = (py * width + px) * 2;
            uint16_t color = (bitmapData[index] << 8) | bitmapData[index + 1];
            
            if (color != transparentColor) {
                display.drawPixel(x + px, y + py, color);
            }
        }
    }
}

void Sprite::renderTransparent(ILI9341_TFT& display) {
    render(display);
}
