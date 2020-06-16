#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <stdint.h>

// structure for storing color
#define LEDS_WIDTH  7
#define LEDS_LENGTH 11
#define LEDS_COUNT  LEDS_WIDTH * LEDS_LENGTH

// convert coordiante location to index
#define loc2index(l) ( (l.y>=0 && l.y<LEDS_LENGTH && l.x>=0 && l.x<LEDS_WIDTH) ? ( ((l.y*LEDS_WIDTH)+l.x) )  : (-1) )

typedef struct {
 uint8_t br, b, g, r;
}crgb_t;

typedef struct {
 uint8_t b, g, r;
}crg_t;

// X and Y coordinate of pixel starting from 0
typedef struct {
  int8_t x, y;
}location_t;

// Class for led driver
typedef struct {
    crgb_t* _buffer;
}led_driver_t;

//extern led_driver_t* mleds;

void led_driver_init(led_driver_t* leds, uint8_t clkPin, uint8_t dataPin);
void leds_setpixel1(crgb_t* ledBuf, location_t locale, crgb_t col);
void leds_setpixel2(crgb_t* ledBuf, uint8_t ind, crgb_t col);
void leds_setbuffer(crgb_t* ledBuf, uint8_t* buf, uint16_t pixelLen);
void leds_show(crgb_t* ledBuf);
void leds_clear1(crgb_t* ledBuf);
void leds_clear2(crgb_t* ledBuf);
void leds_test(crgb_t* ledBuf);
void leds_reverse_array(crgb_t* buf, uint8_t len);

#endif