#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <nrfx_twi.h>
#include "app_util_platform.h"
#include "accel_reg.h"
#include "accel_hal.h"
#include "utils.h"
#include "nrf_log.h"

#define MPU_TWI_BUFFER_SIZE     	14 
#define MPU_TWI_TIMEOUT 		10000 
#define MPU_ADDRESS     		ICM20600_I2C_ADDR1

uint8_t twi_tx_buffer[MPU_TWI_BUFFER_SIZE];

static void merge_register_and_data(uint8_t * new_buffer, uint8_t reg, uint8_t * p_data, uint32_t length)
{
    new_buffer[0] = reg;
    memcpy((new_buffer + 1), p_data, length);
}


void nrf_drv_mpu_write_registers(uint8_t reg, uint8_t * p_data, uint32_t length)
{
    uint32_t err_code;
    uint32_t timeout = MPU_TWI_TIMEOUT;

    // Merging MPU register address and p_data into one buffer.
    merge_register_and_data(twi_tx_buffer, reg, p_data, length);

    err_code = nrfx_twi_tx(&m_twi_instance, MPU_ADDRESS, twi_tx_buffer, length + 1, false);

    APP_ERROR_CHECK(err_code);
}

void nrf_drv_mpu_write_single_register(uint8_t reg, uint8_t data)
{
    uint32_t err_code;
    
    uint8_t packet[2] = {reg, data};

    err_code = nrfx_twi_tx(&m_twi_instance, MPU_ADDRESS, packet, 2, false);
    //NRF_LOG_ERROR("ERROR %u [%s]", err_code, nrf_strerror_get(err_code));
    APP_ERROR_CHECK(err_code);
}


void nrf_drv_mpu_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length)
{
    uint32_t err_code;
    uint32_t timeout = MPU_TWI_TIMEOUT;

    err_code = nrfx_twi_tx(&m_twi_instance, MPU_ADDRESS, &reg, 1, false);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_twi_rx(&m_twi_instance, MPU_ADDRESS, p_data, length);
    APP_ERROR_CHECK(err_code);
}
