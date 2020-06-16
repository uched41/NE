#include "accel_driver.h"
#include "nrf_log.h"
#include "nrf_drv_gpiote.h"
#include <math.h>
#include "device.h"

#define ROLL_MIN  -30
#define ROLL_MAX  30
#define PITCH_MIN -40
#define PITCH_MAX 40

/* Configuration for accelerometer */
static app_mpu_config_t accel_config = MPU_DEFAULT_CONFIG();
static app_mpu_int_pin_cfg_t accel_int_config = MPU_DEFAULT_INT_PIN_CONFIG();
static app_mpu_int_enable_t accel_int_enable = MPU_DEFAULT_INT_ENABLE_CONFIG();

gyro_values_t gyro_vals;
accel_values_t accel_vals;

// Callback for accelerometer interrupt
void accel_cb(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
   NRF_LOG_INFO("acceleromer triggered \n");
}

void accel_loop(){
  app_mpu_read_accel(&accel_vals);
  double X = accel_vals.x;// / 2048;    // No need to divide, calculation remains the same
  double Y = accel_vals.y;// / 2048;
  double Z = accel_vals.z;// / 2048;
  double roll  = atan2( Y, Z) * 180 / M_PI;
  double pitch = atan2( -X, sqrt(Y*Y + Z*Z)) * 180 / M_PI;
  //NRF_LOG_INFO("ax: %d\t ay: %d\t az: %d\t", accel_vals.x, accel_vals.y, accel_vals.z);
  //NRF_LOG_INFO("roll: %d\t pitch: %d\t", (int16_t)roll, (int16_t)pitch);

  if( ((int16_t)roll > ROLL_MIN) && 
      ((int16_t)roll < ROLL_MAX) && 
      ((int16_t)pitch > PITCH_MIN) && 
      ((int16_t)pitch < PITCH_MAX) ){
    NRF_LOG_INFO("Correct position, roll: %d\t pitch: %d\t", (int16_t)roll, (int16_t)pitch);
    device_accel_update(ACCEL_READY);
  }
  else{
    device_accel_update(ACCEL_NOT_READY);
  }
}


void accel_init(void){
    // Set up interrupt pin
    nrf_drv_gpiote_init();
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    APP_ERROR_CHECK(nrf_drv_gpiote_in_init(19, &in_config, accel_cb));

    nrf_drv_gpiote_in_event_enable(19, true);
            
    // hardware initialization of mpu6050
    //accel_config.accel_config.afs_sel = AFS_2G;
    //accel_config.sync_dlpf_gonfig.dlpf_cfg = 0;
    APP_ERROR_CHECK(app_mpu_init());		
    APP_ERROR_CHECK(app_mpu_config(&accel_config));		
    APP_ERROR_CHECK(app_mpu_int_cfg_pin(&accel_int_config));		
    APP_ERROR_CHECK(app_mpu_motion_detect_threshold(1));
    
    //accel_int_enable.mot_en = 1;
    //accel_int_enable.zmot_en = 1;
    APP_ERROR_CHECK(app_mpu_int_enable(&accel_int_enable));		
    NRF_LOG_INFO("Accelerometer initialization complete.")
}

