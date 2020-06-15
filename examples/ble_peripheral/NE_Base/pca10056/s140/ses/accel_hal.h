#ifndef _ACCEL_HAL_
#define _ACCEL_HAL_


#include <stdbool.h>
#include <stdint.h>

void nrf_drv_mpu_write_registers(uint8_t reg, uint8_t * p_data, uint32_t length);

void nrf_drv_mpu_write_single_register(uint8_t reg, uint8_t data);

void nrf_drv_mpu_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length);

#endif