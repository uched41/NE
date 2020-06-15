#include "hardware.h"

#define BSP_QSPI_IO0_PIN QSPI_I00
#define BSP_QSPI_IO1_PIN QSPI_I01
#define BSP_QSPI_IO2_PIN QSPI_I02
#define BSP_QSPI_IO3_PIN QSPI_I03
#define BSP_QSPI_SCK_PIN QSPI_CLK
#define BSP_QSPI_CSN_PIN QSPI_CSN

#include "nrf_log.h"
#include <string.h>
#include <stdlib.h>


#include "flash_driver.h"
#include "fs_main.h"

#define QSPI_TEST_DATA_SIZE PAGE_SIZE

volatile bool m_finished = false;
static uint8_t m_buffer_tx[QSPI_TEST_DATA_SIZE];
static uint8_t m_buffer_rx[QSPI_TEST_DATA_SIZE];

static int mcmp(uint8_t* b1, uint8_t* b2, int len){
  for(int i=0; i<len; i++){
    if(b1[i] != b2[i]) return i;
  }
  return 0;
}


static void qspi_handler(nrf_drv_qspi_evt_t event, void * p_context)
{
    UNUSED_PARAMETER(event);
    UNUSED_PARAMETER(p_context);

    m_finished = true;
}

static void configure_memory(){
    uint32_t err_code;
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = ENABLE_FLASH_RESET,
        .length    = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = true,
        .io3_level = true,
        .wipwait   = true,
        .wren      = true
    };

    // Send reset enable
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Send reset command
    cinstr_cfg.opcode = FLASH_RESET;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Write enable
    flash_write_enable();

    // Switch to qspi mode
    uint8_t qe_enable = 0x02;
    cinstr_cfg.opcode = WRITE_STATUS_REGISTER_2;
    cinstr_cfg.length = NRF_QSPI_CINSTR_LEN_2B;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, &qe_enable, NULL);
    APP_ERROR_CHECK(err_code);
}


void init_flash(void){
    uint32_t err_code;
    mprintf("Flash", "Initializing QSPI\n");
    nrf_drv_qspi_config_t qconfig = NRF_DRV_QSPI_DEFAULT_CONFIG;
    qconfig.pins.io0_pin = QSPI_I00;
    qconfig.pins.io1_pin = QSPI_I01;
    qconfig.pins.io2_pin = QSPI_I02;
    qconfig.pins.io3_pin = QSPI_I03;
    qconfig.pins.sck_pin = QSPI_CLK;
    qconfig.pins.csn_pin = QSPI_CSN;

    qconfig.irq_priority = 2;

    err_code = nrf_drv_qspi_init(&qconfig, qspi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    configure_memory();

    m_finished = false;
}

void flash_write_enable(void){
    uint32_t err_code;
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = NULL,
        .length    = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = true,
        .io3_level = true,
        .wipwait   = true,
        .wren      = true
    };

    // Write enable
    cinstr_cfg.opcode = WRITE_ENABLE;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);
}


void flash_write_page(uint32_t page_addr, uint8_t* buffer){
    uint32_t err_code;
    m_finished = false;
    err_code = nrf_drv_qspi_write(buffer, PAGE_SIZE, page_addr);
    APP_ERROR_CHECK(err_code);
}

void flash_write_page_blocking(uint32_t page_addr, uint8_t* buffer){
    uint32_t err_code;
    err_code = nrf_drv_qspi_write(buffer, PAGE_SIZE, page_addr);
    APP_ERROR_CHECK(err_code);
    WAIT_FOR_QSPI();
}

void flash_read_page(uint32_t page_addr, uint8_t* buffer){
    uint32_t err_code;
    qspi_busy;
    err_code = nrf_drv_qspi_read(buffer, PAGE_SIZE, page_addr);
    APP_ERROR_CHECK(err_code);
    WAIT_FOR_QSPI();
}

void flash_erase_block_4kb(uint32_t addr){
    uint32_t err_code;
    err_code = nrf_drv_qspi_erase(QSPI_ERASE_LEN_LEN_4KB, addr);
    APP_ERROR_CHECK(err_code);
    WAIT_FOR_QSPI();
}

void flash_erase_block_64kb(uint32_t addr){
    uint32_t err_code;
    err_code = nrf_drv_qspi_erase(NRF_QSPI_ERASE_LEN_64KB, addr);
    APP_ERROR_CHECK(err_code);
    WAIT_FOR_QSPI();
}

void flash_erase_chip(){
    uint32_t err_code;
    err_code = nrf_drv_qspi_erase(QSPI_ERASE_LEN_LEN_All, 0);
    APP_ERROR_CHECK(err_code);
    WAIT_FOR_QSPI();
}

void test_flash(void){
    uint32_t err_code;
    srand(9);
    memset(m_buffer_rx, 0, QSPI_TEST_DATA_SIZE);

    for (uint16_t i = 0; i < QSPI_TEST_DATA_SIZE; ++i){
        m_buffer_tx[i] = (uint8_t)rand();
    }

    uint32_t addr = generate_page_addr(8,180);

    flash_erase_block_64kb(addr);

    flash_write_page_blocking(addr, m_buffer_tx);

    flash_read_page(addr, m_buffer_rx);

    int ans = memcmp(m_buffer_tx, m_buffer_rx, QSPI_TEST_DATA_SIZE);
    mprintf("%d\n", ans);
    if ( ans == 0){
        mprintf("Test Flash", "Data consistent - Test Success.\n");
    }
    else{
        mprintf("Test Flash", "Data inconsistent - Test Failed.\n");
    }

    NRF_LOG_HEXDUMP_INFO(m_buffer_tx, QSPI_TEST_DATA_SIZE);
    printf("\n");
    NRF_LOG_HEXDUMP_INFO(m_buffer_rx, QSPI_TEST_DATA_SIZE);
}