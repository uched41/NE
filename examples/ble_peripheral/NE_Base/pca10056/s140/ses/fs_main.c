#include <stdint.h>
#include "fs_types.h"
#include "fs_main.h"
#include "fs_config.h"
#include "nrf_log.h"
#include <string.h>
#include "nrf_log.h"
#include "utils.h"

volatile uint8_t fs_save_active = 0;
volatile uint8_t fs_stopping_save = 0;
volatile vpage_t* fs_handler_page = NULL;

void init_fs(){
  mprintf("FS", "Initializing File System\n");
  page_save_queue = initQueue(MAX_VPAGES);
}

void fs_start_saving(){
  fs_save_active = 1;
  fs_stopping_save = 0;
  mprintf("FS", "Saving Started\n");
}

void fs_stop_saving(){ 
  fs_stopping_save = 1;
  mprintf("FS", "Stopping save. Queue size: %d\n", page_save_queue->size);

  /* Wait until queue is empty */
  while(page_save_queue->size); 
  fs_save_active = 0;
  mprintf("FS", "Saving Stopped.\n");
}

void fs_handler(){
  /* When Function is called again, previous vpage has been saved, so we can free its memory */
  if(fs_handler_page != NULL){    
    sensor_data_t* sdata = sensors_data[fs_handler_page->sindex];
    set_sensor_data_desc(sdata);
    (sdata->ram_blocks)++;
    free(fs_handler_page);
    fs_handler_page = NULL;
  }

  if(page_save_queue->size){
    fs_handler_page = front(page_save_queue);
    dequeue(page_save_queue);

//#ifdef FS_TESTING
    mprintf("FS", "Handling-> ppage: %d \t block: %d \t sensor index: %d\n", 
            fs_handler_page->phy_page_num, fs_handler_page->block_num, fs_handler_page->sindex);
//#endif
    //NRF_LOG_HEXDUMP_INFO(fs_handler_page->_buffer, 10);

    flash_write_page( generate_page_addr(fs_handler_page->phy_page_num, fs_handler_page->block_num), 
                      fs_handler_page->_buffer);
  }
}


void fs_handler_start(){
  fs_handler();
}


void fs_test(){
  sensor_data_t* sensor_test = init_sensor_data(0);

  for(int i=0; i<160; i++){
    for(int j=0; j<256; j++){
      sensor_set_data(sensor_test, (uint8_t*)&j, 1);
    }
  }
  print_current_page(sensor_test);
  mprintf("FS", "Size of Queue: %d\n", page_save_queue->size);

  sensor_data_t* sensor_test2 = init_sensor_data(1);

  //return;

  /*fs_start_saving();

  for(int i=0; i<10; i+=2){
    for(int j=0; j<PAGE_SIZE; j++){
      sensor_set_data(sensor_test, (uint8_t*)&j, 1);
      sensor_set_data(sensor_test2, (uint8_t*)&j, 1);
    }
  }

  uint8_t buf[PAGE_SIZE];*/
  //flash_read_page(generate_page_addr(1, SENSOR_1_START_BLOCK), buf);
  //NRF_LOG_HEXDUMP_DEBUG(buf, PAGE_SIZE);
}