#include "sprite.hpp"
#include "Display.hpp"

Sprite::Sprite(const uint8_t* bitmap, int w, int h, uint16_t transparentColor)
    : bitmap_data(bitmap), width(w), height(h), transparent_color(transparentColor) {
    position = new Vector2D<int>(0, 0);
    collider = new SquareCollider(position, width, height);
}

Sprite::~Sprite() {
    delete collider;
    delete position;
}

void Sprite::setPosition(int x, int y) {
    position->setX(x);
    position->setY(y);
}

void Sprite::move(int dx, int dy) {
    position->setX(position->getX() + dx);
    position->setY(position->getY() + dy);
}

void Sprite::draw(Display& display) {
    int x_pos = position->getX();
    int y_pos = position->getY();
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 2;
            uint16_t color = (bitmap_data[index] << 8) | bitmap_data[index + 1];
            
            if (color != transparent_color) {
                display.drawPixel(x_pos + x, y_pos + y, color);
            }
        }
    }
}

void Sprite::drawScaled(Display& display, float scale) {
    int x_pos = position->getX();
    int y_pos = position->getY();
    int new_width = (int)(width * scale);
    int new_height = (int)(height * scale);
    
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            int src_x = (int)(x / scale);
            int src_y = (int)(y / scale);
            int index = (src_y * width + src_x) * 2;
            uint16_t color = (bitmap_data[index] << 8) | bitmap_data[index + 1];
            
            if (color != transparent_color) {
                display.drawPixel(x_pos + x, y_pos + y, color);
            }
        }
    }
}

bool Sprite::collidesWith(const Sprite& other) const {
    return collider->collidesWith(*other.collider);
}
