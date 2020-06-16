#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include "led_driver.h"
#include "accel_wrapper.h"

typedef enum{
  DEVICE_NONE,
  DEVICE_WATCH
}device_mode_t;

typedef struct {
  int8_t time_hour;
  int8_t time_mins;
  uint8_t brightness;
  accel_state_t accel_current_state;
  accel_state_t accel_previous_state;
  device_mode_t device_mode;
}device_state_t;

extern device_state_t* device_state;

typedef struct {
  crgb_t hour_color;
  crgb_t mins_color;
  crgb_t bck_color;
}device_config_t;

extern device_config_t* device_config;

#define BLACK         (crgb_t){device_state->brightness, 0, 0, 0}
#define RED           (crgb_t){device_state->brightness, 0, 20, 0}

void device_init();
uint8_t device_is_state_changed();

static inline void device_accel_update(accel_state_t state){
  device_state->accel_previous_state = device_state->accel_current_state;
  device_state->accel_current_state = state;
}


#endif 

