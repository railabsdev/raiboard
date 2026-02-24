#include <stdio.h>

#include "pico/stdlib.h"
#include "screen.hpp"

int main() {
    stdio_init_all();
    sleep_ms(1000);
    printf("Touch test starting...\n");

    Screen screen;
    ILI9341_TFT &display = screen.display();

    uint32_t last_print_ms = 0;
    bool last_pressed = false;

    while (true) {
        bool pressed = screen.isTouchPressed();

        if (pressed) {
            uint16_t x = 0;
            uint16_t y = 0;
            bool has_touch = screen.readTouch(x, y);

            if (has_touch) {
                display.fillCircle(x, y, 2, display.C_YELLOW);
            }

            uint32_t now_ms = to_ms_since_boot(get_absolute_time());
            if (has_touch && now_ms - last_print_ms > 100) {
                printf("touch: x=%u y=%u\n", x, y);
                last_print_ms = now_ms;
            }
        } else if (last_pressed) {
            uint32_t now_ms = to_ms_since_boot(get_absolute_time());
            if (now_ms - last_print_ms > 250) {
                printf("touch: released\n");
                last_print_ms = now_ms;
            }
        }

        last_pressed = pressed;
        sleep_ms(10);
    }

    return 0;
}
