#ifndef _LIS3DH_REG_
#define _LIS3DH_REG_

#include <stdint.h>

#ifndef __SHARED__CONSTANTS
#define __SHARED__CONSTANTS

#define MEMS_SET                                        0x01
#define MEMS_RESET                                      0x00

#endif /*__SHARED__CONSTANTS*/


//Register Definition
#define LIS3DH_WHO_AM_I				0x0F  // device identification register

// CONTROL REGISTER 1
#define LIS3DH_CTRL_REG1				0x20
#define LIS3DH_ODR_BIT				        BIT(4)
#define LIS3DH_LPEN					BIT(3)
#define LIS3DH_ZEN					BIT(2)
#define LIS3DH_YEN					BIT(1)
#define LIS3DH_XEN					BIT(0)

//CONTROL REGISTER 2
#define LIS3DH_CTRL_REG2				0x21
#define LIS3DH_HPM     				BIT(6)
#define LIS3DH_HPCF					BIT(4)
#define LIS3DH_FDS					BIT(3)
#define LIS3DH_HPCLICK					BIT(2)
#define LIS3DH_HPIS2					BIT(1)
#define LIS3DH_HPIS1					BIT(0)

//CONTROL REGISTER 3
#define LIS3DH_CTRL_REG3				0x22
#define LIS3DH_I1_CLICK				BIT(7)
#define LIS3DH_I1_AOI1					BIT(6)
#define LIS3DH_I1_AOI2				        BIT(5)
#define LIS3DH_I1_DRDY1				BIT(4)
#define LIS3DH_I1_DRDY2				BIT(3)
#define LIS3DH_I1_WTM					BIT(2)
#define LIS3DH_I1_ORUN					BIT(1)

//CONTROL REGISTER 6
#define LIS3DH_CTRL_REG6				0x25
#define LIS3DH_I2_CLICK				BIT(7)
#define LIS3DH_I2_INT1					BIT(6)
#define LIS3DH_I2_BOOT         			BIT(4)
#define LIS3DH_H_LACTIVE				BIT(1)

//TEMPERATURE CONFIG REGISTER
#define LIS3DH_TEMP_CFG_REG				0x1F
#define LIS3DH_ADC_PD				        BIT(7)
#define LIS3DH_TEMP_EN					BIT(6)

//CONTROL REGISTER 4
#define LIS3DH_CTRL_REG4				0x23
#define LIS3DH_BDU					BIT(7)
#define LIS3DH_BLE					BIT(6)
#define LIS3DH_FS					BIT(4)
#define LIS3DH_HR					BIT(3)
#define LIS3DH_ST       				BIT(1)
#define LIS3DH_SIM					BIT(0)

//CONTROL REGISTER 5
#define LIS3DH_CTRL_REG5				0x24
#define LIS3DH_BOOT                                    BIT(7)
#define LIS3DH_FIFO_EN                                 BIT(6)
#define LIS3DH_LIR_INT1                                BIT(3)
#define LIS3DH_D4D_INT1                                BIT(2)

//REFERENCE/DATA_CAPTURE
#define LIS3DH_REFERENCE_REG		                0x26
#define LIS3DH_REF		                	BIT(0)

//STATUS_REG_AXIES
#define LIS3DH_STATUS_REG				0x27
#define LIS3DH_ZYXOR                                   BIT(7)
#define LIS3DH_ZOR                                     BIT(6)
#define LIS3DH_YOR                                     BIT(5)
#define LIS3DH_XOR                                     BIT(4)
#define LIS3DH_ZYXDA                                   BIT(3)
#define LIS3DH_ZDA                                     BIT(2)
#define LIS3DH_YDA                                     BIT(1)
#define LIS3DH_XDA                                     BIT(0)

//STATUS_REG_AUX
#define LIS3DH_STATUS_AUX				0x07

//INTERRUPT 1 CONFIGURATION
#define LIS3DH_INT1_CFG				0x30
#define LIS3DH_ANDOR                                   BIT(7)
#define LIS3DH_INT_6D                                  BIT(6)
#define LIS3DH_ZHIE                                    BIT(5)
#define LIS3DH_ZLIE                                    BIT(4)
#define LIS3DH_YHIE                                    BIT(3)
#define LIS3DH_YLIE                                    BIT(2)
#define LIS3DH_XHIE                                    BIT(1)
#define LIS3DH_XLIE                                    BIT(0)

//FIFO CONTROL REGISTER
#define LIS3DH_FIFO_CTRL_REG                           0x2E
#define LIS3DH_FM                                      BIT(6)
#define LIS3DH_TR                                      BIT(5)
#define LIS3DH_FTH                                     BIT(0)

//CONTROL REG3 bit mask
#define LIS3DH_CLICK_ON_PIN_INT1_ENABLE                0x80
#define LIS3DH_CLICK_ON_PIN_INT1_DISABLE               0x00
#define LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE              0x40
#define LIS3DH_I1_INT1_ON_PIN_INT1_DISABLE             0x00
#define LIS3DH_I1_INT2_ON_PIN_INT1_ENABLE              0x20
#define LIS3DH_I1_INT2_ON_PIN_INT1_DISABLE             0x00
#define LIS3DH_I1_DRDY1_ON_INT1_ENABLE                 0x10
#define LIS3DH_I1_DRDY1_ON_INT1_DISABLE                0x00
#define LIS3DH_I1_DRDY2_ON_INT1_ENABLE                 0x08
#define LIS3DH_I1_DRDY2_ON_INT1_DISABLE                0x00
#define LIS3DH_WTM_ON_INT1_ENABLE                      0x04
#define LIS3DH_WTM_ON_INT1_DISABLE                     0x00
#define LIS3DH_INT1_OVERRUN_ENABLE                     0x02
#define LIS3DH_INT1_OVERRUN_DISABLE                    0x00

//CONTROL REG6 bit mask
#define LIS3DH_CLICK_ON_PIN_INT2_ENABLE                0x80
#define LIS3DH_CLICK_ON_PIN_INT2_DISABLE               0x00
#define LIS3DH_I2_INT1_ON_PIN_INT2_ENABLE              0x40
#define LIS3DH_I2_INT1_ON_PIN_INT2_DISABLE             0x00
#define LIS3DH_I2_INT2_ON_PIN_INT2_ENABLE              0x20
#define LIS3DH_I2_INT2_ON_PIN_INT2_DISABLE             0x00
#define LIS3DH_I2_BOOT_ON_INT2_ENABLE                  0x10
#define LIS3DH_I2_BOOT_ON_INT2_DISABLE                 0x00
#define LIS3DH_INT_ACTIVE_HIGH                         0x00
#define LIS3DH_INT_ACTIVE_LOW                          0x02

//INT1_CFG bit mask
#define LIS3DH_INT1_AND                                0x80
#define LIS3DH_INT1_OR                                 0x00
#define LIS3DH_INT1_ZHIE_ENABLE                        0x20
#define LIS3DH_INT1_ZHIE_DISABLE                       0x00
#define LIS3DH_INT1_ZLIE_ENABLE                        0x10
#define LIS3DH_INT1_ZLIE_DISABLE                       0x00
#define LIS3DH_INT1_YHIE_ENABLE                        0x08
#define LIS3DH_INT1_YHIE_DISABLE                       0x00
#define LIS3DH_INT1_YLIE_ENABLE                        0x04
#define LIS3DH_INT1_YLIE_DISABLE                       0x00
#define LIS3DH_INT1_XHIE_ENABLE                        0x02
#define LIS3DH_INT1_XHIE_DISABLE                       0x00
#define LIS3DH_INT1_XLIE_ENABLE                        0x01
#define LIS3DH_INT1_XLIE_DISABLE                       0x00

//INT1_SRC bit mask
#define LIS3DH_INT1_SRC_IA                             0x40
#define LIS3DH_INT1_SRC_ZH                             0x20
#define LIS3DH_INT1_SRC_ZL                             0x10
#define LIS3DH_INT1_SRC_YH                             0x08
#define LIS3DH_INT1_SRC_YL                             0x04
#define LIS3DH_INT1_SRC_XH                             0x02
#define LIS3DH_INT1_SRC_XL                             0x01

//INT1 REGISTERS
#define LIS3DH_INT1_THS                                0x32
#define LIS3DH_INT1_DURATION                           0x33

//INTERRUPT 1 SOURCE REGISTER
#define LIS3DH_INT1_SRC				0x31

//FIFO Source Register bit Mask
#define LIS3DH_FIFO_SRC_WTM                            0x80
#define LIS3DH_FIFO_SRC_OVRUN                          0x40
#define LIS3DH_FIFO_SRC_EMPTY                          0x20
  
//INTERRUPT CLICK REGISTER
#define LIS3DH_CLICK_CFG				0x38
//INTERRUPT CLICK CONFIGURATION bit mask
#define LIS3DH_ZD_ENABLE                               0x20
#define LIS3DH_ZD_DISABLE                              0x00
#define LIS3DH_ZS_ENABLE                               0x10
#define LIS3DH_ZS_DISABLE                              0x00
#define LIS3DH_YD_ENABLE                               0x08
#define LIS3DH_YD_DISABLE                              0x00
#define LIS3DH_YS_ENABLE                               0x04
#define LIS3DH_YS_DISABLE                              0x00
#define LIS3DH_XD_ENABLE                               0x02
#define LIS3DH_XD_DISABLE                              0x00
#define LIS3DH_XS_ENABLE                               0x01
#define LIS3DH_XS_DISABLE                              0x00

//INTERRUPT CLICK SOURCE REGISTER
#define LIS3DH_CLICK_SRC                               0x39
//INTERRUPT CLICK SOURCE REGISTER bit mask
#define LIS3DH_IA                                      0x40
#define LIS3DH_DCLICK                                  0x20
#define LIS3DH_SCLICK                                  0x10
#define LIS3DH_CLICK_SIGN                              0x08
#define LIS3DH_CLICK_Z                                 0x04
#define LIS3DH_CLICK_Y                                 0x02
#define LIS3DH_CLICK_X                                 0x01

//Click-click Register
#define LIS3DH_CLICK_THS                               0x3A
#define LIS3DH_TIME_LIMIT                              0x3B
#define LIS3DH_TIME_LATENCY                            0x3C
#define LIS3DH_TIME_WINDOW                             0x3D

//OUTPUT REGISTER
#define LIS3DH_OUT_X_L					0x28
#define LIS3DH_OUT_X_H					0x29
#define LIS3DH_OUT_Y_L					0x2A
#define LIS3DH_OUT_Y_H					0x2B
#define LIS3DH_OUT_Z_L					0x2C
#define LIS3DH_OUT_Z_H					0x2D

//AUX REGISTER
#define LIS3DH_OUT_1_L					0x08
#define LIS3DH_OUT_1_H					0x09
#define LIS3DH_OUT_2_L					0x0A
#define LIS3DH_OUT_2_H					0x0B
#define LIS3DH_OUT_3_L					0x0C
#define LIS3DH_OUT_3_H					0x0D

//STATUS REGISTER bit mask
#define LIS3DH_STATUS_REG_ZYXOR                        0x80    // 1	:	new data set has over written the previous one
							// 0	:	no overrun has occurred (default)	
#define LIS3DH_STATUS_REG_ZOR                          0x40    // 0	:	no overrun has occurred (default)
							// 1	:	new Z-axis data has over written the previous one
#define LIS3DH_STATUS_REG_YOR                          0x20    // 0	:	no overrun has occurred (default)
							// 1	:	new Y-axis data has over written the previous one
#define LIS3DH_STATUS_REG_XOR                          0x10    // 0	:	no overrun has occurred (default)
							// 1	:	new X-axis data has over written the previous one
#define LIS3DH_STATUS_REG_ZYXDA                        0x08    // 0	:	a new set of data is not yet avvious one
                                                        // 1	:	a new set of data is available 
#define LIS3DH_STATUS_REG_ZDA                          0x04    // 0	:	a new data for the Z-Axis is not availvious one
                                                        // 1	:	a new data for the Z-Axis is available
#define LIS3DH_STATUS_REG_YDA                          0x02    // 0	:	a new data for the Y-Axis is not available
                                                        // 1	:	a new data for the Y-Axis is available
#define LIS3DH_STATUS_REG_XDA                          0x01    // 0	:	a new data for the X-Axis is not available

#define LIS3DH_DATAREADY_BIT                           LIS3DH_STATUS_REG_ZYXDA


//STATUS AUX REGISTER bit mask
#define LIS3DH_STATUS_AUX_321OR                         0x80
#define LIS3DH_STATUS_AUX_3OR                           0x40
#define LIS3DH_STATUS_AUX_2OR                           0x20
#define LIS3DH_STATUS_AUX_1OR                           0x10
#define LIS3DH_STATUS_AUX_321DA                         0x08
#define LIS3DH_STATUS_AUX_3DA                           0x04
#define LIS3DH_STATUS_AUX_2DA                           0x02
#define LIS3DH_STATUS_AUX_1DA                           0x01

#define LIS3DH_MEMS_I2C_ADDRESS			        0x19

//FIFO REGISTERS
#define LIS3DH_FIFO_CTRL_REG			        0x2E
#define LIS3DH_FIFO_SRC_REG			        0x2F


#endif /* __DRV_ACC_LIS3DH_TYPES_H__ */
