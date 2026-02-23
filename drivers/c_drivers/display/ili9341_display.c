/**
 * @file ili9341_display.c
 * @brief Bare-bones C display driver implementation for ILI9341
 */

#include "ili9341_display.h"
#include <string.h>

extern void ILI9341_SetupGPIO(int8_t rst, int8_t dc, int8_t cs, int8_t sclk, int8_t mosi, int8_t miso);
extern void ILI9341_SetupScreenSize(uint16_t w, uint16_t h);
extern void ILI9341_SetupSPI_HW(uint32_t speed_hz, void* spi_inst);
extern void ILI9341_Initialize(void);
extern void ILI9341_FillScreen(uint16_t color);
extern void ILI9341_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
extern void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
extern void ILI9341_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
extern void ILI9341_SetCursor(int16_t x, int16_t y);
extern void ILI9341_SetTextColor(uint16_t c, uint16_t bg);
extern void ILI9341_WriteChar(char c);
extern void ILI9341_WriteString(const char* str);
extern void ILI9341_SetRotation(uint8_t rotation);
extern void ILI9341_InvertDisplay(bool invert);
extern void ILI9341_EnableDisplay(bool enable);
extern void ILI9341_SetTextWrap(bool wrap);

/* Simple bitmap font: 5x8 pixels per character */
static const uint8_t font_5x8[96][5] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* Space */
    { 0x3E, 0x5B, 0x4F, 0x5B, 0x3E }, /* ! - simplified */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* " */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* # */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* $ */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* % */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* & */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* ' */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* ( */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* ) */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* * */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* + */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* , */
    { 0x08, 0x08, 0x08, 0x08, 0x08 }, /* - */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* . */
    { 0x00, 0x20, 0x40, 0x80, 0x10 }, /* / */
    { 0x3E, 0x51, 0x49, 0x45, 0x3E }, /* 0 */
    { 0x00, 0x42, 0x7F, 0x40, 0x00 }, /* 1 */
    { 0x42, 0x61, 0x51, 0x49, 0x46 }, /* 2 */
    { 0x21, 0x41, 0x45, 0x4B, 0x31 }, /* 3 */
    { 0x18, 0x14, 0x12, 0x7F, 0x10 }, /* 4 */
    { 0x27, 0x45, 0x45, 0x45, 0x39 }, /* 5 */
    { 0x3C, 0x4A, 0x49, 0x49, 0x30 }, /* 6 */
    { 0x01, 0x71, 0x09, 0x05, 0x03 }, /* 7 */
    { 0x36, 0x49, 0x49, 0x49, 0x36 }, /* 8 */
    { 0x06, 0x49, 0x49, 0x29, 0x1E }, /* 9 */
    { 0x00, 0x36, 0x36, 0x00, 0x00 }, /* : */
    { 0x00, 0x56, 0x36, 0x00, 0x00 }, /* ; */
    { 0x08, 0x14, 0x22, 0x41, 0x00 }, /* < */
    { 0x14, 0x14, 0x14, 0x14, 0x14 }, /* = */
    { 0x00, 0x41, 0x22, 0x14, 0x08 }, /* > */
    { 0x02, 0x01, 0x51, 0x09, 0x06 }, /* ? */
    { 0x32, 0x49, 0x59, 0x51, 0x3E }, /* @ */
    { 0x7E, 0x11, 0x11, 0x11, 0x7E }, /* A */
    { 0x7F, 0x49, 0x49, 0x49, 0x36 }, /* B */
    { 0x3E, 0x41, 0x41, 0x41, 0x22 }, /* C */
    { 0x7F, 0x41, 0x41, 0x22, 0x1C }, /* D */
    { 0x7F, 0x49, 0x49, 0x49, 0x41 }, /* E */
    { 0x7F, 0x09, 0x09, 0x09, 0x01 }, /* F */
    { 0x3E, 0x41, 0x49, 0x49, 0x7A }, /* G */
    { 0x7F, 0x08, 0x08, 0x08, 0x7F }, /* H */
    { 0x00, 0x41, 0x7F, 0x41, 0x00 }, /* I */
    { 0x20, 0x40, 0x41, 0x3F, 0x01 }, /* J */
    { 0x7F, 0x08, 0x14, 0x22, 0x41 }, /* K */
    { 0x7F, 0x40, 0x40, 0x40, 0x40 }, /* L */
    { 0x7F, 0x02, 0x0C, 0x02, 0x7F }, /* M */
    { 0x7F, 0x04, 0x08, 0x10, 0x7F }, /* N */
    { 0x3E, 0x41, 0x41, 0x41, 0x3E }, /* O */
    { 0x7F, 0x09, 0x09, 0x09, 0x06 }, /* P */
    { 0x3E, 0x41, 0x51, 0x21, 0x5E }, /* Q */
    { 0x7F, 0x09, 0x19, 0x29, 0x46 }, /* R */
    { 0x46, 0x49, 0x49, 0x49, 0x31 }, /* S */
    { 0x01, 0x01, 0x7F, 0x01, 0x01 }, /* T */
    { 0x3F, 0x40, 0x40, 0x40, 0x3F }, /* U */
    { 0x1F, 0x20, 0x40, 0x20, 0x1F }, /* V */
    { 0x3F, 0x40, 0x38, 0x40, 0x3F }, /* W */
    { 0x63, 0x14, 0x08, 0x14, 0x63 }, /* X */
    { 0x07, 0x08, 0x70, 0x08, 0x07 }, /* Y */
    { 0x61, 0x51, 0x49, 0x45, 0x43 }, /* Z */
    { 0x00, 0x7F, 0x41, 0x41, 0x00 }, /* [ */
    { 0x15, 0x16, 0x7C, 0x16, 0x15 }, /* \ */
    { 0x00, 0x41, 0x41, 0x7F, 0x00 }, /* ] */
    { 0x04, 0x02, 0x01, 0x02, 0x04 }, /* ^ */
    { 0x40, 0x40, 0x40, 0x40, 0x40 }, /* _ */
    { 0x00, 0x01, 0x02, 0x04, 0x00 }, /* ` */
    { 0x20, 0x54, 0x54, 0x54, 0x78 }, /* a */
    { 0x7F, 0x48, 0x44, 0x44, 0x38 }, /* b */
    { 0x38, 0x44, 0x44, 0x44, 0x20 }, /* c */
    { 0x38, 0x44, 0x44, 0x48, 0x7F }, /* d */
    { 0x38, 0x54, 0x54, 0x54, 0x18 }, /* e */
    { 0x08, 0x7E, 0x09, 0x01, 0x02 }, /* f */
    { 0x0C, 0x52, 0x52, 0x52, 0x3E }, /* g */
    { 0x7F, 0x08, 0x04, 0x04, 0x78 }, /* h */
    { 0x00, 0x44, 0x7D, 0x40, 0x00 }, /* i */
    { 0x20, 0x40, 0x44, 0x3D, 0x00 }, /* j */
    { 0x7F, 0x10, 0x28, 0x44, 0x00 }, /* k */
    { 0x00, 0x41, 0x7F, 0x40, 0x00 }, /* l */
    { 0x7C, 0x04, 0x18, 0x04, 0x78 }, /* m */
    { 0x7C, 0x08, 0x04, 0x04, 0x78 }, /* n */
    { 0x38, 0x44, 0x44, 0x44, 0x38 }, /* o */
    { 0x7C, 0x14, 0x14, 0x14, 0x08 }, /* p */
    { 0x08, 0x14, 0x14, 0x18, 0x7C }, /* q */
    { 0x7C, 0x08, 0x04, 0x04, 0x08 }, /* r */
    { 0x48, 0x54, 0x54, 0x54, 0x20 }, /* s */
    { 0x04, 0x3F, 0x44, 0x40, 0x20 }, /* t */
    { 0x3C, 0x40, 0x40, 0x20, 0x7C }, /* u */
    { 0x1C, 0x20, 0x40, 0x20, 0x1C }, /* v */
    { 0x3C, 0x40, 0x30, 0x40, 0x3C }, /* w */
    { 0x44, 0x28, 0x10, 0x28, 0x44 }, /* x */
    { 0x0C, 0x50, 0x50, 0x50, 0x3C }, /* y */
    { 0x44, 0x64, 0x54, 0x4C, 0x44 }, /* z */
    { 0x00, 0x08, 0x36, 0x41, 0x00 }, /* { */
    { 0x00, 0x00, 0x7F, 0x00, 0x00 }, /* | */
    { 0x00, 0x41, 0x36, 0x08, 0x00 }, /* } */
    { 0x10, 0x08, 0x08, 0x10, 0x08 }, /* ~ */
    { 0x00, 0x00, 0x00, 0x00, 0x00 }  /* DEL */
};

/*  API   */

bool display_init(display_handle_t* display,
                  int8_t rst, int8_t dc, int8_t cs, 
                  int8_t sclk, int8_t mosi, int8_t miso,
                  void* spi_instance, uint32_t spi_speed_hz) {
    if (!display) {
        return false;
    }

    /* Store pin configuration */
    display->pin_rst = rst;
    display->pin_dc = dc;
    display->pin_cs = cs;
    display->pin_sclk = sclk;
    display->pin_mosi = mosi;
    display->pin_miso = miso;

    /* Store SPI configuration */
    display->spi_instance = spi_instance;
    display->spi_speed_hz = spi_speed_hz;
    display->use_hardware_spi = true;

    /* Set default dimensions */
    display->width = DISPLAY_WIDTH;
    display->height = DISPLAY_HEIGHT;
    display->rotation = DISPLAY_ROTATION_0;
    display->cursor_x = 0;
    display->cursor_y = 0;
    display->text_color = COLOR_WHITE;
    display->text_bg_color = COLOR_BLACK;
    display->text_wrap = true;

    /* Configure via displaylib_16 */
    ILI9341_SetupGPIO(rst, dc, cs, sclk, mosi, miso);
    ILI9341_SetupScreenSize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    ILI9341_SetupSPI_HW(spi_speed_hz, spi_instance);
    ILI9341_Initialize();

    display->initialized = true;
    return true;
}

void display_deinit(display_handle_t* display) {
    if (!display) {
        return;
    }

    ILI9341_EnableDisplay(false);
    display->initialized = false;
}

void display_fill_screen(display_handle_t* display, uint16_t color) {
    if (!display || !display->initialized) {
        return;
    }

    ILI9341_FillScreen(color);
}

void display_fill_rect(display_handle_t* display, uint16_t x, uint16_t y, 
                       uint16_t width, uint16_t height, uint16_t color) {
    if (!display || !display->initialized) {
        return;
    }

    ILI9341_FillRect(x, y, width, height, color);
}

void display_draw_rect(display_handle_t* display, uint16_t x, uint16_t y, 
                       uint16_t width, uint16_t height, uint16_t color) {
    if (!display || !display->initialized) {
        return;
    }

    ILI9341_DrawLine(x, y, x + width - 1, y, color);
    ILI9341_DrawLine(x + width - 1, y, x + width - 1, y + height - 1, color);
    ILI9341_DrawLine(x, y + height - 1, x + width - 1, y + height - 1, color);
    ILI9341_DrawLine(x, y, x, y + height - 1, color);
}

void display_draw_pixel(display_handle_t* display, uint16_t x, uint16_t y, uint16_t color) {
    if (!display || !display->initialized) {
        return;
    }

    ILI9341_DrawPixel(x, y, color);
}

void display_draw_line(display_handle_t* display, int16_t x0, int16_t y0, 
                       int16_t x1, int16_t y1, uint16_t color) {
    if (!display || !display->initialized) {
        return;
    }

    ILI9341_DrawLine(x0, y0, x1, y1, color);
}

void display_set_cursor(display_handle_t* display, uint16_t x, uint16_t y) {
    if (!display || !display->initialized) {
        return;
    }

    display->cursor_x = x;
    display->cursor_y = y;
    ILI9341_SetCursor(x, y);
}

void display_set_text_color(display_handle_t* display, uint16_t text_color, uint16_t bg_color) {
    if (!display || !display->initialized) {
        return;
    }

    display->text_color = text_color;
    display->text_bg_color = bg_color;
    ILI9341_SetTextColor(text_color, bg_color);
}

void display_set_text_wrap(display_handle_t* display, bool wrap) {
    if (!display || !display->initialized) {
        return;
    }

    display->text_wrap = wrap;
    ILI9341_SetTextWrap(wrap);
}

void display_write_char(display_handle_t* display, char c) {
    if (!display || !display->initialized) {
        return;
    }

    ILI9341_WriteChar(c);
    display->cursor_x++;
}

void display_write_string(display_handle_t* display, const char* str) {
    if (!display || !str || !display->initialized) {
        return;
    }

    ILI9341_WriteString(str);
    display->cursor_x += strlen(str) * 6; /* Approximate */
}

void display_set_rotation(display_handle_t* display, display_rotation_t rotation) {
    if (!display || !display->initialized) {
        return;
    }

    display->rotation = rotation;
    ILI9341_SetRotation((uint8_t)rotation);

    /* Swap width/height for 90/270 rotations */
    if (rotation == DISPLAY_ROTATION_90 || rotation == DISPLAY_ROTATION_270) {
        uint16_t tmp = display->width;
        display->width = display->height;
        display->height = tmp;
    }
}

void display_invert(display_handle_t* display, bool invert) {
    if (!display || !display->initialized) {
        return;
    }

    ILI9341_InvertDisplay(invert);
}

void display_set_enabled(display_handle_t* display, bool enable) {
    if (!display || !display->initialized) {
        return;
    }

    ILI9341_EnableDisplay(enable);
}

uint16_t display_get_width(const display_handle_t* display) {
    if (!display || !display->initialized) {
        return 0;
    }

    return display->width;
}

uint16_t display_get_height(const display_handle_t* display) {
    if (!display || !display->initialized) {
        return 0;
    }

    return display->height;
}

uint16_t display_get_cursor_x(const display_handle_t* display) {
    if (!display) {
        return 0;
    }

    return display->cursor_x;
}

uint16_t display_get_cursor_y(const display_handle_t* display) {
    if (!display) {
        return 0;
    }

    return display->cursor_y;
}
