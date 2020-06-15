#include "fs_types.h"
#include "fs_main.h"
#include "fram_driver.h"
#include <string.h>
#include "flash_driver.h"
#include "nrf_log.h"
#include "utils.h"

const uint32_t SENSOR_FRAM_INDEX[NUM_OF_SENSORS] = {SENSOR_0_FRAM_INDEX, SENSOR_1_FRAM_INDEX, SENSOR_2_FRAM_INDEX, SENSOR_3_FRAM_INDEX, SENSOR_4_FRAM_INDEX};

const uint8_t SENSOR_BLOCK_RANGE[NUM_OF_SENSORS][2] = { {SENSOR_0_START_BLOCK, SENSOR_0_END_BLOCK}, {SENSOR_1_START_BLOCK, SENSOR_1_END_BLOCK}, 
                                         {SENSOR_2_START_BLOCK, SENSOR_2_END_BLOCK}, {SENSOR_3_START_BLOCK, SENSOR_3_END_BLOCK}, 
                                         {SENSOR_4_START_BLOCK, SENSOR_4_END_BLOCK} };

sensor_data_t* sensors_data[NUM_OF_SENSORS] = {NULL, NULL, NULL, NULL, NULL};

Queue* page_save_queue;


uint8_t get_sensor_data_desc(sensor_data_t* sdata){
  if(sdata == NULL) return FS_FAILED;

  fram_data_read(SENSOR_FRAM_INDEX[sdata->sensor_index], (uint8_t*)&(sdata->sdesc), sizeof(sensor_data_desc_t));
  return FS_SUCCESS;
}


uint8_t set_sensor_data_desc(sensor_data_t* sdata){
  if(sdata == NULL) return FS_FAILED;

  fram_data_write(SENSOR_FRAM_INDEX[sdata->sensor_index],  (uint8_t*)&(sdata->sdesc), sizeof(sensor_data_desc_t));
  return FS_SUCCESS;
}


uint8_t delete_sensor_data_desc(sensor_data_t* sdata){
  if(sdata == NULL) return FS_FAILED;

  memset((uint8_t*)&(sdata->sdesc), 0, sizeof(sensor_data_desc_t));
  set_sensor_data_desc(sdata);
}


void set_default_desc(sensor_data_t* sdata){
  uint8_t sindex = sdata->sensor_index;
  sdata->sdesc.block_end = SENSOR_BLOCK_RANGE[sindex][1];
  sdata->sdesc.block_start = SENSOR_BLOCK_RANGE[sindex][0];
  sdata->sdesc.current_page = 0;
  sdata->sdesc.current_block = sdata->sdesc.block_start;
  sdata->sdesc.filled = 0;
}

void erase_sensor_data_struct(sensor_data_t* sdata){
  if(sdata == NULL) return;

  if(sdata->current_vpage != NULL)
    free(sdata->current_vpage);

  set_default_desc(sdata);
  sdata->ram_blocks = RAM_BLOCKS_PER_SENSOR;
  sdata->current_vpage = make_vpage(sdata, sdata->sdesc.current_page, sdata->sdesc.current_block);
  if(sdata->current_vpage == NULL){
    free(sdata);
    return ;
  }
}


sensor_data_t* init_sensor_data(uint8_t sindex){
  if(sindex >= NUM_OF_SENSORS)
    return NULL;

  mprintf("FS", "Initializing Sensor Data: %d\n", sindex);
  sensor_data_t* sdata = malloc(sizeof(sensor_data_t));
  sdata->sensor_index = sindex;
  sdata->ram_blocks = RAM_BLOCKS_PER_SENSOR;

  get_sensor_data_desc(sdata);

  /* Check if values have been initially saved to flash */
  if( (sdata->sdesc.block_start != SENSOR_BLOCK_RANGE[sindex][0]) || (sdata->sdesc.block_end != SENSOR_BLOCK_RANGE[sindex][1]) ){
    mprintf("FS", "No Previous data saved!, Initializing new data.\n");
    set_default_desc(sdata);
    set_sensor_data_desc(sdata);
  }

  sdata->current_vpage = make_vpage(sdata, sdata->sdesc.current_page, sdata->sdesc.current_block);
  if(sdata->current_vpage == NULL){
    free(sdata);
    return NULL;
  }
  sensors_data[sindex] = sdata;
  print_sdesc(sdata);

  return sdata;
}


vpage_t* make_vpage(sensor_data_t* sdata, uint8_t phy_page, uint8_t block_num){
  
  (sdata->ram_blocks)--;
  vpage_t* vpage = malloc(sizeof(vpage_t));
  
  if(vpage == NULL){
    return NULL;
  }

  vpage->block_num = block_num;
  vpage->phy_page_num = phy_page;
  vpage->_pointer = 0;
  vpage->sindex = sdata->sensor_index;

  return vpage;
}


uint8_t next_page(sensor_data_t* sdata){

  if(!sdata->ram_blocks)
    return FS_NO_RAM_BLOCKS;

  uint8_t ppage;
  uint8_t block;

  if(sdata->sdesc.current_page < 255){
    ppage = sdata->sdesc.current_page + 1;
    block = sdata->sdesc.current_block;
    sdata->sdesc.current_page = ppage;
  }
  else{
    if(sdata->sdesc.current_block < sdata->sdesc.block_end - 1){
      block = sdata->sdesc.current_block + 1;
      ppage = 0;
      sdata->sdesc.current_block = block;
      sdata->sdesc.current_page = ppage;
    }
    else{
      sdata->sdesc.filled = 1;
      mprintf("FS", "Sensor Data Blocks Filled !\n");
      return FS_NO_FLASH_BLOCKS;
    }
  }
  
  save_page(sdata);

  sdata->current_vpage = make_vpage(sdata, ppage, block);

  if(sdata->current_vpage == NULL){
    return FS_UNKNOWN_ERROR;
  }

#ifdef FS_TESTING
  mprintf("FS", "Next page-> Sensor: %d\t Page: %d\t Block: %d\n", sdata->sensor_index,
          sdata->current_vpage->phy_page_num, sdata->current_vpage->block_num);
#endif

  return FS_SUCCESS;
}


uint8_t save_page(sensor_data_t* sdata){
  if(!fs_stopping_save){
    enqueue(page_save_queue, sdata->current_vpage);
  }
  

#ifdef FS_TESTING
  printf("Saving page-> Sensor: %d\t Page: %d\t Block: %d\n", sdata->sensor_index,
          sdata->current_vpage->phy_page_num, sdata->current_vpage->block_num);
#endif

  /* Check if queue was originally empty */
  /*if(page_save_queue->size == 1 && fs_save_active && m_finished){
    fs_handler_start();
  }*/
}


void delete_sensor_data(sensor_data_t* sdata){
  if(sdata == NULL) return ;

  fs_stop_saving();
  for(uint8_t i=sdata->sdesc.block_start; i<sdata->sdesc.block_end; i++){
    flash_erase_block_64kb(generate_page_addr(0, i));
  }
  fs_start_saving();
}


void print_sdesc(sensor_data_t* sdata){
  mprintf("FS", "Sensor %d-> blk_start: %d\t blk_end: %d\t cur_page: %d\t cur_block: %d\n",
        sdata->sensor_index, sdata->sdesc.block_start, sdata->sdesc.block_end, 
        sdata->sdesc.current_page, sdata->sdesc.current_block);
}


void print_current_page(sensor_data_t* sdata){
  NRF_LOG_HEXDUMP_INFO(sdata->current_vpage->_buffer, 20);
}

