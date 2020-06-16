#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>
#include "led_driver.h"

void write_digit(crgb_t* ledBuf, uint8_t* mtemplate, location_t start_loc, uint8_t digit, crgb_t col, crgb_t bckcol);

#endif