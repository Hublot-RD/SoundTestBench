#include "knobs.hpp"

namespace knobs {

// Global variables definition
bool start_button_pressed = false;

void setup() {
    cli(); // Disable global interrupts

    // Configure START button pins
    pinMode(START_PIN, INPUT_PULLUP);

    // Configure SPEED potentiometer pins
    pinMode(SPEED_PIN, INPUT);
    // pinMode(SPEED_VCC_PIN, OUTPUT);
    // digitalWrite(SPEED_VCC_PIN, HIGH);
    // pinMode(SPEED_GND_PIN, OUTPUT);
    // digitalWrite(SPEED_GND_PIN, LOW);

    // Configure external interrupt on INT0 (pin 2)
    EICRA |= (1 << ISC01);  // Falling edge detection on INT0
    EICRA &= ~(1 << ISC00); 
    EIMSK |= (1 << INT0);   // Enable INT0

    sei(); // Enable global interrupts
}

int32_t get_speed_correction() {
    /**
     * @brief Get the value of the speed correction according to potentiometer.
     * 
     * @return int32_t The value of the speed correction, in mRPM. Range: MIN_SPEED_CORR to MAX_SPEED_CORR
    */
    int32_t tmp = analogRead(SPEED_PIN)/1023.0 * (MAX_SPEED_CORR - MIN_SPEED_CORR) + MIN_SPEED_CORR;
    if(tmp < MIN_SPEED_CORR) {
        return MIN_SPEED_CORR;
    }
    if(tmp > MAX_SPEED_CORR) {
        return MAX_SPEED_CORR;
    }
    return tmp;
}

void start_handler() {
    // Mode button was pressed
    start_button_pressed = true;
}

} // namespace knobs

// Interrupt handler for the pushbuttons. Must be outside of any namespace
ISR(INT0_vect) {
    knobs::start_handler();
}