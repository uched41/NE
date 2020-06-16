#include <stdint.h>
#include <stdlib.h>
#include "nrf_log.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "rtc.h"
#include "hardware.h"

enum Register {
  kSecondReg       = 0,
  kMinuteReg       = 1,
  kHourReg         = 2,
  kDateReg         = 3,
  kMonthReg        = 4,
  kDayReg          = 5,
  kYearReg         = 6,
  kWriteProtectReg = 7,
  kRamAddress0     = 32
};

enum Command {
  kClockBurstRead  = 0xBF,
  kClockBurstWrite = 0xBE,
  kRamBurstRead    = 0xFF,
  kRamBurstWrite   = 0xFE
};

static void     writeProtect(ds1302_t* ds1302, uint8_t enable);
static void     writeOut(ds1302_t* ds1302, uint8_t value, uint8_t readAfter);
static uint8_t  readIn(ds1302_t* ds1302);
static void     writeRam(ds1302_t* ds1302, uint8_t address, uint8_t value);
static uint8_t  readRam(ds1302_t* ds1302, uint8_t address);
static void     writeRamBulk(ds1302_t* ds1302, const uint8_t* data, int len);
static void     readRamBulk(ds1302_t* ds1302, uint8_t* data, int len);
static uint8_t  readRegister(ds1302_t* ds1302, uint8_t reg);
static void     writeRegister(ds1302_t* ds1302, uint8_t reg, uint8_t value);
static uint8_t  bcdToDec(const uint8_t bcd);
static uint8_t  decToBcd(const uint8_t dec);
static uint8_t  hourFromRegisterValue(const uint8_t value);
static void     start_spi_session(ds1302_t* ds1302);
static void     stop_spi_session(ds1302_t* ds1302);


// GPIO PIN MANIPULATIONS
#define PIN_LOW(x)    nrf_gpio_pin_clear(x)
#define PIN_HIGH(x)   nrf_gpio_pin_set(x)
#define digitalWrite(x, y)  y ? PIN_HIGH(x) : PIN_LOW(x)
#define digitalRead(x)      nrf_gpio_pin_read(x)

#define OUTPUT  0
#define INPUT   1   
#define SET_PIN_INPUT(x)    nrf_gpio_cfg_input(x, NRF_GPIO_PIN_NOPULL)
#define SET_PIN_OUTPUT(x)   nrf_gpio_cfg_output(x)

ds1302_t mrtc = { 0, RTC_CE_PIN, RTC_IO_PIN, RTC_SCLK_PIN};
Time global_time = {12, 39, 10, 8, 3, kThursday, 2012};

// Establishes and terminates a three-wire SPI session.
static void start_spi_session(ds1302_t* ds1302){
  PIN_LOW(ds1302->sclk_pin_);
  PIN_HIGH(ds1302->ce_pin_);
  nrf_delay_us(4);  // tCC
}

// Terminates a three-wire SPI session.
static void stop_spi_session(ds1302_t* ds1302){
  PIN_LOW(ds1302->ce_pin_);
  nrf_delay_us(4);  // tCWH
}

uint8_t bcdToDec(const uint8_t bcd) {
  return (10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}

uint8_t decToBcd(const uint8_t dec) {
  const uint8_t tens = dec / 10;
  const uint8_t ones = dec % 10;
  return (tens << 4) | ones;
}

uint8_t hourFromRegisterValue(const uint8_t value) {
  uint8_t adj;
  if (value & 128)  // 12-hour mode
    adj = 12 * ((value & 32) >> 5);
  else           // 24-hour mode
    adj = 10 * ((value & (32 + 16)) >> 4);
  return (value & 15) + adj;
}

void rtc_make_time(Time* dtime, uint16_t yr, uint8_t mon, uint8_t date, uint8_t hr, uint8_t min, uint8_t sec, Day day){
  dtime->yr   = yr;
  dtime->mon  = mon;
  dtime->date = date;
  dtime->hr   = hr;
  dtime->min  = min;
  dtime->sec  = sec;
  dtime->day  = day;
}

void rtc_init(ds1302_t* ds1302) {
  ds1302->kRamSize = 31;

  SET_PIN_INPUT(ds1302->io_pin_);
  SET_PIN_OUTPUT(ds1302->sclk_pin_);
  SET_PIN_OUTPUT(ds1302->ce_pin_);

  PIN_LOW(ds1302->ce_pin_);
  PIN_LOW(ds1302->sclk_pin_);

  rtc_halt(ds1302, false);
}

void writeOut(ds1302_t* ds1302, uint8_t value, uint8_t readAfter){
  SET_PIN_OUTPUT(ds1302->io_pin_);

  for (int i = 0; i < 8; ++i) {
    digitalWrite(ds1302->io_pin_, (value >> i) & 1);
    nrf_delay_us(1);
    digitalWrite(ds1302->sclk_pin_, 1);
    nrf_delay_us(1);

    if (readAfter && i == 7) {
      // We're about to read data -- ensure the pin is back in input mode
      // before the clock is lowered.
      SET_PIN_INPUT(ds1302->io_pin_);
    } else {
      digitalWrite(ds1302->sclk_pin_, 0);
      nrf_delay_us(1);
    }
  }
}

uint8_t readIn(ds1302_t* ds1302) {
  uint8_t input_value = 0;
  uint8_t bit = 0;
  SET_PIN_INPUT(ds1302->io_pin_);

  // Bits from the DS1302 are output on the falling edge of the clock
  // cycle. This is called after readIn (which will leave the clock low) or
  // writeOut(..., true) (which will leave it high).
  for (int i = 0; i < 8; ++i) {
    digitalWrite(ds1302->sclk_pin_, 1);
    nrf_delay_us(1);
    digitalWrite(ds1302->sclk_pin_, 0);
    nrf_delay_us(1);

    bit = digitalRead(ds1302->io_pin_);
    input_value |= (bit << i);  // Bits are read LSB first.
  }

  return input_value;
}

uint8_t readRegister(ds1302_t* ds1302, const uint8_t reg) {
  start_spi_session(ds1302);
  const uint8_t cmd_byte = (0x81 | (reg << 1));
  writeOut(ds1302, cmd_byte, true);
  uint8_t ans = readIn(ds1302);
  stop_spi_session(ds1302);
  return ans;
}

void writeRegister(ds1302_t* ds1302, const uint8_t reg, const uint8_t value) {
  start_spi_session(ds1302);
  const uint8_t cmd_byte = (0x80 | (reg << 1));
  writeOut(ds1302, cmd_byte, false);
  writeOut(ds1302, value, false);
  stop_spi_session(ds1302);
}

void writeProtect(ds1302_t* ds1302, uint8_t enable) {
  writeRegister(ds1302, kWriteProtectReg, (enable << 7));
}

void rtc_halt(ds1302_t* ds1302, uint8_t value) {
  uint8_t sec = readRegister(ds1302, kSecondReg);
  sec &= ~(1 << 7);
  sec |= (value << 7);
  writeRegister(ds1302, kSecondReg, sec);
}

void rtc_get_time(ds1302_t* ds1302) {
  start_spi_session(ds1302);

  writeOut(ds1302, kClockBurstRead, true);
  global_time.sec = bcdToDec(readIn(ds1302) & 0x7F);
  global_time.min = bcdToDec(readIn(ds1302));
  global_time.hr = hourFromRegisterValue(readIn(ds1302));
  global_time.date = bcdToDec(readIn(ds1302));
  global_time.mon = bcdToDec(readIn(ds1302));
  global_time.day = bcdToDec(readIn(ds1302));
  global_time.yr = 2000 + bcdToDec(readIn(ds1302));
  stop_spi_session(ds1302);
}

void rtc_set_time(ds1302_t* ds1302, Time* t) {
  // We want to maintain the Clock Halt flag if it is set.
  const uint8_t ch_value = readRegister(ds1302, kSecondReg) & 0x80;

  start_spi_session(ds1302);

  writeOut(ds1302, kClockBurstWrite, false);
  writeOut(ds1302, ch_value | decToBcd(t->sec), false);
  writeOut(ds1302, decToBcd(t->min), false);
  writeOut(ds1302, decToBcd(t->hr), false);
  writeOut(ds1302, decToBcd(t->date), false);
  writeOut(ds1302, decToBcd(t->mon), false);
  writeOut(ds1302, decToBcd(t->day), false);
  writeOut(ds1302, decToBcd(t->yr - 2000), false);
  // All clock registers *and* the WP register have to be written for the time
  // to be set.
  writeOut(ds1302, 0, false);  // Write protection register.
  stop_spi_session(ds1302);
}

void writeRam(ds1302_t* ds1302, const uint8_t address, const uint8_t value) {
  if (address >= ds1302->kRamSize) {
    return;
  }

  writeRegister(ds1302, kRamAddress0 + address, value);
}

uint8_t readRam(ds1302_t* ds1302, const uint8_t address) {
  if (address >= ds1302->kRamSize) {
    return 0;
  }

  return readRegister(ds1302, kRamAddress0 + address);
}

void writeRamBulk(ds1302_t* ds1302, const uint8_t* const data, int len) {
  if (len <= 0) {
    return;
  }
  if (len > ds1302->kRamSize) {
    len = ds1302->kRamSize;
  }

  start_spi_session(ds1302);

  writeOut(ds1302, kRamBurstWrite, false);
  for (int i = 0; i < len; ++i) {
    writeOut(ds1302, data[i], false);
  }
  stop_spi_session(ds1302);
}

void readRamBulk(ds1302_t* ds1302, uint8_t* const data, int len) {
  if (len <= 0) {
    return;
  }
  if (len > ds1302->kRamSize) {
    len = ds1302->kRamSize;
  }

  start_spi_session(ds1302);

  writeOut(ds1302, kRamBurstRead, true);
  for (int i = 0; i < len; ++i) {
    data[i] = readIn(ds1302);
  }
  stop_spi_session(ds1302);
}


bool is_time_less_than(Time* t1, Time* t2){
  if(t1->yr < t2->yr) return true;
  else if(t1->yr > t2->yr) return false;

  if(t1->mon < t2->mon) return true;
  else if(t1->mon > t2->mon) return false;

  if(t1->date < t2->date) return true;
  else if(t1->date > t2->date) return false;

  if(t1->hr < t2->hr) return true;
  else if(t1->hr > t2->hr) return false;

  if(t1->min < t2->min) return true;
  else if(t1->min > t2->min) return false;

  if(t1->sec < t2->sec) return true;
  else if(t1->sec > t2->sec) return false;

  return true;
}

bool is_time_equal(Time* t1, Time* t2){
  if(t1->yr == t2->yr &&
     t1->mon == t2->mon &&
     t1->date == t2->date &&
     t1->hr == t2->hr &&
     t1->min == t2->min &&
     t1->sec == t2->sec) {
    return true;
  }
  return false;
}

uint32_t time_diff(Time* t1, Time* t2){
  int32_t tv1 = 0;
  tv1 += 32140800 * (t1->yr - 2000);
  tv1 += 2678400  * t1->mon;
  tv1 += 86400 * t1->date;
  tv1 += 3600  * t1->hr;
  tv1 += 60    * t1->min;
  tv1 += 1     * t1->sec;

  int32_t tv2 = 0;
  tv2 += 32140800 * (t2->yr - 2000);
  tv2 += 2678400  * t2->mon;
  tv2 += 86400 * t2->date;
  tv2 += 3600  * t2->hr;
  tv2 += 60    * t2->min;
  tv2 += 1     * t2->sec;

  return abs(tv1 - tv2);
}

void print_time(Time* t){
  NRF_LOG_INFO("Time: %d/%d%d %d:%d:%d ", t->yr, t->mon, t->date, t->hr, t->min, t->sec);
}