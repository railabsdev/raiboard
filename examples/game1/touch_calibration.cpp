#include <stdio.h>
#include "pico/stdlib.h"
#include "Screen.hpp"
#include "xpt2046.h"

int main() {
    stdio_init_all();
    sleep_ms(2000);  // Give time to connect serial monitor
    
    printf("\n\n========================================\n");
    printf("XPT2046 Touch Calibration Tool\n");
    printf("========================================\n");
    printf("Press the FOUR CORNERS of the screen:\n");
    printf("1. Top-Left\n");
    printf("2. Top-Right\n");
    printf("3. Bottom-Left\n");
    printf("4. Bottom-Right\n");
    printf("========================================\n\n");

    // Initialize screen (this properly sets up SPI and display)
    Screen screen;
    ILI9341_TFT &display = screen.display();
    
    display.fillScreen(display.C_BLACK);

    printf("Screen initialized. Touch the screen...\n\n");

    uint32_t sample_count = 0;
    bool was_touching = false;

    while (true) {
        // Check if touch is pressed
        bool is_pressed = screen.isTouchPressed();
        
        if (is_pressed) {
            if (!was_touching) {
                printf("\n=== New Touch Started ===\n");
                was_touching = true;
            }
            
            // Continuously read while touching
            uint16_t screen_x = 0, screen_y = 0;
            bool valid = screen.readTouch(screen_x, screen_y);
            
            if (valid) {
                // Read raw values directly - multiple independent reads
                uint16_t raw_x1 = ts_get_x_raw();
                uint16_t raw_y1 = ts_get_y_raw();
                sleep_ms(10);
                uint16_t raw_x2 = ts_get_x_raw();
                uint16_t raw_y2 = ts_get_y_raw();
                
                sample_count++;
                
                printf("Sample #%lu: RAW X: %4u,%4u  Y: %4u,%4u  | MAPPED X: %3u  Y: %3u\n", 
                       sample_count, raw_x1, raw_x2, raw_y1, raw_y2, screen_x, screen_y);
                
                // Draw a visual marker on screen
                if (sample_count % 5 == 1) {  // Only draw every 5th sample to not clutter
                    display.fillCircle(screen_x, screen_y, 4, display.C_RED);
                }
            }
            
            sleep_ms(100);  // Read every 100ms while held
        } else if (was_touching) {
            // Touch released
            printf("=== Touch Released ===\n\n");
            was_touching = false;
        }
        
        sleep_ms(10);
    }

    return 0;
}
