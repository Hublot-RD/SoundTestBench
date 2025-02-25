/*
Pulse Induction Metal detector.

This script is made for the Arduino Nano 33 IoT, based on the SAMD21G18A microcontroller.

User interface
Inputs :
- Start/stop button
- Speed correction potentiometer


Outputs:
- 8 LEDs as a display.
- Serial communication with the computer
*/

#include <Arduino.h>
#include "parameters.hpp"
#include "src\knobs\knobs.hpp"
#include "src\leds\leds.hpp"

constexpr float FILTER_CST = 0.98;

// Global variables
bool desired_channels[pulse::NB_COILS] = {true, true, true, true, false, false, false, false};
uint32_t time_shifting_threshold = 10;

// Function prototypes
void apply_mode(uint8_t mode);

void setup() {
    // Open serial communications on the native USB port
    if(DEBUG) {SerialUSB.begin(SERIAL_BAUD_RATE);}

    delay(STARTUP_TIME_MS);
    
    // Setup each library
    knobs::setup();
    leds::setup();

    // Play startup animation
    leds::animate();
}


void loop() {
    // Define static variables
    static uint8_t display_should_stay_x_cycles = 0; // Number of cycles the display should stay on the current state. Used when changing the mode or taring
    static uint8_t mode = 0; // Mode of the system

    // Update the display
    if(display_should_stay_x_cycles > 0) {
        display_should_stay_x_cycles--;
    } else {
        leds::set_from_pulse(meas.time_shifting, time_shifting_threshold, desired_channels);
    }

    // Update according to knobs
    uint16_t thrsh = pulse::set_threshold(knobs::get_threshold());
    time_shifting_threshold = knobs::get_sensitivity();
    // if(DEBUG) {
    //     SerialUSB.print("Sensitivity: "); SerialUSB.println(time_shifting_threshold);
    //     SerialUSB.print("Threshold: "); SerialUSB.println(thrsh);
    // }
    if(knobs::mode_button_pressed) {
        mode = (mode + 1) % 4;
        apply_mode(mode, &display_should_stay_x_cycles);
        knobs::mode_button_pressed = false;
    }

    // Tare if necessary
    if(knobs::tare_needed) {
        display_should_stay_x_cycles = 1*LOOP_FREQ_HZ;
        leds::set_tare();
        pulse::tare();
        knobs::tare_needed = false;
        if(DEBUG) {SerialUSB.println("Tare done");}
    }

    delay(1000/LOOP_FREQ_HZ);
}

void apply_mode(uint8_t mode, uint8_t *display_should_stay_x_cycles) {
    /**
     * @brief Apply a mode to the system
     * 
     * @param mode: Mode to apply
     */
    if(mode > 7) {return;} // Invalid mode

    switch(mode) {
        case 0:
            // All coil mode
            for (uint8_t i = 0; i < pulse::NB_COILS; i++) {
                desired_channels[i] = (i == 0 || i == 1 || i == 2 || i == 3) ? true : false;
            }
            break;
        case 1:
            // Outter coil mode
            for (uint8_t i = 0; i < pulse::NB_COILS; i++) {
                desired_channels[i] = (i == 0) ? true : false;
            }
            break;
        case 2:
            // Inner coil mode
            for (uint8_t i = 0; i < pulse::NB_COILS; i++) {
                desired_channels[i] = (i == 1 || i == 2 || i == 3) ? true : false;
            }
            break;
        case 3:
            // Inner coil mode
            for (uint8_t i = 0; i < pulse::NB_COILS; i++) {
                desired_channels[i] = (i == 1) ? true : false;
            }
            break;
        default:
            break;
    }
    *display_should_stay_x_cycles = 1*LOOP_FREQ_HZ;
    leds::set_mode(mode);
}