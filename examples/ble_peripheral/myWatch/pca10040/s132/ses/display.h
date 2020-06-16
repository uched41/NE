#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "led_driver.h"
#include "text.h"

typedef enum{
  DISPLAY_NONE,
  DISPLAY_SHOWN,
  DISPLAY_CLEARED
}display_state_t;

typedef struct {
    led_driver_t* led_driver;
    uint8_t ltemplate[LEDS_COUNT];
    display_state_t display_state;
}display_t;

extern display_t mdisplay;

void display_init(display_t* display);
void display_set_time(display_t* display, location_t p1, location_t p2, location_t p3, location_t p4);
void display_show(display_t* display);
void display_loop(display_t* display);
void display_watch_loop(display_t* display);

// Display Functions
typedef void (*DISPLAY_FUNCTION) (display_t* display);    // Function pointer definition
void display_clear(display_t* display);
void displaytime_just_show(display_t* display);
void displaytime_horiontal_linebyline(display_t* display);
void displaytime_randomize(display_t* display);
void displaytime_pixel_follow(display_t* display);
void displaytime_slidein_horizontal(display_t* display);
void displaytime_slidein_vertical(display_t* display);
void displaytime_circular(display_t* display);
void displaytime_stackdown(display_t* display);

/*
 Display function configs
*/
extern int16_t slide_horizontal_type;   // Horizontal slidein Function
extern int16_t slide_horizontal_delay;

extern int16_t slide_vertical_type;     // Vertical slidein Function
extern int16_t slide_vertical_delay;

extern int16_t horizontal_line_delay;   // Horizontal line by line

extern int16_t randomize_delay;         // Randomize

extern int16_t pixel_follow_delay;      // Pixel follow delay

extern int16_t circular_delay;          // Circular Function

extern int16_t stackdown_delay;         // Stackdown delay

#endif


