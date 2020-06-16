#include "select.h"
#include <stdlib.h>

select_t* selection;

void select_init(){
  selection = (select_t*)malloc(sizeof(select_t));
  (*selection) = (select_t){
    .display_function = displaytime_pixel_follow
  };

}

