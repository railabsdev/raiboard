/**
 * @file ili9341_display.h
 * @brief Bare-bones C display driver for ILI9341
 * @author RaiLabs
 */

#ifndef ILI9341_DISPLAY_H
#define ILI9341_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*  Display Dimensions  */
#define DISPLAY_WIDTH       240
#define DISPLAY_HEIGHT      320
#define DISPLAY_BUFFER_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT * 2)

/*  16-bit RGB565 Color Definitions  */
typedef enum {
    COLOR_BLACK    = 0x0000,
    COLOR_WHITE    = 0xFFFF,
    COLOR_RED      = 0xF800,
    COLOR_GREEN    = 0x07E0,
    COLOR_BLUE     = 0x001F,
    COLOR_CYAN     = 0x07FF,
    COLOR_MAGENTA  = 0xF81F,
    COLOR_YELLOW   = 0xFFE0,
    COLOR_GRAY     = 0x8410,
    COLOR_ORANGE   = 0xFC00,
    COLOR_NAVY     = 0x000F,
    COLOR_PURPLE   = 0x780F
} display_color_t;

/*  Display Rotation  */
typedef enum {
    DISPLAY_ROTATION_0   = 0,
    DISPLAY_ROTATION_90  = 1,
    DISPLAY_ROTATION_180 = 2,
    DISPLAY_ROTATION_270 = 3
} display_rotation_t;

/*  Display Handle  */
typedef struct {
    int8_t pin_rst;
    int8_t pin_dc;
    int8_t pin_cs;
    int8_t pin_sclk;
    int8_t pin_mosi;
    int8_t pin_miso;

    /* SPI Configuration */
    void* spi_instance;      /* spi0 / spi1 */
    uint32_t spi_speed_hz;
    bool use_hardware_spi;

    /* Display State */
    bool initialized;
    uint16_t width;
    uint16_t height;
    display_rotation_t rotation;
    uint16_t cursor_x;
    uint16_t cursor_y;
    uint16_t text_color;
    uint16_t text_bg_color;
    bool text_wrap;
} display_handle_t;

/*  API  */

/**
 * @brief Initialize display with default single-color mode
 * @param display Pointer to display_handle_t
 * @param rst Reset pin
 * @param dc Data/Command pin
 * @param cs Chip Select pin
 * @param sclk Clock pin
 * @param mosi MOSI (data out) pin
 * @param miso MISO (data in) pin, -1 to skip
 * @param spi_instance Hardware SPI instance (spi0 or spi1)
 * @param spi_speed_hz SPI clock speed
 * @return true if initialization successful
 */
bool display_init(display_handle_t* display,
                  int8_t rst, int8_t dc, int8_t cs, 
                  int8_t sclk, int8_t mosi, int8_t miso,
                  void* spi_instance, uint32_t spi_speed_hz);

/**
 * @brief Deinitialize display
 * @param display Pointer to display_handle_t
 */
void display_deinit(display_handle_t* display);

/**
 * @brief Fill entire screen with a color
 * @param display Pointer to display_handle_t
 * @param color 16-bit RGB565 color
 */
void display_fill_screen(display_handle_t* display, uint16_t color);

/**
 * @brief Fill a rectangle
 * @param display Pointer to display_handle_t
 * @param x Starting X coordinate
 * @param y Starting Y coordinate
 * @param width Width in pixels
 * @param height Height in pixels
 * @param color 16-bit RGB565 color
 */
void display_fill_rect(display_handle_t* display, uint16_t x, uint16_t y, 
                       uint16_t width, uint16_t height, uint16_t color);

/**
 * @brief Draw outline of a rectangle (not filled)
 * @param display Pointer to display_handle_t
 * @param x Starting X coordinate
 * @param y Starting Y coordinate
 * @param width Width in pixels
 * @param height Height in pixels
 * @param color 16-bit RGB565 color
 */
void display_draw_rect(display_handle_t* display, uint16_t x, uint16_t y, 
                       uint16_t width, uint16_t height, uint16_t color);

/**
 * @brief Draw a single pixel
 * @param display Pointer to display_handle_t
 * @param x X coordinate
 * @param y Y coordinate
 * @param color 16-bit RGB565 color
 */
void display_draw_pixel(display_handle_t* display, uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief Draw a line between two points
 * @param display Pointer to display_handle_t
 * @param x0 Start X coordinate
 * @param y0 Start Y coordinate
 * @param x1 End X coordinate
 * @param y1 End Y coordinate
 * @param color 16-bit RGB565 color
 */
void display_draw_line(display_handle_t* display, int16_t x0, int16_t y0, 
                       int16_t x1, int16_t y1, uint16_t color);

/**
 * @brief Set text cursor position
 * @param display Pointer to display_handle_t
 * @param x Cursor X coordinate
 * @param y Cursor Y coordinate
 */
void display_set_cursor(display_handle_t* display, uint16_t x, uint16_t y);

/**
 * @brief Set text color (foreground and background)
 * @param display Pointer to display_handle_t
 * @param text_color Text color (16-bit RGB565)
 * @param bg_color Background color (16-bit RGB565)
 */
void display_set_text_color(display_handle_t* display, uint16_t text_color, uint16_t bg_color);

/**
 * @brief Enable/disable text wrapping
 * @param display Pointer to display_handle_t
 * @param wrap true to enable wrapping
 */
void display_set_text_wrap(display_handle_t* display, bool wrap);

/**
 * @brief Write a single character
 * @param display Pointer to display_handle_t
 * @param c ASCII character
 */
void display_write_char(display_handle_t* display, char c);

/**
 * @brief Write a string
 * @param display Pointer to display_handle_t
 * @param str Null-terminated string
 */
void display_write_string(display_handle_t* display, const char* str);

/**
 * @brief Set display rotation
 * @param display Pointer to display_handle_t
 * @param rotation Rotation mode (0, 90, 180, 270 degrees)
 */
void display_set_rotation(display_handle_t* display, display_rotation_t rotation);

/**
 * @brief Invert display colors
 * @param display Pointer to display_handle_t
 * @param invert true to invert
 */
void display_invert(display_handle_t* display, bool invert);

/**
 * @brief Enable/disable display output
 * @param display Pointer to display_handle_t
 * @param enable true to turn on display
 */
void display_set_enabled(display_handle_t* display, bool enable);

/**
 * @brief Get display width
 * @param display Pointer to display_handle_t
 * @return Width in pixels
 */
uint16_t display_get_width(const display_handle_t* display);

/**
 * @brief Get display height
 * @param display Pointer to display_handle_t
 * @return Height in pixels
 */
uint16_t display_get_height(const display_handle_t* display);

/**
 * @brief Get current cursor X position
 * @param display Pointer to display_handle_t
 * @return Cursor X coordinate
 */
uint16_t display_get_cursor_x(const display_handle_t* display);

/**
 * @brief Get current cursor Y position
 * @param display Pointer to display_handle_t
 * @return Cursor Y coordinate
 */
uint16_t display_get_cursor_y(const display_handle_t* display);

#ifdef __cplusplus
}
#endif

#endif /* ILI9341_DISPLAY_H */