#include "nrf_log.h"
#include "nrf_drv_gpiote.h"

#include "hardware.h"
#include "accel_driver.h"
#include "accel_reg.h"
#include "accel_hal.h"
#include "utils.h"

static uint8_t _addr = ICM20600_I2C_ADDR1;
static uint8_t _buffer[16];
static uint16_t _acc_scale, _gyro_scale;


static int16_t getAccelerationX(void);
static int16_t getAccelerationY(void);
static int16_t getAccelerationZ(void);
static int16_t getRawAccelerationX(void);
static int16_t getRawAccelerationY(void);
static int16_t getRawAccelerationZ(void);

static int16_t getGyroscopeX(void);
static int16_t getGyroscopeY(void);
static int16_t getGyroscopeZ(void);
static int16_t getRawGyroscopeX(void);
static int16_t getRawGyroscopeY(void);
static int16_t getRawGyroscopeZ(void);

static void setPowerMode(icm20600_power_type_t mode);
static void setSampleRateDivier(uint8_t div);
static void setAccScaleRange(acc_scale_type_t range);
static void setAccAverageSample(acc_averaging_sample_type_t sample);
static void setAccOutputDataRate(acc_lownoise_odr_type_t odr);
static void setGyroScaleRange(gyro_scale_type_t range);
static void setGyroAverageSample(gyro_averaging_sample_type_t sample);
static void setGyroOutputDataRate(gyro_lownoise_odr_type_t odr);

uint8_t accel_get_device_id() {
    nrf_drv_mpu_read_registers(ICM20600_WHO_AM_I, _buffer, 1);
    return _buffer[0];
}

void accel_cb(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action){

}

void init_accel() {
    ret_code_t err_code;
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

    err_code = nrf_drv_gpiote_in_init(MPU_INT, &in_config, accel_cb);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(MPU_INT, true);

    // configuration
    nrf_drv_mpu_write_single_register(ICM20600_CONFIG, 0x00);
    // disable fifo
    nrf_drv_mpu_write_single_register(ICM20600_FIFO_EN, 0x00);

    // set default power mode
    setPowerMode(ICM_6AXIS_LOW_POWER);

    // gyro config
    setGyroScaleRange(RANGE_2K_DPS);
    setGyroOutputDataRate(GYRO_RATE_1K_BW_176);
    setGyroAverageSample(GYRO_AVERAGE_1);

    // accel config
    setAccScaleRange(RANGE_16G);
    setAccOutputDataRate(ACC_RATE_1K_BW_420);
    setAccAverageSample(ACC_AVERAGE_4);
}


void setPowerMode(icm20600_power_type_t mode) {
    uint8_t data_pwr1;
    uint8_t data_pwr2 = 0x00;
    uint8_t data_gyro_lp;
    nrf_drv_mpu_read_registers(ICM20600_PWR_MGMT_1, _buffer, 1);
    data_pwr1 = _buffer[0];
    data_pwr1 &= 0x8f;                  // 0b10001111
    nrf_drv_mpu_read_registers(ICM20600_GYRO_LP_MODE_CFG, _buffer,1);
    data_gyro_lp = _buffer[0];
    // When set to ‘1’ low-power gyroscope mode is enabled. Default setting is 0
    data_gyro_lp &= 0x7f;               // 0b01111111
    switch (mode) {
        case ICM_SLEEP_MODE:
            data_pwr1 |= 0x40;          // set 0b01000000
            break;

        case ICM_STANDYBY_MODE:
            data_pwr1 |= 0x10;          // set 0b00010000
            data_pwr2 = 0x38;           // 0x00111000 disable acc
            break;

        case ICM_ACC_LOW_POWER:
            data_pwr1 |= 0x20;          // set bit5 0b00100000
            data_pwr2 = 0x07;           //0x00000111 disable gyro
            break;

        case ICM_ACC_LOW_NOISE:
            data_pwr1 |= 0x00;
            data_pwr2 = 0x07;           //0x00000111 disable gyro
            break;

        case ICM_GYRO_LOW_POWER:
            data_pwr1 |= 0x00;          // dont set bit5 0b00000000
            data_pwr2 = 0x38;           // 0x00111000 disable acc
            data_gyro_lp |= 0x80;
            break;

        case ICM_GYRO_LOW_NOISE:
            data_pwr1 |= 0x00;
            data_pwr2 = 0x38;           // 0x00111000 disable acc
            break;

        case ICM_6AXIS_LOW_POWER:
            data_pwr1 |= 0x00;          // dont set bit5 0b00100000
            data_gyro_lp |= 0x80;
            break;

        case ICM_6AXIS_LOW_NOISE:
            data_pwr1 |= 0x00;
            break;

        default:
            break;
    }
    nrf_drv_mpu_write_single_register(ICM20600_PWR_MGMT_1, data_pwr1);
    nrf_drv_mpu_write_single_register(ICM20600_PWR_MGMT_2, data_pwr2);
    nrf_drv_mpu_write_single_register(ICM20600_GYRO_LP_MODE_CFG, data_gyro_lp);
}

// SAMPLE_RATE = 1KHz / (1 + div)
// work for low-power gyroscope and low-power accelerometer and low-noise accelerometer
void setSampleRateDivier(uint8_t div) {
    nrf_drv_mpu_write_single_register(ICM20600_SMPLRT_DIV, div);
}


void setAccScaleRange(acc_scale_type_t range) {
    uint8_t data;
    nrf_drv_mpu_read_registers(ICM20600_ACCEL_CONFIG, _buffer, 1);
    data = _buffer[0];
    data &= 0xe7; // 0b 1110 0111

    switch (range) {
        case RANGE_2G:
            data |= 0x00;   // 0bxxx00xxx
            _acc_scale = 4000;
            break;

        case RANGE_4G:
            data |= 0x08;   // 0bxxx01xxx
            _acc_scale = 8000;
            break;

        case RANGE_8G:
            data |= 0x10;   // 0bxxx10xxx
            _acc_scale = 16000;
            break;

        case RANGE_16G:
            data |= 0x18;   // 0bxxx11xxx
            _acc_scale = 32000;
            break;

        default:
            break;
    }

    nrf_drv_mpu_write_single_register(ICM20600_ACCEL_CONFIG, data);
}


// for low power mode only
void setAccAverageSample(acc_averaging_sample_type_t sample) {
    uint8_t data = 0;
    nrf_drv_mpu_read_registers(ICM20600_ACCEL_CONFIG2, _buffer, 1);
    data = _buffer[0];

    data &= 0xcf; // & 0b11001111
    switch (sample) {
        case ACC_AVERAGE_4:
            data |= 0x00; // 0bxx00xxxx
            break;

        case ACC_AVERAGE_8:
            data |= 0x10; // 0bxx01xxxx
            break;

        case ACC_AVERAGE_16:
            data |= 0x20; // 0bxx10xxxx
            break;

        case ACC_AVERAGE_32:
            data |= 0x30; // 0bxx11xxxx
            break;

        default:
            break;
    }

    nrf_drv_mpu_write_single_register(ICM20600_ACCEL_CONFIG2, data);
}


void setAccOutputDataRate(acc_lownoise_odr_type_t odr) {
    uint8_t data;
    nrf_drv_mpu_read_registers(ICM20600_ACCEL_CONFIG2, _buffer, 1);
    data = _buffer[0];
    data &= 0xf0;  // 0b11110000

    switch (odr) {
        case ACC_RATE_4K_BW_1046:
            data |= 0x08;
            break;

        case ACC_RATE_1K_BW_420:
            data |= 0x07;
            break;

        case ACC_RATE_1K_BW_218:
            data |= 0x01;
            break;

        case ACC_RATE_1K_BW_99:
            data |= 0x02;
            break;

        case ACC_RATE_1K_BW_44:
            data |= 0x03;
            break;

        case ACC_RATE_1K_BW_21:
            data |= 0x04;
            break;

        case ACC_RATE_1K_BW_10:
            data |= 0x05;
            break;

        case ACC_RATE_1K_BW_5:
            data |= 0x06;
            break;

        default:
            break;
    }

    nrf_drv_mpu_write_single_register(ICM20600_ACCEL_CONFIG2, data);
}


void setGyroScaleRange(gyro_scale_type_t range) {
    uint8_t data = 0;
    nrf_drv_mpu_read_registers(ICM20600_GYRO_CONFIG, _buffer, 1);
    data = _buffer[0];
    data &= 0xe7; // 0b11100111

    switch (range) {
        case RANGE_250_DPS:
            data |= 0x00;   // 0bxxx00xxx
            _gyro_scale = 500;
            break;

        case RANGE_500_DPS:
            data |= 0x08;   // 0bxxx00xxx
            _gyro_scale = 1000;
            break;

        case RANGE_1K_DPS:
            data |= 0x10;   // 0bxxx10xxx
            _gyro_scale = 2000;
            break;

        case RANGE_2K_DPS:
            data |= 0x18;   // 0bxxx11xxx
            _gyro_scale = 4000;
            break;

        default:
            break;
    }

    nrf_drv_mpu_write_single_register(ICM20600_GYRO_CONFIG, data);
}


// for low power mode only
void setGyroAverageSample(gyro_averaging_sample_type_t sample) {
    uint8_t data = 0;
    nrf_drv_mpu_read_registers(ICM20600_GYRO_LP_MODE_CFG, _buffer, 1);
    data = _buffer[0];

    data &= 0x8f;           // 0b10001111
    switch (sample) {
        case GYRO_AVERAGE_1:
            data |= 0x00; // 0bx000xxxx
            break;

        case GYRO_AVERAGE_2:
            data |= 0x10; // 0bx001xxxx
            break;

        case GYRO_AVERAGE_4:
            data |= 0x20; // 0bx010xxxx
            break;

        case GYRO_AVERAGE_8:
            data |= 0x30; // 0bx011xxxx
            break;

        case GYRO_AVERAGE_16:
            data |= 0x40; // 0bx100xxxx
            break;

        case GYRO_AVERAGE_32:
            data |= 0x50; // 0bx101xxxx
            break;

        case GYRO_AVERAGE_64:
            data |= 0x60;
            break;

        case GYRO_AVERAGE_128:
            data |= 0x70;
            break;


        default:
            break;
    }

    nrf_drv_mpu_write_single_register(ICM20600_GYRO_LP_MODE_CFG, data);
}



void setGyroOutputDataRate(gyro_lownoise_odr_type_t odr) {
    uint8_t data;
    nrf_drv_mpu_read_registers(ICM20600_CONFIG, _buffer, 1);
    data = _buffer[0];
    data &= 0xf8;  // DLPF_CFG[2:0] 0b11111000

    switch (odr) {
        case GYRO_RATE_8K_BW_3281:
            data |= 0x07;
            break;
        case GYRO_RATE_8K_BW_250:
            data |= 0x00;
            break;
        case GYRO_RATE_1K_BW_176:
            data |= 0x01;
            break;
        case GYRO_RATE_1K_BW_92:
            data |= 0x02;
            break;
        case GYRO_RATE_1K_BW_41:
            data |= 0x03;
            break;
        case GYRO_RATE_1K_BW_20:
            data |= 0x04;
            break;
        case GYRO_RATE_1K_BW_10:
            data |= 0x05;
            break;
        case GYRO_RATE_1K_BW_5:
            data |= 0x06;
            break;
    }

    nrf_drv_mpu_write_single_register(ICM20600_CONFIG, data);
}

void get_accel_values(accel_values_t* val) {
    val->x = getAccelerationX();
    val->y = getAccelerationY();
    val->z = getAccelerationZ();
}

int16_t getAccelerationX(void) {
    int32_t raw_data = getRawAccelerationX();
    raw_data = (raw_data * _acc_scale) >> 16;
    return (int16_t)raw_data;
}
int16_t getAccelerationY(void) {
    int32_t raw_data = getRawAccelerationY();
    raw_data = (raw_data * _acc_scale) >> 16;
    return (int16_t)raw_data;
}
int16_t getAccelerationZ(void) {
    int32_t raw_data = getRawAccelerationZ();
    raw_data = (raw_data * _acc_scale) >> 16;
    return (int16_t)raw_data;
}

int16_t getRawAccelerationX(void) {
    nrf_drv_mpu_read_registers(ICM20600_ACCEL_XOUT_H, _buffer, 2);
    return ((int16_t)_buffer[0] << 8) + _buffer[1];
}

int16_t getRawAccelerationY(void) {
    nrf_drv_mpu_read_registers(ICM20600_ACCEL_YOUT_H, _buffer, 2);
    return ((int16_t)_buffer[0] << 8) + _buffer[1];
}

int16_t getRawAccelerationZ(void) {
    nrf_drv_mpu_read_registers(ICM20600_ACCEL_ZOUT_H, _buffer, 2);
    return ((int16_t)_buffer[0] << 8) + _buffer[1];
}

void get_gyro_values(gyro_values_t* val) {
    val->x = getGyroscopeX();
    val->y = getGyroscopeY();
    val->z = getGyroscopeZ();
}

int16_t getGyroscopeX(void) {
    int32_t raw_data = getRawGyroscopeX();
    raw_data = (raw_data * _gyro_scale) >> 16;
    return (int16_t)raw_data;
}

int16_t getGyroscopeY(void) {
    int32_t raw_data = getRawGyroscopeY();
    raw_data = (raw_data * _gyro_scale) >> 16;
    return (int16_t)raw_data;
}

int16_t getGyroscopeZ(void) {
    int32_t raw_data = getRawGyroscopeZ();
    raw_data = (raw_data * _gyro_scale) >> 16;
    return (int16_t)raw_data;
}

int16_t getRawGyroscopeX(void) {
    nrf_drv_mpu_read_registers(ICM20600_GYRO_XOUT_H, _buffer, 2);
    return ((int16_t)_buffer[0] << 8) + _buffer[1];
}

int16_t getRawGyroscopeY(void) {
    nrf_drv_mpu_read_registers(ICM20600_GYRO_YOUT_H, _buffer, 2);
    return ((int16_t)_buffer[0] << 8) + _buffer[1];
}

int16_t getRawGyroscopeZ(void) {
    nrf_drv_mpu_read_registers(ICM20600_GYRO_ZOUT_H, _buffer, 2);
    return ((int16_t)_buffer[0] << 8) + _buffer[1];
}

void accel_reset() {
    uint8_t data;
    nrf_drv_mpu_read_registers(ICM20600_USER_CTRL, _buffer, 1);
    data = _buffer[0];
    data &= 0xfe;  // ICM20600_USER_CTRL[0] 0b11111110
    data |= ICM20600_RESET_BIT;
    nrf_drv_mpu_write_single_register(ICM20600_USER_CTRL, data);
}

void test_accel(){
    accel_values_t a;
    gyro_values_t g;

    get_accel_values(&a);
    get_gyro_values(&g);
    mprintf("Test Accel", "ax: %d\t ay: %d\t az: %d", a.x, a.y, a.z);
    mprintf("Test Accel", "gx: %d\t gy: %d\t gz: %d", g.x, g.y, g.z);
}