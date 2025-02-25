/*
Minimal version to test the buzzer library.

This code is a minimal version to test the buzzer library. It plays the Mario melody and the Metallica melody.
*/

#include <Arduino.h>
#include "knobs.hpp"

// Constants
constexpr bool DEBUG = true;
constexpr uint32_t SERIAL_BAUD_RATE = 115200;


void setup() {
    // Open serial communications on the native USB port
    if(DEBUG) {Serial.begin(SERIAL_BAUD_RATE);while(!Serial);}

    // Setup the library
    knobs::setup();
    if(DEBUG) {Serial.println("Setup complete");}
}


void loop() {
    Serial.print("Speed correction: "); Serial.println(knobs::get_speed_correction());
    Serial.print("Start button pressed: "); Serial.println(knobs::start_button_pressed);
    
    delay(1000);
}
