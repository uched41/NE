#ifndef FRAM_DRIVER_H
#define FRAM_DRIVER_H

// Commands
#define FM25V10_CMD_WREN     0x06  // Set Write Enable Latch
#define FM25V10_CMD_WRDI     0x04  // Write Disable

#define FM25V10_CMD_RDSR     0x05  // Read  Status register
#define FM25V10_CMD_WRSR     0x01  // Write Status register

#define FM25V10_CMD_READ     0x03  // Read Memory data
#define FM25V10_CMD_FSTRD    0x0B  // Fast Read Memory data
#define FM25V10_CMD_WRITE    0x02  // Write memory data

#define FM25V10_CMD_SLEEP    0xB9  // Enter Sleep Mode

#define FM25V10_CMD_RDID     0x9F  // Read DeviceID
#define FM25V10_CMD_SNR      0xC3  // Read Serial Number

#define FM25V10_CMD_BULK     0xC7  // Bulk   Erase FULL
#define FM25V10_CMD_SEER     0xD8  // Sector Erase 64k
#define FM25V10_CMD_SER      0x20  // Sector Erase  4k

//Status Register
#define FM25V10_STA_WEL      0x02      // Write Enable Bit
#define FM25V10_STA_BP0      0x04      // Block Protect bit ‘0’
#define FM25V10_STA_BP1      0x08      // Block Protect bit ‘1’
#define FM25V10_STA_WPEN     0x80      // Write Protect Pin Enable bit

void init_fram(void);

void fram_data_write(uint32_t address, uint8_t* data, uint8_t data_length);

void fram_data_read(uint32_t address, uint8_t* data, uint8_t data_length);

void fram_byte_write(uint32_t address, uint8_t data);

uint8_t fram_byte_read(uint32_t address);

void fram_get_id(uint8_t * ID);

void test_fram(void);

#endif