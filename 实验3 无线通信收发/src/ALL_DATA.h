#ifndef _ALL_USER_DATA_H_
#define _ALL_USER_DATA_H_

#include "stdint.h"

//#define NULL  0
extern volatile uint32_t SysTick_count;


typedef struct{
	int16_t accX;
	int16_t accY;
	int16_t accZ;
	int16_t gyroX;
	int16_t gyroY;
	int16_t gyroZ;
}_st_Mpu;


typedef struct{
	int16_t magX;
	int16_t magY;
	int16_t magZ;
}_st_Mag;


typedef struct{
	float rate;
	float height;
}High;


typedef struct{
	float roll;
	float pitch;
	float yaw;
}_st_AngE;



typedef struct
{
	uint16_t roll;
	uint16_t pitch;
	uint16_t thr;
	uint16_t yaw;
	uint16_t AUX1;
	uint16_t AUX2;
	uint16_t AUX3;
	uint16_t AUX4;	
	uint16_t AUX5;
	uint16_t AUX6;
	uint16_t AUX7;		
}_st_Remote;



typedef volatile struct
{
	float desired;          //< 目标值
	float offset;           //< 偏移值
	float prevError;        //< 上次误差
	float integ;            //< 积分量
	float kp;               //< Kp
	float ki;               //< Ki
	float kd;               //< Kd
	float IntegLimitHigh;   //< 积分限幅
	float IntegLimitLow;
	float measured;
	float out;              //< 输出量            
	float OutLimitHigh;     //< 输出限幅        
	float OutLimitLow;
}PidObject;


typedef volatile struct
{
	uint8_t unlock:1;
	uint8_t height_lock:1;
	uint8_t take_off:1;
	uint8_t take_down:1;
}_st_ALL_flag;


extern _st_Remote Remote;
extern _st_Mpu MPU6050;
extern _st_Mag AK8975; 
extern _st_AngE Angle;


extern _st_ALL_flag ALL_flag;


extern	PidObject pidRateX;
extern	PidObject pidRateY;
extern	PidObject pidRateZ;

extern	PidObject pidPitch;
extern	PidObject pidRoll;
extern	PidObject pidYaw;

extern	PidObject pidHeightRate;
extern	PidObject pidHeightHigh;

#endif

