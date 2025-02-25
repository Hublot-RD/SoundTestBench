/*
Dual motor controller with user interface.

This script is made for the Arduino Uno, based on the ATMEGA328P microcontroller.
An Arduino motor shield must be mounted on the Arduino.

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
#include "src\motor\motor.hpp"

// constexpr float FILTER_CST = 0.98;

// Global variables

// Function prototypes
// void apply_mode(uint8_t mode);

void setup() {
    // Open serial communications on the native USB port
    if(DEBUG) {Serial.begin(SERIAL_BAUD_RATE);}
    
    // Setup each library
    knobs::setup();
    leds::setup();
    motor::setup();

    // Play startup animation
    leds::animate();
}


void loop() {
    // Define static variables
    static uint8_t display_should_stay_x_cycles = 0; // Number of cycles the display should stay on the current state. Used when changing the mode or taring
    static bool is_running = false;
    // static uint8_t mode = 0; // Mode of the system

    Serial.print("Sensitivity: "); Serial.println(analogRead(knobs::SPEED_PIN));
    // Update the display
    if(display_should_stay_x_cycles > 0) {
        display_should_stay_x_cycles--;
    } else {
        leds::set_from_potentiometer(analogRead(knobs::SPEED_PIN));
    }


    // Update according to knobs
    // uint16_t thrsh = pulse::set_threshold(knobs::get_threshold());
    // time_shifting_threshold = knobs::get_sensitivity();
    // if(DEBUG) {
    //     Serial.print("Sensitivity: "); Serial.println(time_shifting_threshold);
    //     Serial.print("Threshold: "); Serial.println(thrsh);
    // }
    // if(knobs::mode_button_pressed) {
    //     mode = (mode + 1) % 4;
    //     apply_mode(mode, &display_should_stay_x_cycles);
    //     knobs::mode_button_pressed = false;
    // }

    // Start/stop if necessary
    if(knobs::start_button_pressed) {
        display_should_stay_x_cycles = 1*LOOP_FREQ_HZ;
        if(is_running) {
            motor::stop(true);
            is_running = false;
        }
        else {
            motor::set_speed(80000, true);
            is_running = true;    
        }
        delay(300); // To avoid double presses
        knobs::start_button_pressed = false;
        if(DEBUG) {Serial.println("Started/stopped motor");}
    }

    delay(1000/LOOP_FREQ_HZ);
}

// void apply_mode(uint8_t mode, uint8_t *display_should_stay_x_cycles) {
//     /**
//      * @brief Apply a mode to the system
//      * 
//      * @param mode: Mode to apply
//      */
//     if(mode > 7) {return;} // Invalid mode

//     switch(mode) {
//         case 0:
//             // All coil mode
//             for (uint8_t i = 0; i < pulse::NB_COILS; i++) {
//                 desired_channels[i] = (i == 0 || i == 1 || i == 2 || i == 3) ? true : false;
//             }
//             break;
//         case 1:
//             // Outter coil mode
//             for (uint8_t i = 0; i < pulse::NB_COILS; i++) {
//                 desired_channels[i] = (i == 0) ? true : false;
//             }
//             break;
//         case 2:
//             // Inner coil mode
//             for (uint8_t i = 0; i < pulse::NB_COILS; i++) {
//                 desired_channels[i] = (i == 1 || i == 2 || i == 3) ? true : false;
//             }
//             break;
//         case 3:
//             // Inner coil mode
//             for (uint8_t i = 0; i < pulse::NB_COILS; i++) {
//                 desired_channels[i] = (i == 1) ? true : false;
//             }
//             break;
//         default:
//             break;
//     }
//     *display_should_stay_x_cycles = 1*LOOP_FREQ_HZ;
//     leds::set_mode(mode);
// }