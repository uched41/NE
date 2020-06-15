#include "nrf_log.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"

#include "hardware.h"
#include "fram_driver.h"
#include "utils.h"

#define fram_cs_low nrf_drv_gpiote_out_clear(FRAM_CS)
#define fram_cs_high  nrf_drv_gpiote_out_set(FRAM_CS)

void init_fram(void){
    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);
    APP_ERROR_CHECK(nrf_drv_gpiote_out_init(FRAM_CS, &out_config));
    fram_cs_high;
    nrf_delay_ms(1); 
}

static void fram_write_enable(){
    fram_cs_low;
    uint8_t cmd = FM25V10_CMD_WREN;
    nrf_drv_spi_transfer(&m_spi, &cmd, 1, NULL, 0); 
    fram_cs_high;
}

void fram_data_write(uint32_t address, uint8_t* data, uint8_t data_length){
    if(!data) return;

    fram_write_enable();

    fram_cs_low;
    uint8_t buf[data_length+4];
    buf[0] = FM25V10_CMD_WRITE;
    buf[1] = (uint8_t)( (address & 0x00010000) >> 16 );
    buf[2] = (uint8_t)( (address & 0x0000ff00) >> 8 );
    buf[3] = (uint8_t)( (address & 0x000000ff) );
    memcpy(buf+4, data, data_length);
    //NRF_LOG_HEXDUMP_INFO(buf, data_length+4);
    nrf_drv_spi_transfer(&m_spi, buf, data_length+4, NULL, 0);

    fram_cs_high;
}

void fram_data_read(uint32_t address, uint8_t* data, uint8_t data_length){
    if(!data) return;
    fram_cs_low;

    uint8_t buf[4];
    buf[0] = FM25V10_CMD_READ;
    buf[1] = (uint8_t)( (address & 0x00010000) >> 16 );
    buf[2] = (uint8_t)( (address & 0x0000ff00) >> 8 );
    buf[3] = (uint8_t)( (address & 0x000000ff) );

    uint8_t rbuf[data_length+4];

    nrf_drv_spi_transfer(&m_spi, buf, 4, rbuf, data_length+4);
    memcpy(data, rbuf+4, data_length);

    fram_cs_high;
}

void fram_byte_write(uint32_t address, uint8_t data){
    fram_write_enable();

    fram_cs_low;

    uint8_t buf[5];
    buf[0] = FM25V10_CMD_WRITE;
    buf[1] = (uint8_t)( (address & 0x00ff0000) >> 16 );
    buf[2] = (uint8_t)( (address & 0x0000ff00) >> 8 );
    buf[3] = (uint8_t)( (address & 0x000000ff) );
    buf[4] = (uint8_t)data;

    nrf_drv_spi_transfer(&m_spi, buf, 5, NULL, 0);

    fram_cs_high;
}

uint8_t fram_byte_read(uint32_t address){
    fram_cs_low;
    uint8_t buf[4];
    buf[0] = FM25V10_CMD_READ;
    buf[1] = (uint8_t)( (address & 0x00ff0000) >> 16 );
    buf[2] = (uint8_t)( (address & 0x0000ff00) >> 8 );
    buf[3] = (uint8_t)( (address & 0x000000ff) );

    uint8_t data[5];
    nrf_drv_spi_transfer(&m_spi, buf, 4, data, 5);

    fram_cs_high;
    return data[4];
}

void fram_sleep(void){
    fram_cs_low;

    uint8_t buf[1];
    buf[0] = FM25V10_CMD_SLEEP;

    nrf_drv_spi_transfer(&m_spi, buf, 1, NULL, 0);

    fram_cs_high;
}

void fram_get_id(uint8_t * ID){
    fram_cs_low;

    uint8_t buf[1];
    buf[0] = FM25V10_CMD_RDID;

    uint8_t rbuf[10];
    nrf_drv_spi_transfer(&m_spi, buf, 1, ID, 10);
    memcpy(buf, rbuf+1, 9);

    fram_cs_high;
}

void test_fram(void){
    uint8_t arr[9];
    fram_get_id(arr);
    NRF_LOG_HEXDUMP_INFO(arr, 9);

    uint8_t mbuf1[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint8_t mbuf2[10];
    fram_data_write(1000, mbuf1, 10);
    fram_data_read(1000, mbuf2, 10);

    NRF_LOG_HEXDUMP_INFO(mbuf2, 10);

    fram_byte_write(1543, 8);
    uint8_t val = fram_byte_read(1543);
    NRF_LOG_HEXDUMP_INFO(&val, 1);
}