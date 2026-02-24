#pragma once

#include <cstdint>

#include "displaylib_16/ili9341.hpp"

class Screen {
public:
    Screen();
    ILI9341_TFT &display();
    bool isTouchPressed() const;
    bool readTouch(uint16_t &x, uint16_t &y);

private:
    ILI9341_TFT display_;
};
