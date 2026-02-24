#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "xpt2046.h"

void ts_spi_setup(void) {
    gpio_set_function(TS_MO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(TS_MI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(TS_SCL_PIN, GPIO_FUNC_SPI);
    
    gpio_init(TS_CS_PIN);
    gpio_set_dir(TS_CS_PIN, GPIO_OUT);
    gpio_put(TS_CS_PIN, 1); 

    gpio_init(TS_IRQ_PIN);
    gpio_set_dir(TS_IRQ_PIN, GPIO_IN);
}



uint16_t ts_get_data16(uint8_t command) {
    uint32_t saved_baudrate = spi_get_baudrate(TS_SPI);
    
    // Ensure we have a valid baudrate to restore (default to 25MHz if not)
    if (saved_baudrate < 1000000 || saved_baudrate > 50000000) {
        saved_baudrate = 25000000;
    }
    
    spi_set_baudrate(TS_SPI, 2000000);  // 2MHz for touch
    sleep_us(10);  // Let SPI bus settle
    
    gpio_put(TS_CS_PIN, 0);
    sleep_us(2);
    
    spi_write_blocking(TS_SPI, &command, 1);
    sleep_us(1);
    
    uint8_t b1, b2;
    spi_read_blocking(TS_SPI, 0x00, &b1, 1);
    spi_read_blocking(TS_SPI, 0x00, &b2, 1);
    
    sleep_us(1);
    gpio_put(TS_CS_PIN, 1);
    
    // Restore display speed
    spi_set_baudrate(TS_SPI, saved_baudrate);
    sleep_us(10);  // Let SPI bus settle after speed change
    
    uint16_t result = ((uint16_t)b1 << 8) | b2;
    return result >> 3;  // XPT2046 returns 12-bit value in upper bits
}


uint16_t ts_get_x_raw(void) {
    int16_t res = 0;
    for (uint8_t i = 0; i < TS_EVAL_COUNT; i++) {
        res += ts_get_data16(TS_COMM_X_DPOS);
    }
    return res / TS_EVAL_COUNT;
}

uint16_t ts_get_y_raw(void) {
    int16_t res = 0;
    for (uint8_t i = 0; i < TS_EVAL_COUNT; i++) {
        res += ts_get_data16(TS_COMM_Y_DPOS);
    }
    return res / TS_EVAL_COUNT;
}

uint16_t ts_get_x(void) {

    uint16_t res = ts_get_x_raw();

    if (res >= TS_X_MAX_EDGE)
        return TS_X_SCREEN_MAX;
    if (res <= TS_X_MIN_EDGE)
        return TS_X_SCREEN_MIN;

    res = (TS_X_SCREEN_MAX * (res - TS_X_MIN_EDGE)) / (TS_X_MAX_EDGE - TS_X_MIN_EDGE);
    return TS_X_SCREEN_MAX - res;

}

uint16_t ts_get_y(void) {

    uint16_t res = ts_get_y_raw();

    if (res >= TS_Y_MAX_EDGE)
        return TS_Y_SCREEN_MAX;
    if (res <= TS_Y_MIN_EDGE)
        return TS_Y_SCREEN_MIN;

    res = (TS_Y_SCREEN_MAX * (res - TS_Y_MIN_EDGE)) / (TS_Y_MAX_EDGE - TS_Y_MIN_EDGE);
    return TS_Y_SCREEN_MAX - res;
}

uint16_t ts_get_z1_raw(void) {
    uint16_t res = 0;
    for (uint8_t i = 0; i < TS_EVAL_COUNT; i++) {
        res += ts_get_data16(TS_COMM_Z1_POS);
    }
    return res / TS_EVAL_COUNT;
}

uint16_t ts_get_z2_raw(void) {
    uint16_t res = 0;
    for (uint8_t i = 0; i < TS_EVAL_COUNT; i++) {
        res += ts_get_data16(TS_COMM_Z2_POS);
    }
    return res / TS_EVAL_COUNT;
}
