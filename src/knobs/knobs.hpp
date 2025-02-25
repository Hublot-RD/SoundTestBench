#ifndef KNOBS_H
#define KNOBS_H

#include <Arduino.h>

namespace knobs {
    // Constants
    constexpr int32_t MAX_SPEED_CORR = 5000;        // Maximum value of the speed correction potentiometer, in mRPM
    constexpr int32_t MIN_SPEED_CORR = -MAX_SPEED_CORR;          // Maximum value of the speed correction potentiometer, in mRPM

    // Pinout
    constexpr uint8_t SPEED_PIN =       A2;         // Pin for the speed potentiometer
    constexpr uint8_t START_PIN =       2;          // Pin for the start/stop button

    // Global variables declaration
    extern bool start_button_pressed;

    // Function prototypes
    void setup(void);
    int32_t get_speed_correction();
    
} // namespace knobs

#endif // KNOBS_H