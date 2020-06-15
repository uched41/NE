#ifndef _I2C_MULTI_DRIVER_H
#define _I2C_MULTI_DRIVER_H

typedef struct{
  uint8_t channel_no;
  uint8_t addr;
}i2c_sensor_t;

void i2c_sensor_select(i2c_sensor_t* sensor);

uint8_t i2c_sensor_read_byte(i2c_sensor_t* sensor, uint8_t reg);

void i2c_sensor_read(i2c_sensor_t* sensor, uint8_t reg, uint8_t* data, uint8_t len);

void i2c_sensor_write_byte(i2c_sensor_t* sensor, uint8_t reg, uint8_t data);

void i2c_sensor_write(i2c_sensor_t* sensor, uint8_t reg, uint8_t* data, uint8_t len);

#endif