#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "data_type.h"

extern void MotorInit(void);
extern void MotorPwmFlash(int16_t MOTO1_PWM,int16_t MOTO2_PWM,int16_t MOTO3_PWM,int16_t MOTO4_PWM);

#endif  // __MOTOR_H__

