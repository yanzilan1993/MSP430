#include "imu.h"
#include "mpu6050.h"
#include "filter.h"
#include "failSafe.h"
#include <string.h>
#include "extern_variable.h"
#include "timer.h"

#define CONSTANTS_ONE_G     9.80665f    // m/s^2

#define SENSOR_MAX_G        8.0f        // tobe fixed to 8g. but IMU need to correct at the same time
#define SENSOR_MAX_W        2000.0f     // deg/s
#define ACC_SCALE           (SENSOR_MAX_G / 32768.0f)
#define GYR_SCALE           (SENSOR_MAX_W / 32768.0f)

extern imu_t imu = {0};
extern uint8 imuCaliFlag = 0;

// should place to a level surface and keep it stop for 1~2 second return 1 when finish
extern uint8 IMU_Calibrate(void)
{
    // 3s
    static float AccSum[3] = {0, 0, 0};
    static float GyrSum[3] = {0, 0, 0};
    static uint16_t Cnt = 0;
    static uint16_t tPrev = 0;
    static uint8 calibrating = 0;
    
    uint8 ret = 0;
    uint8 i = 0;
    uint64 dt = 0;
    uint64 now = 0;

    // remember_to_update_gzj
    now = millis();
    dt = now - tPrev;

    if(calibrating == 0)
    {
        calibrating = 1;
        
        for(i=0; i<3; i++)
        {
            AccSum[i] = 0;
            GyrSum[i] = 0;
            Cnt=0;
            imu.ready = 0;
        }
    }

    if(dt >= 10)		// 如果dt大于10ms
    {
        if(Cnt < 300)
        {
            for(i=0; i<3; i++)
            {
                AccSum[i] += imu.accRaw[i];
                GyrSum[i] += imu.gyroRaw[i];
            }
            
            Cnt++;
            tPrev = now;
        }
        else
        {
            // 配合上面的cnt，这个地方为求平均值
            for(i=0; i<3; i++)
            {
                imu.accOffset[i] = AccSum[i] / (float)Cnt;
                imu.gyrOffset[i] = GyrSum[i] / (float)Cnt;
            }

            // 计算加速度计补偿，计算完成以后准备imu
            imu.accOffset[2] = imu.accOffset[2] - CONSTANTS_ONE_G;
            calibrating = 0;
#ifndef IMU_SW
            imu.ready = 1;
#endif
            ret = 1;
        }
    }
    
    return ret;
}

void ReadIMUSensorHandle(void)
{
    uint8 i;

    // read raw
    MPU6050_Read_Accelerometer(imu.accADC);
    MPU6050_Read_Gyroscope(imu.gyroADC);
    
    // tutn to physical
    for(i=0; i<3; i++)
    {
        imu.accRaw[i] = (float)imu.accADC[i] *ACC_SCALE * CONSTANTS_ONE_G ;
        imu.gyroRaw[i] = (float)imu.gyroADC[i] * GYR_SCALE * M_PI_F /180.f;		//deg/s
    }

    imu.accb[0] = LPF2pApply_1(imu.accRaw[0] - imu.accOffset[0]);
    imu.accb[1] = LPF2pApply_2(imu.accRaw[1] - imu.accOffset[1]);
    imu.accb[2] = LPF2pApply_3(imu.accRaw[2] - imu.accOffset[2]);
    
    #ifdef  IMU_SW
    imu.gyro[0]=LPF2pApply_4(imu.gyroRaw[0]);
    imu.gyro[1]=LPF2pApply_5(imu.gyroRaw[1]);
    imu.gyro[2]=LPF2pApply_6(imu.gyroRaw[2]);
    #else
    imu.gyro[0]=LPF2pApply_4(imu.gyroRaw[0]-imu.gyrOffset[0]);
    imu.gyro[1]=LPF2pApply_5(imu.gyroRaw[1]-imu.gyrOffset[1]);
    imu.gyro[2]=LPF2pApply_6(imu.gyroRaw[2]-imu.gyrOffset[2]);
    #endif    
}

// 检测IMU是否ready，校准好
// 需要将四轴放水平
#define ACCZ_ERR_MAX    0.05    // m/s^2
#define CHECK_TIME      5

uint8 IMUCheck(void)
{
    uint32_t accZSum = 0;
    uint8 i;
    float accZb = 0;

    for(i=0; i<CHECK_TIME; i++)
    {
        MPU6050_Read_Accelerometer(imu.accADC);
        accZSum += imu.accADC[2];
    }
    
    imu.accRaw[2]= (float)(accZSum /(float)CHECK_TIME) *ACC_SCALE * CONSTANTS_ONE_G ;
    accZb=imu.accRaw[2]-imu.accOffset[2];

    if((accZb > CONSTANTS_ONE_G-ACCZ_ERR_MAX ) && (accZb < CONSTANTS_ONE_G + ACCZ_ERR_MAX))
    {
        imu.caliPass = 1;
    }
    else
    {
        imu.caliPass = 0;
    }
        
    return imu.caliPass;
}

