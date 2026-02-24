#include "screen.hpp"

#include "pico/stdlib.h"

extern "C" {
#include "xpt2046.h"
}

Screen::Screen() {
    display_.SetupGPIO(20, 21, 17, 18, 19, 16);
    display_.SetupScreenSize(240, 320);
    display_.SetupSPI(25000000, spi0);
    display_.ILI9341Initialize();
    
    ts_spi_setup();  // same bus
    
    display_.fillScreen(display_.C_BLACK);
}

ILI9341_TFT &Screen::display() {
    return display_;
}

bool Screen::isTouchPressed() const {
    return gpio_get(TS_IRQ_PIN) == 0;
}

bool Screen::readTouch(uint16_t &x, uint16_t &y) {
    if (gpio_get(TS_IRQ_PIN) != 0) {  // IRQ is active low
        return false;
    }

    x = ts_get_x();
    y = ts_get_y();

    if (x >= TS_X_SCREEN_MAX || y >= TS_Y_SCREEN_MAX) {
        return false;
    }

    return true;
}
