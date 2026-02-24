#include "screen.hpp"
#include "pico/stdlib.h"

Screen::Screen() {
    // Use same pin mapping as game1 hardware
    // RST=20, DC=21, CS=17, SCLK=18, MOSI=19, MISO=16
    display_.init(20, 21, 17, 18, 19, 16, spi0, 25000000);
    touch_.init();
    // Diagnostic: clear to white then draw a red rectangle and text to verify output/backlight
    display_.fillScreen(Display::WHITE);
    display_.fillRect(10, 10, 100, 50, Display::RED);
    display_.setCursor(12, 12);
    display_.setTextColor(Display::BLACK, Display::WHITE);
    display_.writeString("RAIBOARD");
}

Display& Screen::display() {
    return display_;
}

bool Screen::isTouchPressed() const {
    // Use IRQ pin directly if desired; fallback to touch isTouched
    return touch_.isTouched();
}

bool Screen::readTouch(uint16_t &x, uint16_t &y) {
    if (!touch_.isTouched()) return false;
    x = touch_.getX();
    y = touch_.getY();
    if (x >= 240 || y >= 320) return false;
    return true;
}
