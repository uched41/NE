#include "display.h"
#include "nrf_log.h"
#include <stdlib.h>
#include "nrf_delay.h"
#include "device.h"
#include "select.h"

#define RANDOM_COLOR  (crgb_t){device_state->brightness, rand()%20, rand()%20, rand()%20}

// positions for writing digits
const location_t posi1 = {0,0};
const location_t posi2 = {4,0};
const location_t posi3 = {0,6};
const location_t posi4 = {4,6};

// Display function variables
int16_t slide_horizontal_type   = 0;   // Horizontal slidein Function
int16_t slide_horizontal_delay  = 200;
int16_t slide_vertical_type     = 0;   // Vertical slidein Function
int16_t slide_vertical_delay    = 100;
int16_t horizontal_line_delay   = 50; // Horizontal line by line
int16_t randomize_delay         = 150;  // Randomize
int16_t pixel_follow_delay      = 30;  // Pixel follow delay
int16_t circular_delay          = 10;  // Circular Function
int16_t stackdown_delay         = 50;  // stackdown Function

display_t mdisplay;

void display_init(display_t* display){
  NRF_LOG_INFO("Initializing display");
  display->led_driver = (led_driver_t*)malloc(sizeof(led_driver_t)); 
  display->display_state = DISPLAY_NONE;
  memset(display->ltemplate, 0, sizeof(display->ltemplate)/sizeof(display->ltemplate[0]));
  led_driver_init(display->led_driver, 26, 25);
}

void display_set_time(display_t* display, location_t p1, location_t p2, location_t p3, location_t p4){
  write_digit(display->led_driver->_buffer, display->ltemplate, p1, device_state->time_hour/10, device_config->hour_color, device_config->bck_color);
  write_digit(display->led_driver->_buffer, display->ltemplate, p2, device_state->time_hour%10, device_config->hour_color, device_config->bck_color);
  write_digit(display->led_driver->_buffer, display->ltemplate, p3, device_state->time_mins/10, device_config->mins_color, device_config->bck_color);
  write_digit(display->led_driver->_buffer, display->ltemplate, p4, device_state->time_mins%10, device_config->mins_color, device_config->bck_color);
}

void display_show(display_t* display){
  leds_show(display->led_driver->_buffer);
}

void display_loop(display_t* display){
  switch(device_state->device_mode){
    case DEVICE_WATCH:
      display_watch_loop(display);
      break;
      
    default:
      break;
  }
}

void display_watch_loop(display_t* display){
  if(!device_is_state_changed())  // Only proceed if something has changed
    return;

  switch(device_state->accel_current_state){
    case ACCEL_READY:
      display->display_state = DISPLAY_SHOWN;
      (*(selection->display_function))(display);
      break;

    case ACCEL_NOT_READY:
      display->display_state = DISPLAY_CLEARED;
      display_clear(display);
      break;

    default:
      break;
  }
}


/*
 * Display Functions 
 */
void display_clear(display_t* display){
  leds_clear1(display->led_driver->_buffer);
}

void displaytime_just_show(display_t* display){
  display_set_time(display, posi1, posi2, posi3, posi4);
  leds_show(display->led_driver->_buffer);
}

void displaytime_horiontal_linebyline(display_t* display){
  crgb_t tempBuf[LEDS_COUNT];
  display_set_time(display, posi1, posi2, posi3, posi4);
  leds_clear2(tempBuf);
  for(uint8_t i=0; i<LEDS_LENGTH; i++){
    memcpy(tempBuf+(i*LEDS_WIDTH), display->led_driver->_buffer+(i*LEDS_WIDTH), sizeof(crgb_t)*LEDS_WIDTH);
    leds_show(tempBuf);
    nrf_delay_ms(horizontal_line_delay);
  }
}

void displaytime_randomize(display_t* display){
  crgb_t tempBuf[LEDS_COUNT];
  leds_clear2(tempBuf);
  for(uint8_t i=0; i<5; i++){
    for(uint8_t j=0; j<LEDS_COUNT; j++){
      tempBuf[j] = RANDOM_COLOR;
    }
    leds_show(tempBuf);
    nrf_delay_ms(randomize_delay);
  }
  displaytime_just_show(display);
}

void displaytime_pixel_follow(display_t* display){
  crgb_t tempBuf[LEDS_COUNT];
  display_set_time(display, posi1, posi2, posi3, posi4);
  leds_clear2(tempBuf);
  for(uint8_t i=0; i<LEDS_COUNT; i++){
    memcpy(tempBuf+(i), display->led_driver->_buffer+(i), sizeof(crgb_t)*(1+i));
    leds_show(tempBuf);
    nrf_delay_ms(pixel_follow_delay);
  }
}

void displaytime_slidein_horizontal(display_t* display){
  for(int8_t i = (slide_horizontal_type==0) ? -3 :-6; i<1; i++){    // For first case, we to iterate only 3 times
    leds_clear2(display->led_driver->_buffer);
   
    switch(slide_horizontal_type){
      case 0:
        display_set_time(display, (location_t){i+posi1.x, posi1.y}, (location_t){posi2.x-i, posi2.y}, (location_t){i+posi3.x, posi3.y}, (location_t){posi4.x-i, posi4.y});
        break;
      case 1:
        display_set_time(display, (location_t){i+posi1.x, posi1.y}, (location_t){i+posi2.x, posi2.y}, (location_t){i+posi3.x, posi3.y}, (location_t){i+posi4.x, posi4.y});
        break;
      case 2:
        display_set_time(display, (location_t){i+posi1.x, posi1.y}, (location_t){i+posi2.x, posi2.y}, (location_t){-i+posi3.x, posi3.y}, (location_t){-i+posi4.x, posi4.y});
        break;
      case 3:
        break;
      default:
        break;
    }
    display_show(display);
    nrf_delay_ms(slide_horizontal_delay);
  }
}

void displaytime_slidein_vertical(display_t* display){
  for(int8_t i = (slide_vertical_type==0) ? -5 :-10; i<1; i++){    // For first case, we to iterate only 3 times
    leds_clear2(display->led_driver->_buffer);
   
    switch(slide_vertical_type){
      case 0:
        display_set_time(display, (location_t){posi1.x, i+posi1.y}, (location_t){posi2.x, i+posi2.y}, (location_t){posi3.x, posi3.y-i}, (location_t){posi4.x, posi4.y-i});
        break;
      case 1:
        display_set_time(display, (location_t){posi1.x, i+posi1.y}, (location_t){posi2.x, i+posi2.y}, (location_t){posi3.x, i+posi3.y}, (location_t){posi4.x, i+posi4.y});
        break;
      case 2:
        display_set_time(display, (location_t){posi1.x, posi1.y-i}, (location_t){posi2.x, posi2.y-i}, (location_t){posi3.x, posi3.y-i}, (location_t){posi4.x, posi4.y-i});
        break;
      case 3:
        break;
      default:
        break;
    }
    display_show(display);
    nrf_delay_ms(slide_vertical_delay);
  }
}

void displaytime_circular(display_t* display){
  crgb_t tempBuf[LEDS_COUNT];
  display_set_time(display, posi1, posi2, posi3, posi4);
  leds_clear2(tempBuf);

  int8_t middle_x = 3;
  int8_t middle_y = 5;
  
  for(int8_t i=0; i<6; i++){
    int8_t spiraln = i+1;

    // Iterate to right
    int8_t indr = loc2index( ((location_t){middle_x+i, middle_y}) ) ;
    tempBuf[indr] = display->led_driver->_buffer[indr];
    leds_show(tempBuf);
    nrf_delay_ms(circular_delay);

    // Iterate to up
    if(i<4){
      for(int8_t k=0; k<spiraln; k++){
        int8_t indt = loc2index( ((location_t){middle_x+i, middle_y-k}) ) ;
        tempBuf[indt] = display->led_driver->_buffer[indt];
        leds_show(tempBuf);
        nrf_delay_ms(circular_delay);
      }
    }

    // Iterate to left
    for(int8_t k=0; k<(spiraln-1)*2; k++){
      int8_t indl = loc2index( ((location_t){middle_x+i-k, middle_y-i}) ) ;
      tempBuf[indl] = display->led_driver->_buffer[indl];
      leds_show(tempBuf);
      nrf_delay_ms(circular_delay);
    }

    // Iterate to down
    if(i<4){
      for(int8_t k=0; k<(spiraln-1)*2; k++){
        int8_t ind = loc2index( ((location_t){middle_x-i, middle_y-i+k}) ) ;
        tempBuf[ind] = display->led_driver->_buffer[ind];
        leds_show(tempBuf);
        nrf_delay_ms(circular_delay);
      }
    }
    
    // Iterate to right
    for(int8_t k=0; k<(spiraln-1)*2; k++){
      int8_t ind = loc2index( ((location_t){middle_x-i+k, middle_y+i}) ) ;
      tempBuf[ind] = display->led_driver->_buffer[ind];
      leds_show(tempBuf);
      nrf_delay_ms(circular_delay);
    }

    // Iterate to up
    if(i<4){
       for(int8_t k=0; k<(spiraln); k++){
        int8_t ind = loc2index( ((location_t){middle_x+i, middle_y+i-k}) ) ;
        tempBuf[ind] = display->led_driver->_buffer[ind];
        leds_show(tempBuf);
        nrf_delay_ms(circular_delay);
      }
    }
   
  }
}

void displaytime_stackdown(display_t* display){
  crgb_t tempBuf[LEDS_COUNT];
  crgb_t blackRow[LEDS_WIDTH] = {BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};
  display_set_time(display, posi1, posi2, posi3, posi4);
  leds_clear2(tempBuf);

  for(int8_t i=LEDS_LENGTH-1; i>-1; i--){
    if(i==5) continue;
    for(int8_t j=0; j<i+1; j++){
      memcpy(tempBuf+(j*LEDS_WIDTH), display->led_driver->_buffer+(i*LEDS_WIDTH), sizeof(crgb_t)*LEDS_WIDTH);
      if(j%2) leds_reverse_array(tempBuf+(j*LEDS_WIDTH), LEDS_WIDTH);
      leds_show(tempBuf);
      delay(stackdown_delay);
      memcpy(tempBuf+(j*LEDS_WIDTH), blackRow, sizeof(crgb_t)*LEDS_WIDTH);
    }
    memcpy(tempBuf+(i*LEDS_WIDTH), display->led_driver->_buffer+(i*LEDS_WIDTH), sizeof(crgb_t)*LEDS_WIDTH);
  }
}