#include "nrf_log.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"

#include "utils.h"
#include "hardware.h"
#include <stdio.h>
#include <stdarg.h>

const nrf_drv_spi_t m_spi = NRF_DRV_SPI_INSTANCE(1);
volatile bool spi_rx_done = false;
volatile bool spi_tx_done = false;

const nrfx_twi_t m_twi_instance = NRFX_TWI_INSTANCE(0);
volatile static bool twi_tx_done = false;
volatile static bool twi_rx_done = false;

static void twi_event_handler(nrfx_twi_evt_t const * p_event, void * p_context){
   
}


void spi_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context){
    
}


void init_peripherals(void){
    mprintf("Init", "Initializing GPIOTE");
    APP_ERROR_CHECK(nrf_drv_gpiote_init());

    mprintf("Init", "Initializing SPI");
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.mosi_pin = MSPI_MOSI;
    spi_config.miso_pin = MSPI_MISO;
    spi_config.sck_pin  = MSPI_SCK;
    spi_config.frequency = NRF_DRV_SPI_FREQ_8M;
    APP_ERROR_CHECK(nrf_drv_spi_init(&m_spi, &spi_config, NULL, NULL));

    mprintf("Init", "Initializing I2C");
     const nrfx_twi_config_t twi_mpu_config = {
       .scl                = MSCL,
       .sda                = MSDA,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .hold_bus_uninit     = false
    };
    APP_ERROR_CHECK(nrfx_twi_init(&m_twi_instance, &twi_mpu_config, NULL, NULL));    
    nrfx_twi_enable(&m_twi_instance);
}


void mprintf(const char* head, const char* fmt, ...){
  va_list args;
  va_start (args, fmt);
  printf("<%s>: ", head);
  vprintf(fmt, args);
  va_end(args);
}
