#ifndef ACCEL_DRIVER_H
#define ACCEL_DRIVER_H

// Gyroscope scale range
typedef enum  {
    RANGE_250_DPS = 0,
    RANGE_500_DPS,
    RANGE_1K_DPS,
    RANGE_2K_DPS,
}gyro_scale_type_t;

// Accelerometer scale range
typedef enum  {
    RANGE_2G = 0,
    RANGE_4G,
    RANGE_8G,
    RANGE_16G,
}acc_scale_type_t;

// Gyroscope output data rate
typedef enum  {
    GYRO_RATE_8K_BW_3281 = 0,
    GYRO_RATE_8K_BW_250,
    GYRO_RATE_1K_BW_176,
    GYRO_RATE_1K_BW_92,
    GYRO_RATE_1K_BW_41,
    GYRO_RATE_1K_BW_20,
    GYRO_RATE_1K_BW_10,
    GYRO_RATE_1K_BW_5,
}gyro_lownoise_odr_type_t;

// Accelerometer output data rate
typedef enum  {
    ACC_RATE_4K_BW_1046 = 0,
    ACC_RATE_1K_BW_420,
    ACC_RATE_1K_BW_218,
    ACC_RATE_1K_BW_99,
    ACC_RATE_1K_BW_44,
    ACC_RATE_1K_BW_21,
    ACC_RATE_1K_BW_10,
    ACC_RATE_1K_BW_5,
}acc_lownoise_odr_type_t;

// Averaging filter settings for Low Power Accelerometer mode
typedef enum  {
    ACC_AVERAGE_4 = 0,
    ACC_AVERAGE_8,
    ACC_AVERAGE_16,
    ACC_AVERAGE_32,
}acc_averaging_sample_type_t;

// Averaging filter configuration for low-power gyroscope mode
typedef enum  {
    GYRO_AVERAGE_1 = 0,
    GYRO_AVERAGE_2,
    GYRO_AVERAGE_4,
    GYRO_AVERAGE_8,
    GYRO_AVERAGE_16,
    GYRO_AVERAGE_32,
    GYRO_AVERAGE_64,
    GYRO_AVERAGE_128,
}gyro_averaging_sample_type_t;

// ICM20600 power mode
typedef enum  {
    ICM_SLEEP_MODE = 0,
    ICM_STANDYBY_MODE,
    ICM_ACC_LOW_POWER,
    ICM_ACC_LOW_NOISE,
    ICM_GYRO_LOW_POWER,
    ICM_GYRO_LOW_NOISE,
    ICM_6AXIS_LOW_POWER,
    ICM_6AXIS_LOW_NOISE,
}icm20600_power_type_t;

typedef struct{
  int16_t x, y, z;
}accel_values_t;

typedef struct{
  int16_t x, y, z;
}gyro_values_t;

void init_accel();

uint8_t accel_get_device_id();

void accel_reset();

void get_accel_values(accel_values_t* val);

void get_gyro_values(gyro_values_t* val);

void test_accel();

#endif