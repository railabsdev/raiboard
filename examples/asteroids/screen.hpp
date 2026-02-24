#pragma once

#include <cstdint>
#include "Display.hpp"
#include "Touch.hpp"

class Screen {
public:
    Screen();
    Display& display();
    bool isTouchPressed() const;
    bool readTouch(uint16_t &x, uint16_t &y);

private:
    Display display_;
    Touch touch_;
};
