#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include <cstdint>

class Joystick {
public:
    Joystick();
    ~Joystick() = default;

    // Read analog values (0-4095 for 12-bit ADC)
    uint16_t readX() const;
    uint16_t readY() const;
    
    // Read normalized values (-1.0 to 1.0, with deadzone)
    float readXNormalized() const;
    float readYNormalized() const;
    
    // Button state (pressed = true)
    bool isButtonPressed() const;
    
    // Get direction as screen coordinates (centered, with deadzone)
    void getDirection(int16_t& dx, int16_t& dy, float sensitivity = 1.0f) const;

private:
    static constexpr uint8_t VRX_PIN = 26;  // ADC0
    static constexpr uint8_t VRY_PIN = 27;  // ADC1
    static constexpr uint8_t SW_PIN = 8;   // Button
    
    static constexpr uint16_t CENTER = 2048;  // 12-bit ADC center
    static constexpr uint16_t DEADZONE = 200; // Deadzone around center
    
    float applyDeadzone(int16_t value) const;
};

#endif // JOYSTICK_HPP
