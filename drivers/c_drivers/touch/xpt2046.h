#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Default pin and SPI configuration (override in board config if needed)
#ifndef TS_SPI
#define TS_SPI spi0
#endif

#ifndef TS_MO_PIN
#define TS_MO_PIN 19
#endif
#ifndef TS_MI_PIN
#define TS_MI_PIN 16
#endif
#ifndef TS_SCL_PIN
#define TS_SCL_PIN 18
#endif
#ifndef TS_CS_PIN
#define TS_CS_PIN 17
#endif
#ifndef TS_IRQ_PIN
#define TS_IRQ_PIN 20
#endif

// Calibration / mapping defaults (adjust for your panel)
#ifndef TS_EVAL_COUNT
#define TS_EVAL_COUNT 3
#endif
#ifndef TS_X_MIN_EDGE
#define TS_X_MIN_EDGE 200
#endif
#ifndef TS_X_MAX_EDGE
#define TS_X_MAX_EDGE 3900
#endif
#ifndef TS_Y_MIN_EDGE
#define TS_Y_MIN_EDGE 200
#endif
#ifndef TS_Y_MAX_EDGE
#define TS_Y_MAX_EDGE 3900
#endif

#ifndef TS_X_SCREEN_MIN
#define TS_X_SCREEN_MIN 0
#endif
#ifndef TS_X_SCREEN_MAX
#define TS_X_SCREEN_MAX 239
#endif
#ifndef TS_Y_SCREEN_MIN
#define TS_Y_SCREEN_MIN 0
#endif
#ifndef TS_Y_SCREEN_MAX
#define TS_Y_SCREEN_MAX 319
#endif

// XPT2046 command bytes
#define TS_COMM_Y_DPOS 0x90
#define TS_COMM_X_DPOS 0xD0
#define TS_COMM_Z1_POS 0xB0
#define TS_COMM_Z2_POS 0xC0

// Initialization
void ts_spi_setup(void);

// Low-level read
uint16_t ts_get_data16(uint8_t command);

// Raw samples
uint16_t ts_get_x_raw(void);
uint16_t ts_get_y_raw(void);
uint16_t ts_get_z1_raw(void);
uint16_t ts_get_z2_raw(void);

// Calibrated screen coords
uint16_t ts_get_x(void);
uint16_t ts_get_y(void);

// Simple touch detection (based on pressure/ztouch)
bool ts_is_touched(void);

#ifdef __cplusplus
}
#endif
