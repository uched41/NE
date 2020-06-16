#include "led_driver.h"
#include "nrf_drv_spi.h"
#include "nrf_log.h"
#include "utils.h"
#include <stdlib.h>

nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(1);    // spi instance
uint8_t start_buffer[4] = {0, 0, 0, 0};
uint8_t end_buffer[13] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Initialize led driver
void led_driver_init(led_driver_t* leds, uint8_t clkPin, uint8_t dataPin){	                                                                                                                      
  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  spi_config.mosi_pin = dataPin;
  spi_config.sck_pin  = clkPin;
  spi_config.frequency = NRF_DRV_SPI_FREQ_8M;

  ret_code_t err_code;
  err_code = nrf_drv_spi_init(&spi, &spi_config, NULL, NULL);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_INFO("SPI started.");
  
  leds->_buffer = (crgb_t*)malloc(sizeof(crgb_t)*LEDS_COUNT);
  if(leds->_buffer == NULL){
    NRF_LOG_INFO("Unable to allocate memoery for leds.");
    APP_ERROR_CHECK(NRF_ERROR_SDK_ERROR_BASE);
  }
  leds_clear2(leds->_buffer);
  NRF_LOG_INFO("Led driver initialized.");
}

// Set pixel using location coordinate
void leds_setpixel1(crgb_t* ledBuf, location_t locale, crgb_t col){
  int8_t x = loc2index(locale);
  if(x>=0 && x<LEDS_COUNT){
    ledBuf[x] = col;
  }
}

// Set pixel using index
void leds_setpixel2(crgb_t* ledBuf, uint8_t ind, crgb_t col){
  ledBuf[ind] = col;
}

void leds_show(crgb_t* ledBuf){
  // Note the nrf_drv_spi_transfer function accepts a maximum buffer length of 255
  int slen = sizeof(crgb_t)*LEDS_COUNT;
  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, start_buffer, sizeof(start_buffer)/sizeof(start_buffer[0]), NULL, 0));      // starting sequence
  if(slen<256){
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, (uint8_t*)(ledBuf), slen, NULL, 0));  
  }
  else{
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, (uint8_t*)(ledBuf), 255, NULL, 0));   
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, ((uint8_t*)(ledBuf))+255, slen-255, NULL, 0));  
  }
  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, end_buffer, sizeof(end_buffer)/sizeof(end_buffer[0]), NULL, 0));     // ending sequence
}

// Clear Leds and show
void leds_clear1(crgb_t* ledBuf){
  for(int i=0; i<LEDS_COUNT; i++){
    ledBuf[i] = (crgb_t){0b11110000, 0, 0, 0};
  }
  leds_show(ledBuf);
}

// Clear leds without show
void leds_clear2(crgb_t* ledBuf){
  for(int i=0; i<LEDS_COUNT; i++){
    ledBuf[i] = (crgb_t){0b11110000, 0, 0, 0};
  }
}

// Set buffer directly 
void leds_setbuffer(crgb_t* ledBuf, uint8_t* buf, uint16_t buflen){
  memcpy(ledBuf, buf, buflen);
}

void leds_test(crgb_t* ledBuf){
  leds_clear1(ledBuf);
  for(uint8_t i=0; i<LEDS_COUNT; i++){
    leds_setpixel2(ledBuf, i, (crgb_t){0b11110000, 0, 15, 0});
    leds_show(ledBuf);
    delay(30);
  }
}

void leds_reverse_array(crgb_t* buf, uint8_t len){
  for(uint8_t i=0; i<4; i++){
    crgb_t tem = buf[len-i-1];
    buf[len-i-1] = buf[i];
    buf[i] = tem;
  }
}