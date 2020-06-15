#ifndef FS_MAIN_H
#define FS_MAIN_H

#include "fs_types.h"
#include "fs_config.h"

extern volatile vpage_t* fs_handler_page;

void init_fs();

void fs_start_saving();

void fs_stop_saving();

void fs_handler();

void fs_handler_start();

void fs_test();

void fs_error_handler(sensor_data_t sdata, FS_RETURN_T res);

#endif