#include "motor.h"
#include "timer.h"

#define Moto_PwmMax 999

extern void MotorInit(void)
{
    Timer_A0_Init_PWM_Output();
    MotorPwmFlash(0, 0, 0, 0);
}

void MotorPwmFlash(int16_t MOTO1_PWM,int16_t MOTO2_PWM,int16_t MOTO3_PWM,int16_t MOTO4_PWM)
{
    float duty1 = 0.0;
    float duty2 = 0.0;
    float duty3 = 0.0;
    float duty4 = 0.0;
    
     if(MOTO1_PWM > Moto_PwmMax)
        MOTO1_PWM = Moto_PwmMax;
     
     if(MOTO2_PWM > Moto_PwmMax)
        MOTO2_PWM = Moto_PwmMax;
     
     if(MOTO3_PWM > Moto_PwmMax)
        MOTO3_PWM = Moto_PwmMax;
     
     if(MOTO4_PWM > Moto_PwmMax)
        MOTO4_PWM = Moto_PwmMax;
     
     if(MOTO1_PWM < 0)
        MOTO1_PWM = 0;
     
     if(MOTO2_PWM < 0)
        MOTO2_PWM = 0;
     
     if(MOTO3_PWM < 0)
        MOTO3_PWM = 0;
     
     if(MOTO4_PWM < 0)
        MOTO4_PWM = 0;
     
     duty1 = MOTO1_PWM / 1000.0f;
     duty2 = MOTO2_PWM / 1000.0f;
     duty3 = MOTO3_PWM / 1000.0f;
     duty4 = MOTO4_PWM / 1000.0f;
     
     PWM_Set_Ch_Duty(1, duty1);
     PWM_Set_Ch_Duty(2, duty2);
     PWM_Set_Ch_Duty(3, duty3);
     PWM_Set_Ch_Duty(4, duty4);
}

