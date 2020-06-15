#ifndef BLE_FSM_H
#define BLE_FSM_H

#include <stdint.h>
#include "ble_cus.h"
#include "queue.h"

typedef enum{
  read_sensor_data_command,
  erase_sensor_data_command,
  read_sensor_data_desc_command,
  erase_sensor_data_desc_command,
  start_saving_command,
  stop_saving_command,
  real_time_command,
  read_flash_page_command,
  fill_sensor_data
}ble_command_t;

typedef enum{
  ble_fsm_success,
  ble_fsm_sensor_not_initialized,
  ble_fsm_wrong_index,
  ble_fsm_cmd_not_found
}ble_error_code_t;

typedef struct{
  uint8_t buffer[16];
  uint8_t len;
}ble_fsm_reply_t;

typedef struct{
  uint8_t blk_start;
  uint8_t blk_end;
  uint8_t page_start;
  uint8_t page_end;
}ble_stream_request_t;

extern Queue* ble_stream_queue;

/* Funtions */

ble_fsm_reply_t ble_fsm_parse_data(ble_cus_t * p_cus, const uint8_t* buf, uint8_t len);

uint8_t ble_fsm_exec_command();

void ble_page_send(uint8_t* bbuf, uint8_t len);

void ble_stream_loop(void);

void ble_stream_init(void);
#endif