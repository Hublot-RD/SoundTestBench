#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

namespace motor {
    // Constants
    constexpr uint32_t MAX_SPEED = 80000;      // Maximal speed of the motor

    // Pinout
    constexpr uint8_t A_DIR_PIN = 12;          // Pins for the direction of motor A
    constexpr uint8_t A_PWM_PIN = 3;           // Pins for the PWM of motor A
    constexpr uint8_t A_BRK_PIN = 9;           // Pins for the brake of motor A
    constexpr uint8_t B_DIR_PIN = 13;          // Pins for the direction of motor B
    constexpr uint8_t B_PWM_PIN = 11;          // Pins for the PWM of motor B
    constexpr uint8_t B_BRK_PIN = 8;           // Pins for the brake of motor B

    // Function prototypes
    void setup(void);
    void set_speed(uint32_t speed, bool motor_A);
    void set_direction(bool forward, bool motor_A);
    void stop(bool motor_A);
    
} // namespace motor

#endif // MOTOR_H