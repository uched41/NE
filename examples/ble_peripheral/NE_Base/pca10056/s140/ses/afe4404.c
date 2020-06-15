#include "nrf_log.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"

#include "hardware.h"
#include "utils.h"
#include "afe4404.h"
#include "afe4404_reg.h"

i2c_sensor_t afe4404;

volatile uint8_t adc_rdy = 0;

void afe4404_cb(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
  //NRF_LOG_INFO("Data ready");
  adc_rdy = 1;
}


static void I2C_write (uint8_t slave_address, uint8_t reg_address, uint8_t configData[], uint8_t byteCount)
{
  i2c_sensor_write(&afe4404, reg_address, configData, byteCount);
}

static inline void I2C_read(uint8_t slave_address, uint8_t reg_address, uint8_t *read_Data, uint8_t byteCount)
{ 
  i2c_sensor_read(&afe4404, reg_address, read_Data, byteCount);
}

void AFE4404_Reg_Init(void)
{
  AFE4404_Disable_Read();
  AFE4404_Reg_Write(1, 100);    //AFE_LED2STC

  //long ans = AFE4404_Reg_Read(1);
  //NRF_LOG_INFO("val: %ld", ans);

  AFE4404_Reg_Write(2, 399);    //AFE_LED2ENDC
  AFE4404_Reg_Write(3, 802);    //AFE_LED1LEDSTC
  AFE4404_Reg_Write(4, 1201);   //AFE_LED1LEDENDC
  AFE4404_Reg_Write(5, 501);    //AFE_ALED2STC
  AFE4404_Reg_Write(6, 800);    //AFE_ALED2ENDC
  AFE4404_Reg_Write(7, 902);    //AFE_LED1STC
  AFE4404_Reg_Write(8, 1201);   //AFE_LED1ENDC
  AFE4404_Reg_Write(9, 0);      //AFE_LED2LEDSTC
  AFE4404_Reg_Write(10, 399);   //AFE_LED2LEDENDC
  AFE4404_Reg_Write(11, 1303);  //AFE_ALED1STC
  AFE4404_Reg_Write(12, 1602);  //AFE_ALED1ENDC
  AFE4404_Reg_Write(13, 409);   //AFE_LED2CONVST
  AFE4404_Reg_Write(14, 1468);  //AFE_LED2CONVEND
  AFE4404_Reg_Write(15, 1478);  //AFE_ALED2CONVST
  AFE4404_Reg_Write(16, 2537);  //AFE_ALED2CONVEND
  AFE4404_Reg_Write(17, 2547);  //AFE_LED1CONVST
  AFE4404_Reg_Write(18, 3606);  //AFE_LED1CONVEND
  AFE4404_Reg_Write(19, 3616);  //AFE_ALED1CONVST
  AFE4404_Reg_Write(20, 4675);  //AFE_ALED1CONVEND
  AFE4404_Reg_Write(21, 401);   //AFE_ADCRSTSTCT0
  AFE4404_Reg_Write(22, 407);   //AFE_ADCRSTENDCT0
  AFE4404_Reg_Write(23, 1470);  //AFE_ADCRSTSTCT1
  AFE4404_Reg_Write(24, 1476);  //AFE_ADCRSTENDCT1
  AFE4404_Reg_Write(25, 2539);  //AFE_ADCRSTSTCT2
  AFE4404_Reg_Write(26, 2545);  //AFE_ADCRSTENDCT2
  AFE4404_Reg_Write(27, 3608);  //AFE_ADCRSTSTCT3
  AFE4404_Reg_Write(28, 3614);  //AFE_ADCRSTENDCT3
  AFE4404_Reg_Write(54, 401);   //AFE_LED3LEDSTC
  AFE4404_Reg_Write(55, 800);   //AFE_LED3LEDENDC
  AFE4404_Reg_Write(29, 39999); //AFE_PRPCOUNT
  AFE4404_Reg_Write(30, 0x000103);	//AFE_CONTROL1 TimerEN = 1; NUMAV = 3
  AFE4404_Reg_Write(32, 0x008003);  //AFE_TIA_SEP_GAIN (LED2) ENSEPGAIN = 1; LED2/LED3 gain = 50K
  AFE4404_Reg_Write(33, 0x000005);  //AFE_TIA_GAIN (LED1) LED1/LED1AMB gain = 50K
  AFE4404_Reg_Write(58, 0x039EE7 );  //AFE_DAC_SETTING_REG
  //AFE4404_Reg_Write(58, 0x039EA7 );  //AFE_DAC_SETTING_REG
  //AFE4404_Reg_Write(58, 0x039f67 );  //AFE_DAC_SETTING_REG
  AFE4404_Reg_Write(34, 0x0030CF); 	//LED3 - 3.125mA; LED2 - 3.125mA; LED1 - 12.5mA
  //AFE4404_Reg_Write(34, 0x00379E);
  AFE4404_Reg_Write(35, 0x124218); 	//DYN1, LEDCurr, DYN2, Ext CLK, DYN3, DYN4 //0x000200); - 0x200 Osc mode //AFE_CONTROL2
  AFE4404_Reg_Write(49, 0x000020); 	//ENABLE_INPUT_SHORT
  AFE4404_Reg_Write(57, 0);     	//CLKDIV_PRF
  AFE4404_Reg_Write(50, 5475);  	//AFE_DPD1STC
  AFE4404_Reg_Write(51, 39199); 	//AFE_DPD1ENDC
  AFE4404_Enable_Read();
}

void AFE4404_Init(void)
{
  afe4404.channel_no = AFE4404_I2C_CHANNEL;
  afe4404.addr = AFE4404_I2C_DEFAULT_ADDRESS;

  AFE4404_RESETZ_Init ();
  AFE4404_Trigger_HWReset ();
  AFE4404_Reg_Init();
  AFE4404_ADCRDY_Interrupt_Init();
}

void AFE4404_RESETZ_Init (void)
{
  nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);
  APP_ERROR_CHECK(nrf_drv_gpiote_out_init(AFE4404_RESET_PIN, &out_config));
  nrf_drv_gpiote_out_set(AFE4404_RESET_PIN);
  nrf_delay_ms(10);  
}

void AFE4404_Enable_HWPDN (void)
{
  nrf_drv_gpiote_out_clear(AFE4404_RESET_PIN);
  nrf_delay_ms(10);        
}

void AFE4404_Disable_HWPDN (void)
{
  nrf_drv_gpiote_out_set(AFE4404_RESET_PIN);
  nrf_delay_ms(10);        
}

void AFE4404_Trigger_HWReset (void)
{
  nrf_drv_gpiote_out_clear(AFE4404_RESET_PIN);
  nrf_delay_us(30);
  nrf_drv_gpiote_out_set(AFE4404_RESET_PIN);
  nrf_delay_ms(10);
}

void AFE4404_Reg_Write (unsigned char reg_address, unsigned long data)
{
  unsigned char configData[3];
  configData[0]=(unsigned char)(data >>16);
  configData[1]=(unsigned char)(((data & 0x00FFFF) >>8));
  configData[2]=(unsigned char)(((data & 0x0000FF)));
  I2C_write(AFE4404_I2C_DEFAULT_ADDRESS, reg_address, configData, 3);
}

signed long AFE4404_Reg_Read(unsigned char Reg_address)
{
  unsigned char configData[3];
  signed long retVal;
  I2C_read (AFE4404_I2C_DEFAULT_ADDRESS, Reg_address, configData, 3);
  retVal = configData[0];
  retVal = (retVal << 8) | configData[1];
  retVal = (retVal << 8) | configData[2];
  if (Reg_address >= 0x2A && Reg_address <= 0x2F)
  {
  	if (retVal & 0x00200000) 	// check if the ADC value is positive or negative
  	{
  	  retVal &= 0x003FFFFF;		// convert it to a 22 bit value
      //Serial.println(retVal, HEX);
      //retVal = (~retVal)+1;
      //retVal = retVal^0xFFC00000;
      //retVal = (~retVal)+1;
      //Serial.println(retVal, HEX);
  	  //return (retVal^0x00000000);
  	}
  }
  return retVal;
}

void AFE4404_Enable_Read (void)
{
  unsigned char configData[3];
  configData[0]=0x00;
  configData[1]=0x00;
  configData[2]=0x01;
  I2C_write (AFE4404_I2C_DEFAULT_ADDRESS, CONTROL0, configData, 3);
}

void AFE4404_Disable_Read (void)
{
  unsigned char configData[3];
  configData[0]=0x00;
  configData[1]=0x00;
  configData[2]=0x00;
  I2C_write (AFE4404_I2C_DEFAULT_ADDRESS, CONTROL0, configData, 3);
}

void AFE4404_ADCRDY_Interrupt_Init (void)
{
  ret_code_t err_code;
  nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

  err_code = nrf_drv_gpiote_in_init(AFE4404_INT_PIN, &in_config, afe4404_cb);
  APP_ERROR_CHECK(err_code);

  nrf_drv_gpiote_in_event_enable(AFE4404_INT_PIN, true);
}



