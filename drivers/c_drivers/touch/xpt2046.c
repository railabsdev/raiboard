#include "xpt2046.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <stdlib.h>

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
    spi_set_baudrate(TS_SPI, 2000000);  // 2MHz for touch

    gpio_put(TS_CS_PIN, 0);
    sleep_us(1);

    uint8_t cmd = command;
    spi_write_blocking(TS_SPI, &cmd, 1);

    uint8_t b1 = 0, b2 = 0;
    spi_read_blocking(TS_SPI, 0x00, &b1, 1);
    spi_read_blocking(TS_SPI, 0x00, &b2, 1);

    gpio_put(TS_CS_PIN, 1);

    spi_set_baudrate(TS_SPI, saved_baudrate);

    uint16_t result = ((uint16_t)b1 << 8) | b2;
    return result >> 3;  // XPT2046 returns 12-bit value in upper bits
}

uint16_t ts_get_x_raw(void) {
    int32_t res = 0;
    for (uint8_t i = 0; i < TS_EVAL_COUNT; i++) {
        res += ts_get_data16(TS_COMM_X_DPOS);
    }
    return (uint16_t)(res / TS_EVAL_COUNT);
}

uint16_t ts_get_y_raw(void) {
    int32_t res = 0;
    for (uint8_t i = 0; i < TS_EVAL_COUNT; i++) {
        res += ts_get_data16(TS_COMM_Y_DPOS);
    }
    return (uint16_t)(res / TS_EVAL_COUNT);
}

uint16_t ts_get_z1_raw(void) {
    int32_t res = 0;
    for (uint8_t i = 0; i < TS_EVAL_COUNT; i++) {
        res += ts_get_data16(TS_COMM_Z1_POS);
    }
    return (uint16_t)(res / TS_EVAL_COUNT);
}

uint16_t ts_get_z2_raw(void) {
    int32_t res = 0;
    for (uint8_t i = 0; i < TS_EVAL_COUNT; i++) {
        res += ts_get_data16(TS_COMM_Z2_POS);
    }
    return (uint16_t)(res / TS_EVAL_COUNT);
}

uint16_t ts_get_x(void) {
    uint16_t res = ts_get_x_raw();

    if (res >= TS_X_MAX_EDGE)
        return TS_X_SCREEN_MAX;
    if (res <= TS_X_MIN_EDGE)
        return TS_X_SCREEN_MIN;

    uint32_t mapped = (uint32_t)(TS_X_SCREEN_MAX * (res - TS_X_MIN_EDGE)) / (TS_X_MAX_EDGE - TS_X_MIN_EDGE);
    return (uint16_t)(TS_X_SCREEN_MAX - mapped);
}

uint16_t ts_get_y(void) {
    uint16_t res = ts_get_y_raw();

    if (res >= TS_Y_MAX_EDGE)
        return TS_Y_SCREEN_MIN;
    if (res <= TS_Y_MIN_EDGE)
        return TS_Y_SCREEN_MIN;

    uint32_t mapped = (uint32_t)(TS_Y_SCREEN_MAX * (res - TS_Y_MIN_EDGE)) / (TS_Y_MAX_EDGE - TS_Y_MIN_EDGE);
    return (uint16_t)(TS_Y_SCREEN_MAX - mapped);
}

bool ts_is_touched(void) {
    // Use pressure measurement (z1/z2) or IRQ pin if available
    uint16_t z1 = ts_get_z1_raw();
    uint16_t z2 = ts_get_z2_raw();
    // Simple heuristic: z1 should be > 0 and less than z2
    return (z1 > 50 && z2 > z1);
}
