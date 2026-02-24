# Touch Screen Issue - XPT2046 Controller

## Problem Description

The XPT2046 touch controller is returning stuck/cached values instead of actual touch coordinates.

### Symptoms

1. **Same RAW values regardless of touch location**: Touching different areas of the screen returns identical RAW X/Y values (e.g., 1349, 1349)
2. **Values don't update**: Dragging finger across screen doesn't produce changing coordinates
3. **Initial touch works, then freezes**: First touch registers, but subsequent touches return same cached values
4. **Issue persists across driver implementations**: Both original and SPI-improved drivers show identical behavior

### Hardware Setup

- **Display**: ILI9341 (240x320, SPI)
- **Touch Controller**: XPT2046 (shared SPI bus with display)
- **Platform**: Raspberry Pi Pico (RP2040)
- **SPI Configuration**: 
  - Display: 25MHz
  - Touch: 2MHz (switched dynamically)
- **Pin Configuration**:
  - SPI Bus: spi0
  - Touch CS: GP22
  - Touch IRQ: GP15

### Tested Solutions (All Failed)

1. ✗ **SPI Speed Management**: Added baudrate validation and timing delays
2. ✗ **Display Initialization**: Ensured display properly initializes SPI before touch reads
3. ✗ **Multiple Reads with Averaging**: Same cached value returned
4. ✗ **Different Sampling Rates**: No improvement with faster/slower polling
5. ✗ **Driver Comparison**: Both old and new driver versions exhibit identical behavior

### Code Implementations Tested

#### Current Driver Setup (examples/game1/xpt2046.c)
```c
uint16_t ts_get_data16(uint8_t command) {
    uint32_t saved_baudrate = spi_get_baudrate(TS_SPI);
    
    // Validate and set touch speed
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
    
    spi_set_baudrate(TS_SPI, saved_baudrate);
    sleep_us(10);
    
    uint16_t result = ((uint16_t)b1 << 8) | b2;
    return result >> 3;
}
```

#### Calibration Tool Output (Typical)
```
Sample #1:
  RAW    -> X: 1349  Y: 1349
  MAPPED -> X: 164  Y: 229

Sample #2:
  RAW    -> X: 1349  Y: 1349
  MAPPED -> X: 164  Y: 229

Sample #3:
  RAW    -> X: 1349  Y: 1349
  MAPPED -> X: 164  Y: 229
```

### Hardware Questions

1. Is the XPT2046 defective or misconfigured?
2. Could there be a hardware conflict on the shared SPI bus?
3. Are additional pull-up/pull-down resistors needed?
4. Is the IRQ pin functioning correctly?

### Workaround Implemented

Switched to **analog joystick input** (VRX/VRY on ADC pins, SW on GPIO) as a temporary solution.

### Files for Reference

- Touch Driver: `/game/xpt2046.c`, `/game/xpt2046.h`
- Calibration Tool: `/examples/game1/touch_calibration.cpp`
- Screen Wrapper: `/game/Screen.cpp`, `/game/Screen.hpp`

### Next Steps

1. Test with different XPT2046 module (hardware issue?)
2. Try bit-banging SPI instead of hardware SPI
3. Verify power supply stability during touch reads
4. Check for electrical interference on SPI lines
5. Test with oscilloscope to verify SPI communications

---

**Status**: Unresolved - Using joystick as alternative input method
