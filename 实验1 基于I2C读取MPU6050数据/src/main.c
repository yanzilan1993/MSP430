#include "board.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "motor.h"
#include "battery.h"
#include "mpu6050.h"
#include "imu.h"
#include "control.h"
#include "commapp.h"
#include "imuso3.h"
#include "configtable.h"
#include "dbg.h"

int main(void)
{
    int16_t ACC_Buff[3];
    int16_t Gyro_Buff[3];
    
    System_Clock_Init();
    
    _EINT();

    Timer_B0_Init_ms_Interrupt();
    
    U1_UCA0_Init(USCI_A_UART_CLOCKSOURCE_ACLK, 57600);
    U2_UCA1_Init(USCI_A_UART_CLOCKSOURCE_ACLK, 115200);

    MPU6050_Init();
    LED_Init();
    
    delay_ms(500);
    
    for( ; ; )
    {
        MPU6050_Read_Accelerometer(ACC_Buff);
        MPU6050_Read_Gyroscope(Gyro_Buff);
        
        for(int i=0;i<4;i++)
        {
            LED_Toogle(&LED[i+2]);
        }
        
        delay_ms(100);
    }
}
