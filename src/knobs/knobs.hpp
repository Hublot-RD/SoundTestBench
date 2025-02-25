#ifndef KNOBS_H
#define KNOBS_H

#include <Arduino.h>

namespace knobs {
    // Constants
    constexpr uint32_t MAX_SPEED_CORR = 5;           // Maximum value of the speed correction potentiometer, in RPM
    constexpr uint32_t MIN_SPEED_CORR = -MAX_SPEED_CORR;          // Maximum value of the speed correction potentiometer, in RPM

    // Pinout
    constexpr uint8_t SPEED_PIN =       A1;          // Pin for the speed potentiometer
    constexpr uint8_t SPEED_VCC_PIN =   A2;          // VCC pin for the speed potentiometer
    constexpr uint8_t SPEED_GND_PIN =   A0;          // GND pin for the speed potentiometer
    constexpr uint8_t START_PIN =       11;          // Pin for the start/stop button

    // Global variables declaration
    extern uint32_t speed;
    extern bool tare_needed;
    extern bool start_button_pressed;

    // Function prototypes
    void setup(void);
    int get_speed_correction();
    
} // namespace knobs

#endif // KNOBS_H