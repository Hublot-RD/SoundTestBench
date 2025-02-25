/*
Minimal version to test the LEDs library.

This code is a minimal version to test the LEDs library. It sets the 8 LEDs to different colors.
*/

#include <Arduino.h>
#include "leds.hpp"

// Constants
constexpr bool DEBUG = true;
constexpr uint32_t SERIAL_BAUD_RATE = 115200;


void setup() {
    // Open serial communications on the native USB port
    if(DEBUG) {Serial.begin(SERIAL_BAUD_RATE);while(!Serial);}

    // Setup the library
    leds::setup();
    if(DEBUG) {Serial.println("Setup complete");}

    // Test the function
    for(uint16_t pot_value = 0; pot_value < 1024; pot_value++) {
      leds::set_from_potentiometer(pot_value);
      delay(5);
    }

    delay(5000);
}


void loop() {
    leds::animate();
}
