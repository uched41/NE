#ifndef	ACCCEL_WRAPPER_H
#define	ACCCEL_WRAPPER_H

#include <stdbool.h>
#include "lis3dh.h"

extern  AxesRaw_t accel_values;

typedef enum{
  ACCEL_NOT_READY,
  ACCEL_READY
}accel_state_t;

void accel_init(void);
void accel_loop();


#endif
