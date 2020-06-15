#ifndef FS_TYPES_H 
#define FS_TYPES_H

#include <string.h>
#include <stdint.h>
#include "queue.h"
#include "flash_driver.h"
#include "fram_driver.h"
#include "queue.h"
#include "fs_config.h"

extern Queue* page_save_queue;
extern volatile uint8_t fs_save_active;
extern volatile uint8_t fs_stopping_save;

typedef struct{
  uint8_t _buffer[PAGE_SIZE];
  uint8_t phy_page_num;
  uint8_t block_num;
  uint8_t _pointer;
  uint8_t sindex;
}vpage_t;

typedef struct{
  uint8_t block_start;
  uint8_t block_end;
  uint8_t current_page;
  uint8_t current_block;
  uint8_t filled;
}sensor_data_desc_t;

typedef struct{
  sensor_data_desc_t sdesc;
  vpage_t* current_vpage;
  uint8_t sensor_index;
  uint8_t ram_blocks;
}sensor_data_t;

extern sensor_data_t* sensors_data[];

#define generate_page_addr(ppage, block) (uint32_t)(block*256*256) + (uint32_t)(ppage*256)

/* Functions */

uint8_t get_sensor_data_desc(sensor_data_t* sdata);

uint8_t set_sensor_data_desc(sensor_data_t* sdata);

uint8_t delete_sensor_data_desc(sensor_data_t* sdata);

void erase_sensor_data_struct(sensor_data_t* sdata);

void set_default_desc(sensor_data_t* sdata);

sensor_data_t* init_sensor_data(uint8_t sindex);

vpage_t* make_vpage(sensor_data_t* sdata, uint8_t phy_page, uint8_t block_num);

uint8_t save_page(sensor_data_t* sdata);

void delete_sensor_data(sensor_data_t* sdata);

uint8_t next_page(sensor_data_t* sdata);

void print_sdesc(sensor_data_t* sdata);

void print_current_page(sensor_data_t* sdata);


/* Function to set data */
static inline void sensor_set_data(sensor_data_t* sdata, uint8_t* data, uint8_t len ){
    if(sdata->sdesc.filled || !fs_save_active)
      return;

    if(sdata->current_vpage->_pointer < PAGE_SIZE-len){
      memcpy(sdata->current_vpage->_buffer + (sdata->current_vpage->_pointer), data, len);
      sdata->current_vpage->_pointer += len;
    }

    else if(sdata->current_vpage->_pointer == PAGE_SIZE-len){
      memcpy(sdata->current_vpage->_buffer + (sdata->current_vpage->_pointer), data, len);
      uint8_t res = next_page(sdata);
      if(res != FS_SUCCESS) 
        return;
    }
                   
    else{
      uint8_t res = next_page(sdata);
      if(res != FS_SUCCESS) 
        return;
      memcpy(sdata->current_vpage->_buffer + (sdata->current_vpage->_pointer), data, len);
      sdata->current_vpage->_pointer += len;
    }
}

#endif 