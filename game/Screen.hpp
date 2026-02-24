#pragma once

#include <cstdint>
#include "displaylib_16/ili9341.hpp"

extern "C" {
#include "xpt2046.h"
}

class Screen {
public:
    Screen();
    ILI9341_TFT& display();
    bool isTouchPressed() const;
    bool readTouch(uint16_t& x, uint16_t& y);
    void clear(uint16_t color = 0x0000);

private:
    ILI9341_TFT display_;
};
