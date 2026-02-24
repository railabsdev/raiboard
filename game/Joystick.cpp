#include "Joystick.hpp"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <cmath>

Joystick::Joystick() {
    // Initialize ADC
    adc_init();
    
    // Configure analog pins
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);
    
    // Configure button pin
    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN);  // Most joysticks are active low
}

uint16_t Joystick::readX() const {
    adc_select_input(0);  // ADC0 = GP26
    return adc_read();
}

uint16_t Joystick::readY() const {
    adc_select_input(1);  // ADC1 = GP27
    return adc_read();
}

float Joystick::applyDeadzone(int16_t value) const {
    if (std::abs(value) < DEADZONE) {
        return 0.0f;
    }
    
    // Map to -1.0 to 1.0 range, accounting for deadzone
    float normalized = static_cast<float>(value) / (CENTER - DEADZONE);
    return std::max(-1.0f, std::min(1.0f, normalized));
}

float Joystick::readXNormalized() const {
    int16_t raw = static_cast<int16_t>(readX()) - CENTER;
    return applyDeadzone(raw);
}

float Joystick::readYNormalized() const {
    int16_t raw = static_cast<int16_t>(readY()) - CENTER;
    return applyDeadzone(raw);
}

bool Joystick::isButtonPressed() const {
    return !gpio_get(SW_PIN);  // Active low
}

void Joystick::getDirection(int16_t& dx, int16_t& dy, float sensitivity) const {
    float x = readXNormalized();
    float y = readYNormalized();
    
    dx = static_cast<int16_t>(x * sensitivity);
    dy = static_cast<int16_t>(y * sensitivity);
}
