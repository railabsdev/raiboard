/**
 * @file Display.cpp
 * @brief C++ wrapper implementation for display driver
 */

#include "Display.hpp"
#include <cstring>

Display::Display() {
    std::memset(&handle_, 0, sizeof(handle_));
}

Display::~Display() {
    deinit();
}

bool Display::init(int8_t rst, int8_t dc, int8_t cs, int8_t sclk, int8_t mosi, int8_t miso,
                   void* spi_inst, uint32_t spi_speed_hz) {
    return display_init(&handle_, rst, dc, cs, sclk, mosi, miso, spi_inst, spi_speed_hz);
}

void Display::deinit() {
    display_deinit(&handle_);
}

void Display::fillScreen(uint16_t color) {
    display_fill_screen(&handle_, color);
}

void Display::fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    display_fill_rect(&handle_, x, y, width, height, color);
}

void Display::drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    display_draw_rect(&handle_, x, y, width, height, color);
}

void Display::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    display_draw_pixel(&handle_, x, y, color);
}

void Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    display_draw_line(&handle_, x0, y0, x1, y1, color);
}

void Display::setCursor(uint16_t x, uint16_t y) {
    display_set_cursor(&handle_, x, y);
}

void Display::setTextColor(uint16_t text_color, uint16_t bg_color) {
    display_set_text_color(&handle_, text_color, bg_color);
}

void Display::setTextWrap(bool wrap) {
    display_set_text_wrap(&handle_, wrap);
}

void Display::writeChar(char c) {
    display_write_char(&handle_, c);
}

void Display::writeString(const char* str) {
    display_write_string(&handle_, str);
}

void Display::setRotation(display_rotation_t rotation) {
    display_set_rotation(&handle_, rotation);
}

void Display::invert(bool invert) {
    display_invert(&handle_, invert);
}

void Display::setEnabled(bool enable) {
    display_set_enabled(&handle_, enable);
}

uint16_t Display::getWidth() const {
    return display_get_width(&handle_);
}

uint16_t Display::getHeight() const {
    return display_get_height(&handle_);
}

uint16_t Display::getCursorX() const {
    return display_get_cursor_x(&handle_);
}

uint16_t Display::getCursorY() const {
    return display_get_cursor_y(&handle_);
}

bool Display::isInitialized() const {
    return handle_.initialized;
}
