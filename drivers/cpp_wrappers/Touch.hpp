#pragma once

#include <cstdint>
extern "C" {
#include "../c_drivers/touch/xpt2046.h"
}

class Touch {
public:
    Touch() = default;

    // Initialize TS hardware (sets SPI pins, CS, IRQ)
    void init() {
        ts_spi_setup();
    }

    bool isTouched() {
        return ts_is_touched();
    }

    uint16_t getX() {
        return ts_get_x();
    }

    uint16_t getY() {
        return ts_get_y();
    }
};
