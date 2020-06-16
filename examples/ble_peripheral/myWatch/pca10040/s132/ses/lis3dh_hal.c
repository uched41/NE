#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <nrfx_twi.h>
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "lis3dh_hal.h"
#include "lis3dh_reg.h"
#include "hardware.h"

/* Pins to connect MPU. Pinout is different for nRF51 DK and nRF52 DK
 * and therefore I have added a conditional statement defining different pins
 * for each board. This is only for my own convenience. 
 */
#define MPU_TWI_SCL_PIN ACCEL_SCL_PIN
#define MPU_TWI_SDA_PIN ACCEL_SDA_PIN


#define MPU_TWI_BUFFER_SIZE     	14 // 14 byte buffers will suffice to read acceleromter, gyroscope and temperature data in one transmission.
#define MPU_TWI_TIMEOUT 		10000 
#define MPU_ADDRESS     		LIS3DH_MEMS_I2C_ADDRESS


static const nrfx_twi_t m_twi_instance = NRFX_TWI_INSTANCE(0);
volatile static bool twi_tx_done = false;
volatile static bool twi_rx_done = false;

uint8_t twi_tx_buffer[MPU_TWI_BUFFER_SIZE];

static void nrf_drv_mpu_twi_event_handler(nrfx_twi_evt_t const * p_event, void * p_context)
{
    switch(p_event->type)
    {
        case NRFX_TWI_EVT_DONE:
            switch(p_event->xfer_desc.type)
            {
                case NRFX_TWI_XFER_TX:
                    twi_tx_done = true;
                    break;
                case NRFX_TWI_XFER_TXTX:
                    twi_tx_done = true;
                    break;
                case NRFX_TWI_XFER_RX:
                    twi_rx_done = true;
                    break;
                case NRFX_TWI_XFER_TXRX:
                    twi_rx_done = true;
                    break;
                default:
                    break;
            }
            break;
        case NRFX_TWI_EVT_ADDRESS_NACK:
            break;
        case NRFX_TWI_EVT_DATA_NACK:
            break;
        default:
            break;
    }
}



/**
 * @brief TWI initialization.
 * Just the usual way. Nothing special here
 */
uint32_t nrf_drv_mpu_init(void)
{
    uint32_t err_code;
    
    const nrfx_twi_config_t twi_mpu_config = {
       .scl                = MPU_TWI_SCL_PIN,
       .sda                = MPU_TWI_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGHEST,
       .hold_bus_uninit     = false
    };
    
    err_code = nrfx_twi_init(&m_twi_instance, &twi_mpu_config, nrf_drv_mpu_twi_event_handler, NULL);
    if(err_code != NRF_SUCCESS)
	{
		return err_code;
	}
    
    nrfx_twi_enable(&m_twi_instance);
	
	return NRF_SUCCESS;
}




// The TWI driver is not able to do two transmits without repeating the ADDRESS + Write bit byte
// Hence we need to merge the MPU register address with the buffer and then transmit all as one transmission
static void merge_register_and_data(uint8_t * new_buffer, uint8_t reg, uint8_t * p_data, uint32_t length)
{
    new_buffer[0] = reg;
    memcpy((new_buffer + 1), p_data, length);
}


uint32_t nrf_drv_mpu_write_registers(uint8_t reg, uint8_t * p_data, uint32_t length)
{
    // This burst write function is not optimal and needs improvement.
    // The new SDK 11 TWI driver is not able to do two transmits without repeating the ADDRESS + Write bit byte
    uint32_t err_code;
    uint32_t timeout = MPU_TWI_TIMEOUT;

    // Merging MPU register address and p_data into one buffer.
    merge_register_and_data(twi_tx_buffer, reg, p_data, length);

    // Setting up transfer
    nrfx_twi_xfer_desc_t xfer_desc;
    xfer_desc.address = MPU_ADDRESS;
    xfer_desc.type = NRFX_TWI_XFER_TX;
    xfer_desc.primary_length = length + 1;
    xfer_desc.p_primary_buf = twi_tx_buffer;

    // Transferring
    err_code = nrfx_twi_xfer(&m_twi_instance, &xfer_desc, 0);

    while((!twi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    twi_tx_done = false;

    return err_code;
}

uint32_t nrf_drv_mpu_write_single_register(uint8_t reg, uint8_t data)
{
    uint32_t err_code;
    uint32_t timeout = MPU_TWI_TIMEOUT;

    uint8_t packet[2] = {reg, data};

    err_code = nrfx_twi_tx(&m_twi_instance, MPU_ADDRESS, packet, 2, false);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!twi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;

    twi_tx_done = false;

    return err_code;
}


uint32_t nrf_drv_mpu_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length)
{
    uint32_t err_code;
    uint32_t timeout = MPU_TWI_TIMEOUT;

    err_code = nrfx_twi_tx(&m_twi_instance, MPU_ADDRESS, &reg, 1, false);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!twi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    twi_tx_done = false;

    err_code = nrfx_twi_rx(&m_twi_instance, MPU_ADDRESS, p_data, length);
    if(err_code != NRF_SUCCESS) return err_code;

    timeout = MPU_TWI_TIMEOUT;
    while((!twi_rx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    twi_rx_done = false;

    return err_code;
}
