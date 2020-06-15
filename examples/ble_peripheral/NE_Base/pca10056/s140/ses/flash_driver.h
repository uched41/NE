#ifndef FLASH_DRIVER_H
#define FLASH_DRIVER_H

#include "stdbool.h"
#include "nrf_drv_qspi.h"

#define PAGE_SIZE 256
#define NUM_OF_PAGES 65536 
#define NUM_OF_BLOCKS 256
/*
 * Definitions of the commands shown in this example.
 */
#define MFTR_DEVICE_ID            0x94
#define QUAD_INPUT_PAGE_PROGRAM   0x32
#define FAST_READ_QUAD_IO         0xEB
#define FAST_READ_QUAD_OUPUT      0x6B
#define SET_BURST_WITH_WRAP       0x77
#define WRITE_ENABLE              0x06
#define READ_STATUS_REGISTER_1    0x05
#define READ_STATUS_REGISTER_2    0x35
#define READ_STATUS_REGISTER_3    0x15
#define WRITE_STATUS_REGISTER_1   0x01
#define WRITE_STATUS_REGISTER_2   0x31
#define WRITE_STATUS_REGISTER_3   0x11
#define SECTOR_ERASE              0x20
#define BLOCK_ERASE_32KB          0x52
#define CHIP_ERASE                0xC7
#define FLASH_POWER_DOWN          0xB9
#define FLASH_POWER_UP            0xAB
#define ENABLE_FLASH_RESET        0x66
#define FLASH_RESET               0x99

extern volatile bool m_finished;

#define WAIT_FOR_QSPI() do { \
        while (!m_finished) {} \
        m_finished = false;    \
    } while (0)

#define qspi_busy while(nrfx_qspi_mem_busy_check() != NRF_SUCCESS){}

void test_flash();

void init_flash(); 

void flash_write_enable(void);

void flash_write_page(uint32_t page_addr, uint8_t* buffer);

void flash_write_page_blocking(uint32_t page_addr, uint8_t* buffer);

void flash_read_page(uint32_t page_addr, uint8_t* buffer);

void flash_erase_block_4kb(uint32_t addr);

void flash_erase_block_64kb(uint32_t addr);

void flash_erase_chip();

#endif