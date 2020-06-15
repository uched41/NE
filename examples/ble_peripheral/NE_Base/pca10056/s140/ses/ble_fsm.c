#include "ble_fsm.h"
#include "fs_main.h"
#include "sdk_common.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_delay.h"
#include <stdlib.h>
#include "utils.h"

#define COMMAND_1_INDEX     0
#define ARGUMENT_1_INDEX    1
#define ARGUMENT_2_INDEX    2

#define REPLY_RETURN_CODE_INDEX 0
#define REPLY_DATA_INDEX        1

Queue* ble_stream_queue;

static ble_fsm_reply_t ble_start_saving(){
  ble_fsm_reply_t ret;

  if(!fs_save_active)
    fs_start_saving();

  ret.buffer[REPLY_RETURN_CODE_INDEX] = ble_fsm_success;
  ret.len = 1;
  return ret;
}


static ble_fsm_reply_t ble_stop_saving(){
  ble_fsm_reply_t ret;

  if(fs_save_active)
    fs_stop_saving();

  ret.buffer[REPLY_RETURN_CODE_INDEX] = ble_fsm_success;
  ret.len = 1;
  return ret;
}


static ble_fsm_reply_t ble_read_data(sensor_data_t* sdata){
  ble_fsm_reply_t ret;
  mprintf("BLE", "Read Data Start\n");
  ble_stream_request_t* ble_request = malloc(sizeof(ble_stream_request_t));

  ble_request->blk_start = sdata->sdesc.block_start;
  ble_request->blk_end  = sdata->sdesc.current_block;
  ble_request->page_end = sdata->sdesc.current_page;
  ble_request->page_start = 0;

  enqueue(ble_stream_queue, ble_request);

  ret.buffer[REPLY_RETURN_CODE_INDEX] = ble_fsm_success;
  ret.len = 1;
  mprintf("BLE", "Read Data Finished\n");
  return ret;
}


static ble_fsm_reply_t ble_erase_data(sensor_data_t* sdata){
  ble_fsm_reply_t ret;
  mprintf("BLE", "Erase Data Start\n");

  uint8_t initial_save_status = fs_save_active;
  ble_stop_saving();

  uint8_t start_block = sdata->sdesc.block_start;
  uint8_t stop_block = sdata->sdesc.current_block;

  mprintf("BLE", "Erasing Sensor %d-> block_start:%d, end_block: %d\n", start_block, stop_block);
  for(uint8_t j=start_block; j<stop_block+1; j++){
    mprintf("BLE", "Erasing Block %d \n", j);
    flash_erase_block_64kb( generate_page_addr(0, j) );
  }

  erase_sensor_data_struct(sdata);

  if(initial_save_status)
    ble_start_saving();

  ret.buffer[REPLY_RETURN_CODE_INDEX] = ble_fsm_success;
  ret.len = 1;
  mprintf("BLE", "Erase Data End\n");
  return ret;
}


static ble_fsm_reply_t ble_read_desc(sensor_data_t* sdata){
  ble_fsm_reply_t ret;
  
  mprintf("BLE", "Reading Sensor %d Data Description\n", sdata->sensor_index);

  ret.buffer[REPLY_RETURN_CODE_INDEX] = ble_fsm_success;
  memcpy(ret.buffer+REPLY_DATA_INDEX, (uint8_t*)&sdata->sdesc, sizeof(sensor_data_desc_t));
  ret.len = sizeof(sensor_data_desc_t) + REPLY_DATA_INDEX;
  return ret;
}


static ble_fsm_reply_t ble_erase_desc(sensor_data_t* sdata){
  ble_fsm_reply_t ret;

  mprintf("BLE", "Erasing Sensor %d Data Description\n", sdata->sensor_index);

  set_default_desc(sdata);
  set_sensor_data_desc(sdata);

  ret.buffer[REPLY_RETURN_CODE_INDEX] = ble_fsm_success;
  ret.len = 1;
  return ret;
}


static ble_fsm_reply_t ble_read_flash_page(uint8_t page, uint8_t blk){
  ble_fsm_reply_t ret;
  uint8_t buf[PAGE_SIZE];

  uint8_t initial_save_status = fs_save_active;
  ble_stop_saving();

  mprintf("BLE", "Reading Page: %d, Block: %d, addr: %lu", page, blk, generate_page_addr(page, blk));
  flash_read_page( generate_page_addr(page, blk), buf);
  //NRF_LOG_HEXDUMP_INFO(buf, PAGE_SIZE);

  ble_page_send(buf, PAGE_SIZE/2);
  ble_page_send(buf+PAGE_SIZE/2, PAGE_SIZE/2);

  if(initial_save_status)
    ble_start_saving();

  ret.buffer[REPLY_RETURN_CODE_INDEX] = ble_fsm_success;
  ret.len = 1;
  return ret;
}


static ble_fsm_reply_t ble_fill_sensor_data(sensor_data_t* sdata){
  ble_fsm_reply_t ret;
  uint8_t buf[PAGE_SIZE];
  mprintf("BLE", "Fill Data Start\n");

  ret = ble_erase_data(sdata);
  if(ret.buffer[REPLY_RETURN_CODE_INDEX] != ble_fsm_success)
    return ret;

  mprintf("BLE", "Filling Sensor data: %d\n", sdata->sensor_index);
  ble_start_saving();
  uint32_t k = 100;
  for(uint32_t i=0; i<UINT16_MAX+1; i++){
    sensor_set_data(sdata, (uint8_t*)&k, 4);
  }

  ret.buffer[REPLY_RETURN_CODE_INDEX] = ble_fsm_success;
  ret.len = 1;
  mprintf("BLE", "Fill Data End\n");
  return ret;
}


/* BLE Write Parser */

ble_fsm_reply_t ble_fsm_parse_data(ble_cus_t * p_cus, const uint8_t* buf, uint8_t len){
  ble_fsm_reply_t ret;
  ret.buffer[REPLY_RETURN_CODE_INDEX] = ble_fsm_cmd_not_found;
  ret.len = 1;

  uint8_t cmd = buf[COMMAND_1_INDEX];
  uint8_t arg1, arg2;

  sensor_data_t* sdata;

  mprintf("BLE", "Parsing BLE Data\n");

  /* Arg 1 is usually sensor number */
  if(len > 1){
    arg1 = buf[ARGUMENT_1_INDEX];
    
    if(cmd != read_flash_page_command){
      uint8_t sensor = arg1;
      if(sensor >= NUM_OF_SENSORS){
        ret.buffer[REPLY_RETURN_CODE_INDEX] = ble_fsm_wrong_index;
        ret.len = 1;
        return ret;
      }

      sdata = sensors_data[sensor];

      if(sdata == NULL){
        ret.buffer[REPLY_RETURN_CODE_INDEX] = ble_fsm_sensor_not_initialized;
        ret.len = 1;
        return ret;
      }
    }
    
  }

  if(len > 2) arg2 = buf[ARGUMENT_2_INDEX];

  switch(cmd){
      case read_sensor_data_command: 
        return ble_read_data(sdata);
        break;

      case erase_sensor_data_command: 
        return ble_erase_data(sdata);
        break;

      case read_sensor_data_desc_command: 
        return ble_read_desc(sdata);
        break;

      case erase_sensor_data_desc_command: 
        return ble_erase_desc(sdata);
        break;

      case start_saving_command: 
        return ble_start_saving();
        break;

      case stop_saving_command: 
        return ble_stop_saving();
        break;

      case real_time_command: 

        break;

      case read_flash_page_command:
        return ble_read_flash_page(arg1, arg2);
        break;

      case fill_sensor_data:
        return ble_fill_sensor_data(sdata);
      default: 
        return ret;
        break;
  }

}

void ble_stream_init(void){
  ble_stream_queue = initQueue(255);
}

void ble_stream_loop(void){
  if(ble_stream_queue->size){
    ble_stream_request_t* ble_request = front(ble_stream_queue);
    if(ble_request == NULL) return;

      uint8_t initial_save_status = fs_save_active;
      ble_stop_saving();

      uint8_t start_block = ble_request->blk_start;
      uint8_t stop_block = ble_request->blk_end;
      uint8_t start_page = ble_request->page_start;
      uint8_t end_page   = ble_request->page_end;

      uint8_t buf[PAGE_SIZE];
      uint16_t k = 0;
      for(uint8_t i=start_block; i<stop_block+1; i++){
        for(uint8_t j=start_page; j<256; j++){
          if(i==stop_block && j>end_page) break;

          flash_read_page( generate_page_addr(j, i), buf);

          /* Send in 2 halves */
          ble_page_send(buf, PAGE_SIZE/2);
          ble_page_send(buf+PAGE_SIZE/2, PAGE_SIZE/2);
        }
      }

      if(initial_save_status)
        ble_start_saving();

      dequeue(ble_stream_queue);

      free(ble_request);
  }
}
