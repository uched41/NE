#ifndef ACCEL_DRIVER_H
#define ACCEL_DRIVER_H

#include "app_mpu.h"

extern gyro_values_t gyro_vals;
extern accel_values_t accel_vals;

typedef enum{
  ACCEL_NOT_READY,
  ACCEL_READY
}accel_state_t;

void accel_init(void);
void accel_loop();

#endif