#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include <FastLED.h>

namespace leds {
    // Constants
    constexpr uint8_t NUM_LEDS = 8;         // Number of LEDs
    constexpr uint8_t BRIGHTNESS = 100;     // LEDs brightness, 0-255

    // Pinout
    constexpr uint8_t LEDS_VCC = 7;          // VCC pin for the 8 LEDs
    constexpr uint8_t LEDS_GND = 5;          // Ground pin for the 8 LEDs
    constexpr uint8_t LEDS_PIN = 6;          // Data pin for the 8 LEDs

    // Colors
    #define RED   CHSV(HSVHue::HUE_RED, 255, BRIGHTNESS)
    #define GREEN CHSV(HSVHue::HUE_GREEN, 255, BRIGHTNESS)
    #define BLUE  CHSV(HSVHue::HUE_BLUE, 255, BRIGHTNESS)
    #define YELLOW CHSV(HSVHue::HUE_YELLOW, 255, BRIGHTNESS)
    #define WHITE CHSV(0, 0, BRIGHTNESS)
    #define BLACK CHSV(0, 0, 0)

    // Function prototypes
    void setup(void);
    void set_from_potentiometer(uint16_t potentiometer_value);
    void set_mode(uint8_t mode);
    void set(uint8_t channel, CHSV color);
    void animate(void);
    
} // namespace leds

#endif // LEDS_H