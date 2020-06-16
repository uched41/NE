#ifndef DS1302_H_
#define DS1302_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

typedef enum{
    kSunday    = 1,
    kMonday    = 2,
    kTuesday   = 3,
    kWednesday = 4,
    kThursday  = 5,
    kFriday    = 6,
    kSaturday  = 7
}Day;

typedef struct  {
  uint8_t sec;
  uint8_t min;
  uint8_t hr;
  uint8_t date;
  uint8_t mon;
  Day day;
  uint16_t yr;
} Time;

typedef struct {
  int kRamSize;
  uint8_t ce_pin_;
  uint8_t io_pin_;
  uint8_t sclk_pin_;
} ds1302_t;

extern ds1302_t mrtc;
extern Time global_time;

// Create a time object with a given time
void rtc_make_time(Time* dtime, uint16_t yr, uint8_t mon, uint8_t date, uint8_t hr, uint8_t min, uint8_t sec, Day day);

void rtc_init(ds1302_t* ds1302);

void rtc_get_time(ds1302_t* ds1302);

void rtc_set_time(ds1302_t* ds1302, Time* t);

void rtc_halt(ds1302_t* ds1302, uint8_t value);

void print_time(Time* t);

#endif  // DS1302_H_