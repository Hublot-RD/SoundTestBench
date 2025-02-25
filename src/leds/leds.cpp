#include "leds.hpp"

namespace leds {

CRGBArray<NUM_LEDS> led_stick;

void setup(void) {
    /**
     * @brief Setup the LED stick
    */
    // Setup the LED stick
    FastLED.addLeds<NEOPIXEL,LEDS_PIN>(led_stick, NUM_LEDS);
    pinMode(LEDS_GND, OUTPUT);
    pinMode(LEDS_VCC, OUTPUT);
    digitalWrite(LEDS_GND, LOW);
    digitalWrite(LEDS_VCC, HIGH);
}

void set_from_potentiometer(uint16_t potentiometer_value) {
    /**
     * @brief Set the LEDs color based on the potentiometer value
     * 
     * @param potentiometer_value: Potentiometer value to determine the color [0, 1023]
    */
    potentiometer_value++; // To have at least one LED ON, and achieve full LEDs ON when potentiometer_value = 1023
    
    for(uint8_t i = 0; i < NUM_LEDS; i++) {
        if(potentiometer_value >= 1024/NUM_LEDS*(i+1)) { // LED is fully ON
            led_stick[i] = WHITE;
        } 
        else if (potentiometer_value >= 1024/NUM_LEDS*(i)) { // LED is partially ON
            uint8_t color_hue = (potentiometer_value - 1024/NUM_LEDS*i) * HSVHue::HUE_BLUE / (1024/NUM_LEDS);
            led_stick[i] = CHSV(color_hue, 255, BRIGHTNESS);
        }
        else { // LED is fully OFF
            led_stick[i] = BLACK;
        }
    }
    FastLED.show();
}

void set_mode(uint8_t mode){
    /**
     * @brief Set the LEDs color based on the mode
     * 
     * @param mode: Mode to set
    */
    for(uint8_t i = 0; i < NUM_LEDS; i++) {
        if(NUM_LEDS - i - 1 == mode) {led_stick[i] = BLUE;} 
        else {led_stick[i] = BLACK;}
    }
    FastLED.show();
}

void set(uint8_t channel, CHSV color) {
    /**
     * @brief Set the color of a specific LED
     * 
     * @param channel: Channel number
     * @param color: Color to set
    */
    led_stick[channel] = color;
    FastLED.show();
}

void animate(void) {
    /**
     * @brief Animate the LEDs with a rainbow effect
    */
    for (uint8_t hue = 0; hue < 255; hue++) {
        for(uint8_t i = 0; i < NUM_LEDS; i++) {
            led_stick[i] = CHSV(hue+32*i, 255, BRIGHTNESS);
        }
        FastLED.show();
        delay(10);
    }
}

} // namespace leds