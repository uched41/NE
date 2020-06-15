#ifndef UTILS_H
#define UTILS_H

#include "nrf_drv_spi.h"
#include "nrfx_twi.h"

extern const nrf_drv_spi_t m_spi;
extern const nrfx_twi_t m_twi_instance;


void init_peripherals(void);

void mprintf(const char* head, const char* fmt, ...);

#endif