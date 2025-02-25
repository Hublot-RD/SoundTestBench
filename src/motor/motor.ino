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
    if(DEBUG) {SerialUSB.begin(SERIAL_BAUD_RATE);while(!SerialUSB);}

    // Setup the library
    motor::setup();
}


void loop() {
    delay(1000);
}
