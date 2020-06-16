#include "device.h"
#include "select.h"
#include <stdlib.h>

#define FULL_BRIGHTNESS 0b11111111
#define HALF_BRIGHTNESS 0b11110000
#define LOW_BRIGHTNESS  0b11100010
#define VLOW_BRIGHTNESS 0b11100001

device_state_t* device_state;
device_config_t* device_config;

void device_init(){
  select_init();  // Initialize select settings
  device_state = (device_state_t*)malloc(sizeof(device_state_t));
  device_config = (device_config_t*)malloc(sizeof(device_config_t));

  (*device_state) = (device_state_t){
      .brightness = LOW_BRIGHTNESS,
      .time_hour  = 23,
      .time_mins  = 46,
      .accel_current_state = ACCEL_NOT_READY,
      .accel_previous_state = ACCEL_NOT_READY,
      .device_mode = DEVICE_WATCH,
  };
  
  (*device_config) = (device_config_t){
      .hour_color = (crgb_t){device_state->brightness, 0, 0, 20},
      .mins_color = (crgb_t){device_state->brightness, 0, 20, 20},
      .bck_color  = (crgb_t){device_state->brightness, 0, 0, 0},
  };
}

uint8_t device_is_state_changed(){
  return (device_state->accel_current_state != device_state->accel_previous_state) ? 1 : 0 ;
}