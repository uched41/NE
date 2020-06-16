#ifndef __LIS3DH_DRIVER__H
#define __LIS3DH_DRIVER__H

#ifndef __ARCHDEP__TYPES
#define __ARCHDEP__TYPES

typedef unsigned char u8_t;
typedef unsigned short int u16_t;
typedef short int i16_t;
typedef signed char i8_t;

#endif /*__ARCHDEP__TYPES*/

typedef u8_t LIS3DH_IntPinConf_t;
typedef u8_t LIS3DH_Axis_t;
typedef u8_t LIS3DH_Int1Conf_t;


//define structure
#ifndef __SHARED__TYPES
#define __SHARED__TYPES

typedef enum {
  MEMS_SUCCESS				=		0x01,
  MEMS_ERROR				=		0x00	
} result_t;

typedef enum {
  MEMS_ENABLE				=		0x01,
  MEMS_DISABLE				=		0x00	
} State_t;

typedef struct {
  i16_t AXIS_X;
  i16_t AXIS_Y;
  i16_t AXIS_Z;
} AxesRaw_t;

extern AxesRaw_t accel_data;

#endif /*__SHARED__TYPES*/

typedef enum {  
  LIS3DH_ODR_1Hz		        =		0x01,		
  LIS3DH_ODR_10Hz                      =		0x02,
  LIS3DH_ODR_25Hz		        =		0x03,
  LIS3DH_ODR_50Hz		        =		0x04,
  LIS3DH_ODR_100Hz		        =		0x05,	
  LIS3DH_ODR_200Hz		        =		0x06,
  LIS3DH_ODR_400Hz		        =		0x07,
  LIS3DH_ODR_1620Hz_LP		        =		0x08,
  LIS3DH_ODR_1344Hz_NP_5367HZ_LP       =		0x09	
} LIS3DH_ODR_t;

typedef enum {
  LIS3DH_POWER_DOWN                    =		0x00,
  LIS3DH_LOW_POWER 			=		0x01,
  LIS3DH_NORMAL			=		0x02
} LIS3DH_Mode_t;

typedef enum {
  LIS3DH_HPM_NORMAL_MODE_RES           =               0x00,
  LIS3DH_HPM_REF_SIGNAL                =               0x01,
  LIS3DH_HPM_NORMAL_MODE               =               0x02,
  LIS3DH_HPM_AUTORESET_INT             =               0x03
} LIS3DH_HPFMode_t;

typedef enum {
  LIS3DH_HPFCF_0                       =               0x00,
  LIS3DH_HPFCF_1                       =               0x01,
  LIS3DH_HPFCF_2                       = 		0x02,
  LIS3DH_HPFCF_3                       =               0x03
} LIS3DH_HPFCutOffFreq_t;

typedef struct {
  u16_t AUX_1;
  u16_t AUX_2;
  u16_t AUX_3;
} LIS3DH_Aux123Raw_t;

typedef enum {
  LIS3DH_FULLSCALE_2                   =               0x00,
  LIS3DH_FULLSCALE_4                   =               0x01,
  LIS3DH_FULLSCALE_8                   =               0x02,
  LIS3DH_FULLSCALE_16                  =               0x03
} LIS3DH_Fullscale_t;

typedef enum {
  LIS3DH_BLE_LSB			=		0x00,
  LIS3DH_BLE_MSB			=		0x01
} LIS3DH_Endianess_t;

typedef enum {
  LIS3DH_SELF_TEST_DISABLE             =               0x00,
  LIS3DH_SELF_TEST_0                   =               0x01,
  LIS3DH_SELF_TEST_1                   =               0x02
} LIS3DH_SelfTest_t;

typedef enum {
  LIS3DH_FIFO_BYPASS_MODE              =               0x00,
  LIS3DH_FIFO_MODE                     =               0x01,
  LIS3DH_FIFO_STREAM_MODE              =               0x02,
  LIS3DH_FIFO_TRIGGER_MODE             =               0x03,
  LIS3DH_FIFO_DISABLE                  =               0x04
} LIS3DH_FifoMode_t;

typedef enum {
  LIS3DH_TRIG_INT1                     =		0x00,
  LIS3DH_TRIG_INT2 			=		0x01
} LIS3DH_TrigInt_t;

typedef enum {
  LIS3DH_SPI_4_WIRE                    =               0x00,
  LIS3DH_SPI_3_WIRE                    =               0x01
} LIS3DH_SPIMode_t;

typedef enum {
  LIS3DH_X_ENABLE                      =               0x01,
  LIS3DH_X_DISABLE                     =               0x00,
  LIS3DH_Y_ENABLE                      =               0x02,
  LIS3DH_Y_DISABLE                     =               0x00,
  LIS3DH_Z_ENABLE                      =               0x04,
  LIS3DH_Z_DISABLE                     =               0x00    
} LIS3DH_AXISenable_t;

typedef enum {
  LIS3DH_INT1_6D_4D_DISABLE            =               0x00,
  LIS3DH_INT1_6D_ENABLE                =               0x01,
  LIS3DH_INT1_4D_ENABLE                =               0x02 
} LIS3DH_INT_6D_4D_t;

typedef enum {
  LIS3DH_UP_SX                         =               0x44,
  LIS3DH_UP_DX                         =               0x42,
  LIS3DH_DW_SX                         =               0x41,
  LIS3DH_DW_DX                         =               0x48,
  LIS3DH_TOP                           =               0x60,
  LIS3DH_BOTTOM                        =               0x50
} LIS3DH_POSITION_6D_t;

typedef enum {
  LIS3DH_INT_MODE_OR                   =               0x00,
  LIS3DH_INT_MODE_6D_MOVEMENT          =               0x01,
  LIS3DH_INT_MODE_AND                  =               0x02,
  LIS3DH_INT_MODE_6D_POSITION          =               0x03  
} LIS3DH_Int1Mode_t;


//interrupt click response
//  b7 = don't care   b6 = IA  b5 = DClick  b4 = Sclick  b3 = Sign  
//  b2 = z      b1 = y     b0 = x
typedef enum {
LIS3DH_DCLICK_Z_P                      =               0x24,
LIS3DH_DCLICK_Z_N                      =               0x2C,
LIS3DH_SCLICK_Z_P                      =               0x14,
LIS3DH_SCLICK_Z_N                      =               0x1C,
LIS3DH_DCLICK_Y_P                      =               0x22,
LIS3DH_DCLICK_Y_N                      =               0x2A,
LIS3DH_SCLICK_Y_P                      =               0x12,
LIS3DH_SCLICK_Y_N			=		0x1A,
LIS3DH_DCLICK_X_P                      =               0x21,
LIS3DH_DCLICK_X_N                      =               0x29,
LIS3DH_SCLICK_X_P                      =               0x11,
LIS3DH_SCLICK_X_N                      =               0x19,
LIS3DH_NO_CLICK                        =               0x00
} LIS3DH_Click_Response; 

/* Exported macro ------------------------------------------------------------*/

#ifndef __SHARED__MACROS

#define __SHARED__MACROS
#define ValBit(VAR,Place)         (VAR & (1<<Place))
#define BIT(x) ( (x) )

#endif /*__SHARED__MACROS*/

/* Exported functions --------------------------------------------------------*/
//Sensor Configuration Functions
result_t LIS3DH_SetODR(LIS3DH_ODR_t ov);
result_t LIS3DH_SetMode(LIS3DH_Mode_t md);
result_t LIS3DH_SetAxis(LIS3DH_Axis_t axis);
result_t LIS3DH_SetFullScale(LIS3DH_Fullscale_t fs);
result_t LIS3DH_SetBDU(State_t bdu);
result_t LIS3DH_SetBLE(LIS3DH_Endianess_t ble);
result_t LIS3DH_SetSelfTest(LIS3DH_SelfTest_t st);
result_t LIS3DH_SetTemperature(State_t state);
result_t LIS3DH_SetADCAux(State_t state);

//Filtering Functions
result_t LIS3DH_HPFClickEnable(State_t hpfe);
result_t LIS3DH_HPFAOI1Enable(State_t hpfe);
result_t LIS3DH_HPFAOI2Enable(State_t hpfe);
result_t LIS3DH_SetHPFMode(LIS3DH_HPFMode_t hpf);
result_t LIS3DH_SetHPFCutOFF(LIS3DH_HPFCutOffFreq_t hpf);
result_t LIS3DH_SetFilterDataSel(State_t state);

//Interrupt Functions
result_t LIS3DH_SetInt1Pin(LIS3DH_IntPinConf_t pinConf);
result_t LIS3DH_SetInt2Pin(LIS3DH_IntPinConf_t pinConf);
result_t LIS3DH_Int1LatchEnable(State_t latch);
result_t LIS3DH_ResetInt1Latch(void);
result_t LIS3DH_SetIntConfiguration(LIS3DH_Int1Conf_t ic);
result_t LIS3DH_SetInt1Threshold(u8_t ths);
result_t LIS3DH_SetInt1Duration(LIS3DH_Int1Conf_t id);
result_t LIS3DH_SetIntMode(LIS3DH_Int1Mode_t ic);
result_t LIS3DH_SetClickCFG(u8_t status);
result_t LIS3DH_SetInt6D4DConfiguration(LIS3DH_INT_6D_4D_t ic);
result_t LIS3DH_GetInt1Src(u8_t* val);
result_t LIS3DH_GetInt1SrcBit(u8_t statusBIT, u8_t* val);

//FIFO Functions
result_t LIS3DH_FIFOModeEnable(LIS3DH_FifoMode_t fm);
result_t LIS3DH_SetWaterMark(u8_t wtm);
result_t LIS3DH_SetTriggerInt(LIS3DH_TrigInt_t tr);
result_t LIS3DH_GetFifoSourceReg(u8_t* val);
result_t LIS3DH_GetFifoSourceBit(u8_t statusBIT, u8_t* val);
result_t LIS3DH_GetFifoSourceFSS(u8_t* val);

//Other Reading Functions
result_t LIS3DH_GetStatusReg(u8_t* val);
result_t LIS3DH_GetStatusBit(u8_t statusBIT, u8_t* val);
result_t LIS3DH_GetStatusAUXBit(u8_t statusBIT, u8_t* val);
result_t LIS3DH_GetStatusAUX(u8_t* val);
result_t LIS3DH_GetAccAxesRaw(AxesRaw_t* buff);
result_t LIS3DH_GetAuxRaw(LIS3DH_Aux123Raw_t* buff);
result_t LIS3DH_GetClickResponse(u8_t* val);
result_t LIS3DH_GetTempRaw(i8_t* val);
result_t LIS3DH_GetWHO_AM_I(u8_t* val);
result_t LIS3DH_Get6DPosition(u8_t* val);

void LIS3DH_Init(void);
uint8_t SetLIS3DHActivityDetection(uint8_t Th, LIS3DH_Int1Mode_t Mode, uint8_t OnOff);


#endif /* __LIS3DH_H */

