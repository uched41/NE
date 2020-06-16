#include "utils.h"
#include "nrf_log.h"
#include <stdlib.h>
#include "nrf_delay.h"
#include "nrf_drv_rng.h"

void utils_init(){
  NRF_LOG_INFO("Initializing Utils.");
  nrf_delay_ms(100);

  // Delay above gives enough time for random number to be generated by the drivers
  APP_ERROR_CHECK(nrf_drv_rng_init(NULL));
  uint8_t p_buff[2];
  APP_ERROR_CHECK(nrf_drv_rng_rand(p_buff, 2));

  srand(p_buff[1]); 
}