#ifndef SELECT_H
#define SELECT_H

#include "display.h"

typedef struct {
  DISPLAY_FUNCTION display_function;
}select_t;

extern select_t* selection;

void select_init();

#endif