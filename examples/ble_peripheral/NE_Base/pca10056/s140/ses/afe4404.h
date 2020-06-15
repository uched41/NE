#ifndef _AFE4404_H
#define _AFE4404_H

#include "i2c_multi_driver.h"

extern i2c_sensor_t afe4404;

extern volatile uint8_t adc_rdy;

void AFE4404_Init(void);

void AFE4404_RESETZ_Init (void);

void AFE4404_Enable_HWPDN (void);

void AFE4404_Disable_HWPDN (void);

void AFE4404_Trigger_HWReset (void);

void AFE4404_ADCRDY_Interrupt_Init (void);

void AFE4404_ADCRDY_Interrupt_Enable (void);

void AFE4404_ADCRDY_Interrupt_Disable (void);

void AFE4404_Reg_Init(void);

void AFE4404_Reg_Write(unsigned char reg_address, unsigned long reg_data);

signed long AFE4404_Reg_Read(unsigned char reg_address);

void AFE4404_Enable_Read (void);

void AFE4404_Disable_Read (void);





#endif