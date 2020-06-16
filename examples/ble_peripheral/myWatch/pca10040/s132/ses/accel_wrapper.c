#include "nrf_drv_gpiote.h"
#include "accel_wrapper.h"
#include "nrf_log.h"
#include <stdlib.h>
#include <math.h>
#include "device.h"
#include "hardware.h"

#define ROLL_MIN  -30
#define ROLL_MAX  30
#define PITCH_MIN -40
#define PITCH_MAX 40

AxesRaw_t accel_values;

/*
   Callback for accelerometer interrupt
*/
void accel_cb(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
   NRF_LOG_INFO("acceleromer triggered \n");
}


void accel_loop(){
  LIS3DH_GetAccAxesRaw(&accel_values);
  double X = accel_values.AXIS_X;// / 2048;    // No need to divide, calculation remains the same
  double Y = accel_values.AXIS_Y;// / 2048;
  double Z = accel_values.AXIS_Z;// / 2048;
  double roll  = atan2( Y, Z) * 180 / M_PI;
  double pitch = atan2( -X, sqrt(Y*Y + Z*Z)) * 180 / M_PI;

  NRF_LOG_INFO("ax: %d\t ay: %d\t az: %d\t", X, Y, Z);
  NRF_LOG_INFO("roll: %d\t pitch: %d\t", (int16_t)roll, (int16_t)pitch);

  /*if( ((int16_t)roll > ROLL_MIN) && 
      ((int16_t)roll < ROLL_MAX) && 
      ((int16_t)pitch > PITCH_MIN) && 
      ((int16_t)pitch < PITCH_MAX) ){
    NRF_LOG_INFO("Correct position, roll: %d\t pitch: %d\t", (int16_t)roll, (int16_t)pitch);
    device_accel_update(ACCEL_READY);
  }
  else{
    device_accel_update(ACCEL_NOT_READY);
  }*/

}

/*
  Accelerometer initialization,
*/
void accel_init(void){
    ret_code_t err_code;
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

    err_code = nrf_drv_gpiote_in_init(ACCEL_INT, &in_config, accel_cb);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(ACCEL_INT, true);
          
    uint8_t temp;
    LIS3DH_Init();

    SetLIS3DHActivityDetection(20, LIS3DH_INT_MODE_6D_POSITION, 1);  

    LIS3DH_GetWHO_AM_I(&temp);
    NRF_LOG_INFO("Accelerometer initialization complete");
}





