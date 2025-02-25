/*
Minimal version to test the motor library.

This code is a minimal version to test the motor library. It makes the two motors turn.
*/

#include <Arduino.h>
#include "motor.hpp"

// Constants
constexpr bool DEBUG = true;
constexpr uint32_t SERIAL_BAUD_RATE = 115200;


void setup() {
    // Open serial communications on the native USB port
    if(DEBUG) {Serial.begin(SERIAL_BAUD_RATE);while(!Serial);}

    // Setup the library
    motor::setup();
}


void loop() {
    static bool dir = false;

    motor::set_speed(80000, true);
    delay(2000);

    motor::stop(true);
    dir = !dir;
    motor::set_direction(dir, true);
    delay(1000);
}
