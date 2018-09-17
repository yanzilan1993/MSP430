#include "board.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "motor.h"
#include "mpu6050.h"
#include "kalman.h"

static struct _1_ekf_filter ekf[3] = {{0.02,0,0,0,0.001,0.543},
                                      {0.02,0,0,0,0.001,0.543},
                                      {0.02,0,0,0,0.001,0.543}};	

const float factor = 0.15f;  //ÂË²¨ÒòËØ
static float tBuff[3];
int main(void)
{
    int16_t ACC_Buff[3];
    int16_t Gyro_Buff[3];
    int16_t Filter_Out_ACC[3];
    int16_t Filter_Out_Gyro[3];
    
    System_Clock_Init();
    
    _EINT();

    Timer_B0_Init_ms_Interrupt();
    
    U1_UCA0_Init(USCI_A_UART_CLOCKSOURCE_ACLK, 57600);
    U2_UCA1_Init(USCI_A_UART_CLOCKSOURCE_ACLK, 115200);

    MPU6050_Init();

    delay_ms(500);
    
    for( ; ; )
    {
        MPU6050_Read_Accelerometer(ACC_Buff);
        MPU6050_Read_Gyroscope(Gyro_Buff);
        
        for(int i=0;i<3;i++)
        {
            kalman_1(&ekf[i],(float)ACC_Buff[i]);  //Ò»Î¬¿¨¶ûÂü
            Filter_Out_ACC[i] = (int16_t)ekf[i].out;
        }
        
        for(int i=0;i<3;i++)
        {
            Filter_Out_Gyro[i] = tBuff[i] = tBuff[i] * (1 - factor) + Gyro_Buff[i] * factor;  
        }

        delay_ms(500);
    }
}
