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
void set_motor_speed(uint32_t A_speed, uint32_t B_speed, uint32_t speed_correction);

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

    // Update the display
    if(display_should_stay_x_cycles > 0) {
        display_should_stay_x_cycles--;
    } else {
        leds::set_from_potentiometer(analogRead(knobs::SPEED_PIN));
    }

    // Update according to knobs
    if(is_running) {set_motor_speed(A_NOMINAL_SPEED, B_NOMINAL_SPEED, knobs::get_speed_correction());}

    // Start/stop if necessary
    if(knobs::start_button_pressed) {
        display_should_stay_x_cycles = 1*LOOP_FREQ_HZ;
        if(is_running) {
            motor::stop(true); motor::stop(false);
            is_running = false;
        }
        else {
            set_motor_speed(A_NOMINAL_SPEED, B_NOMINAL_SPEED, knobs::get_speed_correction());
            is_running = true;    
        }
        delay(300); // To avoid double presses
        knobs::start_button_pressed = false;
        if(DEBUG) {Serial.println("Started/stopped motor");}
    }

    delay(1000/LOOP_FREQ_HZ);
}

void set_motor_speed(uint32_t A_speed, uint32_t B_speed, int32_t speed_correction) {
    /**
     * @brief Set the speed of the motors.
     * 
     * @param A_speed: Speed of motor A, in mRPM
     * @param B_speed: Speed of motor B, in mRPM
     * @param speed_correction: Speed correction, in mRPM
     * 
     * @note The speed correction is applied to the motor A only.
     */
    // Set the speed of the motors
    motor::set_speed(A_speed + speed_correction, true);
    motor::set_speed(B_speed, false);

    Serial.print("A: "); Serial.print(A_speed + speed_correction); Serial.print(" mRPM\t");
    Serial.print("B: "); Serial.print(B_speed); Serial.print(" mRPM\t");
    Serial.print("Correction: "); Serial.print(speed_correction); Serial.println(" mRPM");
}