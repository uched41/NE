#include "nrf_log.h"
#include "nrf_drv_gpiote.h"
#include <nrfx_twi.h>
#include "nrf_delay.h"

#include "hardware.h"
#include "i2c_multi_driver.h"
#include "utils.h"

#define I2C_ADDR 0X70

static volatile uint8_t current_sensor = 255;

static uint8_t twi_tx_buffer2[64];

static void merge_register_and_data(uint8_t * new_buffer, uint8_t reg, uint8_t * p_data, uint32_t length)
{
    new_buffer[0] = reg;
    memcpy((new_buffer + 1), p_data, length);
}

void i2c_sensor_select(i2c_sensor_t* sensor){
    uint32_t err_code;
    uint8_t in = 1 << (sensor->channel_no);

    err_code = nrfx_twi_tx(&m_twi_instance, I2C_ADDR, &in, 1, false);
    //NRF_LOG_ERROR("ERROR %u [%s]", err_code, nrf_strerror_get(err_code));
    APP_ERROR_CHECK(err_code);

    current_sensor = sensor->channel_no;
}


uint8_t i2c_sensor_read_byte(i2c_sensor_t* sensor, uint8_t reg){
    uint32_t err_code;
    uint8_t val;

    if(current_sensor != sensor->channel_no){
      i2c_sensor_select(sensor);
    }

    err_code = nrfx_twi_tx(&m_twi_instance, sensor->addr, &reg, 1, false);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_twi_rx(&m_twi_instance, sensor->addr, &val, 1);
    APP_ERROR_CHECK(err_code);

    return val;
}

void i2c_sensor_read(i2c_sensor_t* sensor, uint8_t reg, uint8_t* data, uint8_t len){
    uint32_t err_code;

    if(current_sensor != sensor->channel_no){
      i2c_sensor_select(sensor);
    }

    err_code = nrfx_twi_tx(&m_twi_instance, sensor->addr, &reg, 1, false);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_twi_rx(&m_twi_instance, sensor->addr, data, len);
    APP_ERROR_CHECK(err_code);
}

void i2c_sensor_write_byte(i2c_sensor_t* sensor, uint8_t reg, uint8_t data){
    uint32_t err_code;
    
    if(current_sensor != sensor->channel_no){
      i2c_sensor_select(sensor);
    }

    uint8_t packet[2] = {reg, data};

    err_code = nrfx_twi_tx(&m_twi_instance, sensor->addr, packet, 2, false);
    APP_ERROR_CHECK(err_code);
}

void i2c_sensor_write(i2c_sensor_t* sensor, uint8_t reg, uint8_t* data, uint8_t len){
    uint32_t err_code;

    if(current_sensor != sensor->channel_no){
      i2c_sensor_select(sensor);
    }

    // Merging MPU register address and p_data into one buffer.
    merge_register_and_data(twi_tx_buffer2, reg, data, len);

    /*err_code = nrfx_twi_tx(&m_twi_instance, sensor->addr, &reg, 1, false);
    if(err_code != NRF_SUCCESS){
      NRF_LOG_ERROR("ERROR %u [%s]", err_code, nrf_strerror_get(err_code));
    }
    //APP_ERROR_CHECK(err_code);

    err_code = nrfx_twi_tx(&m_twi_instance, sensor->addr, data, len, false);*/

    err_code = nrfx_twi_tx(&m_twi_instance, sensor->addr, twi_tx_buffer2, len+1, false);

    if(err_code != NRF_SUCCESS){
      NRF_LOG_ERROR("ERROR %u [%s]", err_code, nrf_strerror_get(err_code));
    }
    APP_ERROR_CHECK(err_code);
    //nrf_delay_ms(10);
}