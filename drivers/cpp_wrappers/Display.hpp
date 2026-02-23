/**
 * @file Display.hpp
 * @brief C++ wrapper for ILI9341 display driver
 * @author RaiLabs
 */

#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "ili9341_display.h"
#include <cstdint>
#include <cstddef>

class Display {
public:
    /**
     * @brief Constructor
     */
    Display();

    /**
     * @brief Destructor
     */
    ~Display();

    /**
     * @brief Initialize display
     * @param rst Reset pin
     * @param dc Data/Command pin
     * @param cs Chip Select pin
     * @param sclk Clock pin
     * @param mosi MOSI pin
     * @param miso MISO pin (-1 to skip)
     * @param spi_inst Hardware SPI instance (spi0 or spi1)
     * @param spi_speed_hz SPI clock speed (e.g., 25000000)
     * @return true if successful
     */
    bool init(int8_t rst, int8_t dc, int8_t cs, int8_t sclk, int8_t mosi, int8_t miso,
              void* spi_inst, uint32_t spi_speed_hz);

    /**
     * @brief Deinitialize display (turn off)
     */
    void deinit();

    /**
     * @brief Fill entire screen with color
     * @param color 16-bit RGB565 color
     */
    void fillScreen(uint16_t color);

    /**
     * @brief Fill rectangle
     * @param x Starting X
     * @param y Starting Y
     * @param width Width in pixels
     * @param height Height in pixels
     * @param color 16-bit RGB565 color
     */
    void fillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

    /**
     * @brief Draw rectangle outline (not filled)
     * @param x Starting X
     * @param y Starting Y
     * @param width Width in pixels
     * @param height Height in pixels
     * @param color 16-bit RGB565 color
     */
    void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

    /**
     * @brief Draw a single pixel
     * @param x X coordinate
     * @param y Y coordinate
     * @param color 16-bit RGB565 color
     */
    void drawPixel(uint16_t x, uint16_t y, uint16_t color);

    /**
     * @brief Draw a line
     * @param x0 Start X
     * @param y0 Start Y
     * @param x1 End X
     * @param y1 End Y
     * @param color 16-bit RGB565 color
     */
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

    /**
     * @brief Set text cursor position
     * @param x Cursor X
     * @param y Cursor Y
     */
    void setCursor(uint16_t x, uint16_t y);

    /**
     * @brief Set text color and background color
     * @param text_color Text color (16-bit RGB565)
     * @param bg_color Background color (16-bit RGB565)
     */
    void setTextColor(uint16_t text_color, uint16_t bg_color);

    /**
     * @brief Enable/disable text wrapping
     * @param wrap true to wrap text
     */
    void setTextWrap(bool wrap);

    /**
     * @brief Write a single character
     * @param c ASCII character to write
     */
    void writeChar(char c);

    /**
     * @brief Write a C string
     * @param str Null-terminated string
     */
    void writeString(const char* str);

    /**
     * @brief Set display rotation
     * @param rotation 0, 90, 180, or 270 degrees
     */
    void setRotation(display_rotation_t rotation);

    /**
     * @brief Invert display colors
     * @param invert true to invert
     */
    void invert(bool invert);

    /**
     * @brief Enable/disable display output
     * @param enable true to turn on
     */
    void setEnabled(bool enable);

    /**
     * @brief Get display width in pixels
     * @return Width in pixels
     */
    uint16_t getWidth() const;

    /**
     * @brief Get display height in pixels
     * @return Height in pixels
     */
    uint16_t getHeight() const;

    /**
     * @brief Get cursor X position
     * @return Cursor X coordinate
     */
    uint16_t getCursorX() const;

    /**
     * @brief Get cursor Y position
     * @return Cursor Y coordinate
     */
    uint16_t getCursorY() const;

    /**
     * @brief Check if display is initialized
     * @return true if initialized
     */
    bool isInitialized() const;

    /**
     * @brief Get const reference to C handle
     * @return Reference to underlying display_handle_t
     */
    const display_handle_t& getHandle() const { return handle_; }

    /**
     * @brief Get mutable reference to C handle
     * @return Reference to underlying display_handle_t
     */
    display_handle_t& getHandle() { return handle_; }

    /* Predefined colors as constants */
    static constexpr uint16_t BLACK   = 0x0000;
    static constexpr uint16_t WHITE   = 0xFFFF;
    static constexpr uint16_t RED     = 0xF800;
    static constexpr uint16_t GREEN   = 0x07E0;
    static constexpr uint16_t BLUE    = 0x001F;
    static constexpr uint16_t CYAN    = 0x07FF;
    static constexpr uint16_t MAGENTA = 0xF81F;
    static constexpr uint16_t YELLOW  = 0xFFE0;
    static constexpr uint16_t ORANGE  = 0xFC00;

private:
    display_handle_t handle_;
};

#endif /* DISPLAY_HPP */
