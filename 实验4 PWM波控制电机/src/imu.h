#ifndef __IMU_H__
#define __IMU_H__

#include <math.h>
#include "data_type.h"

// 校准时间
#define GYRO_CALC_TIME      3000000l    // us
#define M_PI_F                              3.1415926

typedef enum
{
    Rol = 0,
    Pit,
    Yaw,
    Thr,
    
    IMU_Axis_Num,
}IMU_Axis_t;

typedef struct IMU_tt
{
    uint8 caliPass;
    
    uint8 ready;
    int16 accADC[3];
    int16 gyroADC[3];
    int16 magADC[3];
    float accRaw[3];        // m/s^2
    float gyroRaw[3];       // rad/s
    float magRaw[3];        //
    float accOffset[3];     // m/s^2
    float gyrOffset[3];
    float accb[3];          // filted, in body frame
    float accg[3];
    float gyro[3];
    float DCMgb[3][3];
    float q[4];
    
    float rol_deg;             // deg
    float pit_deg;
    float yaw_deg;
    
    float rol_rad;          // rad
    float pit_rad;
    float yaw_rad;
    
    uint8 Alarm;
} imu_t;

extern imu_t imu;
extern uint8 imuCaliFlag;

extern uint8 IMU_Calibrate(void);
extern void ReadIMUSensorHandle(void);
extern uint8 IMUCheck(void);

#endif  // __IMU_H__

