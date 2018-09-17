#ifndef SPL06_H
#define SPL06_H

#include "data_type.h"


#define s32 int32
#define int16 short
#define int32 int
#define uint8 unsigned char

#define HW_ADR 0x76 //SDO LOW
#define CONTINUOUS_PRESSURE     1
#define CONTINUOUS_TEMPERATURE  2
#define CONTINUOUS_P_AND_T      3
#define PRESSURE_SENSOR     0
#define TEMPERATURE_SENSOR  1


struct SPL06_calib_param_t {	
    int16 c0;
    int16 c1;
    int32 c00;
    int32 c10;
    int16 c01;
    int16 c11;
    int16 c20;
    int16 c21;
    int16 c30;       
};

struct SPL06_t {	
    struct SPL06_calib_param_t calib_param;/**<calibration data*/	
    uint8 chip_id;	
    int32 i32rawPressure;
    int32 i32rawTemperature;
    int32 i32kP;    
    int32 i32kT;
};

extern float temperature,temperature2,baro_Offset;
extern unsigned char baro_start;

void SPL06_init(void);
void SPL06_rateset(uint8 iSensor, uint8 u8OverSmpl, uint8 u8SmplRate);
void SPL06_start_temperature(void);
void SPL06_start_pressure(void);
void SPL06_start_continuous(uint8 mode);
void SPL06_get_raw_temp(void);
void SPL06_get_raw_pressure(void);
float SPL06_get_temperature(void);
float SPL06_get_pressure(void);
void user_SPL06_get(void);

#endif

