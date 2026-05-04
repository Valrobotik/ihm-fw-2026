#pragma once
#include <stdint.h>

void leds_init();
void leds_set_color(uint32_t color);
void leds_set_pixel(uint8_t index, uint32_t color);
