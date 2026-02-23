# RaiBoard Project Examples

Common project structures and examples for RaiBoard.

---

## Table of Contents

- [Simple Display App](#simple-display-app)
- [LoRa Communication](#lora-communication)
- [Multi-Module Project](#multi-module-project)
- [Using Only Display](#using-only-display)
- [Using Only LoRa](#using-only-lora)
- [Hardware Integration Examples](#hardware-integration-examples)

---

## Simple Display App

Bare minimum project for drawing on the display.

### Project Structure

```
display_demo/
├── CMakeLists.txt
└── main.cpp
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.12)

add_executable(display_demo main.cpp)

target_link_libraries(display_demo
    pico_drivers_cpp
    pico_stdlib
)

pico_enable_stdio_usb(display_demo 1)
pico_add_extra_outputs(display_demo)
```

### main.cpp

```cpp
#include "pico/stdlib.h"
#include "Display.hpp"

int main() {
    stdio_init_all();
    
    // Initialize display
    Display screen;
    if (!screen.init(15, 16, 17, 18, 19, -1, spi0, 25000000)) {
        printf("Display init failed!\n");
        return -1;
    }
    
    // Clear screen
    screen.fillScreen(COLOR_BLACK);
    sleep_ms(500);
    
    // Draw various shapes
    screen.fillRectangle(10, 10, 100, 50, COLOR_BLUE);
    screen.drawCircle(150, 50, 30, COLOR_RED);
    screen.drawLine(10, 100, 230, 100, COLOR_GREEN);
    
    // Draw text
    screen.drawString(30, 150, "RAIBoard", COLOR_WHITE, COLOR_BLACK);
    screen.drawString(20, 200, "Display Test", COLOR_CYAN, COLOR_BLACK);
    
    // Main loop
    int count = 0;
    while (1) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Count: %d", count++);
        screen.drawString(50, 250, buffer, COLOR_YELLOW, COLOR_BLACK);
        sleep_ms(1000);
    }
    
    return 0;
}
```

### Setup in Main CMakeLists.txt

Add to `/raiboard/CMakeLists.txt`:

```cmake
add_subdirectory(drivers)
add_subdirectory(display_demo)
```

### Build and Run

```bash
cd /raiboard/build
cmake ..
ninja
# Load build/display_demo/display_demo.uf2 to Pico
```

---

## LoRa Communication

Simple LoRa transmitter/receiver.

### Project Structure

```
lora_demo/
├── CMakeLists.txt
└── main.cpp
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.12)

add_executable(lora_demo main.cpp)

target_link_libraries(lora_demo
    pico_drivers_cpp
    pico_stdlib
)

pico_enable_stdio_usb(lora_demo 1)
pico_add_extra_outputs(lora_demo)
```

### main.cpp - Simple Transmitter

```cpp
#include "pico/stdlib.h"
#include "LoRa.hpp"
#include "Display.hpp"

int main() {
    stdio_init_all();
    
    // Initialize display
    Display screen;
    screen.init(15, 16, 17, 18, 19, -1, spi0, 25000000);
    screen.fillScreen(COLOR_BLACK);
    screen.drawString(50, 50, "LoRa TX Demo", COLOR_WHITE, COLOR_BLACK);
    
    // Initialize LoRa
    LoRa radio;
    if (!radio.init(915000000, LORA_SF_7)) {
        printf("LoRa init failed!\n");
        screen.drawString(30, 100, "LoRa Init Failed!", COLOR_RED, COLOR_BLACK);
        while (1) sleep_ms(1000);
    }
    
    screen.drawString(30, 120, "Ready to TX", COLOR_GREEN, COLOR_BLACK);
    
    // Transmit counter
    uint8_t counter = 0;
    while (1) {
        // Create message
        uint8_t message[10] = {0xAA, 0xBB, counter};
        
        // Transmit
        if (radio.transmit(message, 3)) {
            printf("Transmitted packet %d\n", counter);
            char buf[32];
            snprintf(buf, sizeof(buf), "TX: %d", counter);
            screen.drawString(50, 150, buf, COLOR_CYAN, COLOR_BLACK);
        }
        
        counter++;
        sleep_ms(2000);
    }
    
    return 0;
}
```

### main.cpp - Simple Receiver

```cpp
#include "pico/stdlib.h"
#include "LoRa.hpp"
#include "Display.hpp"

Display* g_screen = nullptr;
int rx_count = 0;
int rssi_value = 0;

void rx_callback(const uint8_t* data, size_t len, int16_t rssi, int8_t snr) {
    rx_count++;
    rssi_value = rssi;
    
    printf("Received %d bytes, RSSI: %d, SNR: %d\n", len, rssi, snr);
    
    if (g_screen) {
        char buf[32];
        snprintf(buf, sizeof(buf), "RX: %d", rx_count);
        g_screen->drawString(50, 150, buf, COLOR_GREEN, COLOR_BLACK);
        
        snprintf(buf, sizeof(buf), "RSSI: %d", rssi);
        g_screen->drawString(50, 180, buf, COLOR_YELLOW, COLOR_BLACK);
    }
}

int main() {
    stdio_init_all();
    
    // Initialize display
    Display screen;
    screen.init(15, 16, 17, 18, 19, -1, spi0, 25000000);
    screen.fillScreen(COLOR_BLACK);
    screen.drawString(50, 50, "LoRa RX Demo", COLOR_WHITE, COLOR_BLACK);
    g_screen = &screen;
    
    // Initialize LoRa
    LoRa radio;
    if (!radio.init(915000000, LORA_SF_7)) {
        printf("LoRa init failed!\n");
        screen.drawString(30, 100, "LoRa Init Failed!", COLOR_RED, COLOR_BLACK);
        while (1) sleep_ms(1000);
    }
    
    // Set receive callback
    radio.setRxCallback(rx_callback);
    
    // Start receiving
    radio.setRxMode();
    screen.drawString(30, 120, "Listening...", COLOR_GREEN, COLOR_BLACK);
    
    // Main loop
    while (1) {
        tight_loop_contents();
    }
    
    return 0;
}
```

---

## Multi-Module Project

Larger project with separate files for organization.

### Project Structure

```
weather_station/
├── CMakeLists.txt
├── main.cpp
├── display_manager.h
├── display_manager.cpp
├── sensor_reader.h
└── sensor_reader.cpp
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.12)

add_executable(weather_station
    main.cpp
    display_manager.cpp
    sensor_reader.cpp
)

target_include_directories(weather_station PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(weather_station
    pico_drivers_cpp
    pico_stdlib
    hardware_adc
    hardware_timer
)

pico_enable_stdio_usb(weather_station 1)
pico_add_extra_outputs(weather_station)
```

### display_manager.h

```cpp
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "Display.hpp"

class DisplayManager {
private:
    Display* screen;
    
public:
    DisplayManager();
    ~DisplayManager();
    
    bool initialize();
    void updateTemperature(float temp);
    void updateHumidity(float humidity);
    void updateStatus(const char* status);
};

#endif // DISPLAY_MANAGER_H
```

### display_manager.cpp

```cpp
#include "display_manager.h"

DisplayManager::DisplayManager() : screen(nullptr) {}

DisplayManager::~DisplayManager() {
    if (screen) delete screen;
}

bool DisplayManager::initialize() {
    screen = new Display();
    if (!screen->init(15, 16, 17, 18, 19, -1, spi0, 25000000)) {
        return false;
    }
    screen->fillScreen(COLOR_BLACK);
    return true;
}

void DisplayManager::updateTemperature(float temp) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Temp: %.1f C", temp);
    screen->drawString(10, 50, buf, COLOR_WHITE, COLOR_BLACK);
}

void DisplayManager::updateHumidity(float humidity) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Humidity: %.1f%%", humidity);
    screen->drawString(10, 100, buf, COLOR_WHITE, COLOR_BLACK);
}

void DisplayManager::updateStatus(const char* status) {
    screen->drawString(10, 200, status, COLOR_CYAN, COLOR_BLACK);
}
```

### main.cpp

```cpp
#include "pico/stdlib.h"
#include "display_manager.h"
#include "sensor_reader.h"

int main() {
    stdio_init_all();
    
    // Initialize display
    DisplayManager display;
    if (!display.initialize()) {
        printf("Display init failed!\n");
        return -1;
    }
    display.updateStatus("Initializing...");
    
    // Initialize sensors
    SensorReader sensors;
    if (!sensors.initialize()) {
        printf("Sensor init failed!\n");
        display.updateStatus("Sensor Init Failed!");
        while (1) sleep_ms(1000);
    }
    
    display.updateStatus("Ready!");
    
    // Main loop
    while (1) {
        float temp = sensors.readTemperature();
        float humidity = sensors.readHumidity();
        
        display.updateTemperature(temp);
        display.updateHumidity(humidity);
        
        printf("Temp: %.1f, Humidity: %.1f\n", temp, humidity);
        
        sleep_ms(1000);
    }
    
    return 0;
}
```

---

## Using Only Display

Project that doesn't use LoRa.

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.12)

add_executable(display_only main.cpp)

target_link_libraries(display_only
    pico_drivers_c_display  # Just the display driver
    pico_stdlib
)

target_include_directories(display_only PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../drivers/c_drivers/display
)

pico_add_extra_outputs(display_only)
```

### main.cpp (C Style)

```c
#include "pico/stdlib.h"
#include "ili9341_display.h"

int main() {
    stdio_init_all();
    
    display_init_default();
    display_fill_screen(COLOR_BLACK);
    
    display_write_string(50, 100, "Hello Display!", COLOR_WHITE, COLOR_BLACK);
    
    while (1) {
        sleep_ms(1000);
    }
    
    return 0;
}
```

---

## Using Only LoRa

Project that doesn't use display.

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.12)

add_executable(lora_only main.cpp)

target_link_libraries(lora_only
    pico_drivers_c_lora     # Just the LoRa driver
    pico_stdlib
)

target_include_directories(lora_only PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../drivers/c_drivers/lora
)

pico_enable_stdio_usb(lora_only 1)
pico_add_extra_outputs(lora_only)
```

### main.cpp (C Style)

```c
#include "pico/stdlib.h"
#include "lora_radio.h"

int main() {
    stdio_init_all();
    
    if (!lora_init_default()) {
        printf("LoRa init failed!\n");
        return -1;
    }
    
    printf("LoRa initialized\n");
    
    // Transmit data
    uint8_t data[] = {0xAA, 0xBB, 0xCC};
    lora_transmit(data, 3);
    printf("Data transmitted\n");
    
    while (1) {
        sleep_ms(1000);
    }
    
    return 0;
}
```

---

## Hardware Integration Examples

### With Temperature Sensor (ADC)

```cpp
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "Display.hpp"

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);  // ADC channel 0 on GPIO 26
    
    Display screen;
    screen.init(15, 16, 17, 18, 19, -1, spi0, 25000000);
    screen.fillScreen(COLOR_BLACK);
    
    while (1) {
        adc_select_input(0);
        uint16_t raw = adc_read();
        
        // Convert to voltage (assuming 3.3V reference)
        float voltage = raw * 3.3f / 4096.0f;
        
        // Convert to temperature (adjust based on your sensor)
        float temp = (voltage - 0.5f) / 0.01f;
        
        char buf[32];
        snprintf(buf, sizeof(buf), "Temp: %.1f C", temp);
        screen.drawString(50, 100, buf, COLOR_WHITE, COLOR_BLACK);
        
        sleep_ms(1000);
    }
    
    return 0;
}
```

### With Button Input

```cpp
#include "pico/stdlib.h"
#include "Display.hpp"

const int BUTTON_PIN = 28;

bool button_pressed() {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    
    return !gpio_get(BUTTON_PIN);  // Button pulls low when pressed
}

int main() {
    stdio_init_all();
    
    Display screen;
    screen.init(15, 16, 17, 18, 19, -1, spi0, 25000000);
    screen.fillScreen(COLOR_BLACK);
    
    int count = 0;
    
    while (1) {
        if (button_pressed()) {
            count++;
            char buf[32];
            snprintf(buf, sizeof(buf), "Count: %d", count);
            screen.drawString(50, 100, buf, COLOR_CYAN, COLOR_BLACK);
            
            // Debounce
            sleep_ms(200);
        }
    }
    
    return 0;
}
```

### With UART Communication

```cpp
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "Display.hpp"

int main() {
    stdio_init_all();
    
    // Initialize UART0 on GPIO 0/1
    uart_init(uart0, 9600);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    
    Display screen;
    screen.init(15, 16, 17, 18, 19, -1, spi0, 25000000);
    screen.fillScreen(COLOR_BLACK);
    
    while (1) {
        if (uart_is_readable(uart0)) {
            char c = uart_getc(uart0);
            printf("Received: %c\n", c);
            screen.drawChar(50, 100, c, COLOR_WHITE, COLOR_BLACK);
        }
        
        sleep_ms(100);
    }
    
    return 0;
}
```

---

## Summary

When creating a new project:

1. **Choose your base template** - See examples above
2. **Copy and modify** - Adjust for your hardware
3. **Create CMakeLists.txt** - Use the relevant template
4. **Add to main CMakeLists.txt** - Register subdirectory
5. **Build and upload** - `ninja` in build directory

For more information:
- [QUICKSTART.md](../QUICKSTART.md) - Get started in 5 minutes
- [README.md](../README.md) - Full API documentation
- [CMAKE_GUIDE.md](../CMAKE_GUIDE.md) - Build system details

---

**These examples show the power and flexibility of RAIBoard - use only the drivers you need, and organize your code however makes sense for your project!**
