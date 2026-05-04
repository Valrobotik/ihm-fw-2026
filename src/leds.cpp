#include "leds.hpp"
#include "param.hpp"
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(NUM_LEDS, RGB_LEDS_PIN, NEO_GRB + NEO_KHZ800);

void leds_init() {
  strip.begin();
  strip.setBrightness(LED_BRIGHTNESS);
  strip.show();
}

void leds_set_color(uint32_t color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void leds_set_pixel(uint8_t index, uint32_t color) {
  if (index < NUM_LEDS) {
    strip.setPixelColor(index, color);
    strip.show();
  }
}
