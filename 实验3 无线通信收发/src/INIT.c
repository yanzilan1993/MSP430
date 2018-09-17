#include "ALL_DEFINE.h"


volatile uint32_t SysTick_count; 
_st_Mpu MPU6050;   
_st_AngE Angle;   
_st_Remote Remote; 

_st_ALL_flag ALL_flag; 

PidObject pidRateX; 
PidObject pidRateY;
PidObject pidRateZ;

PidObject pidPitch; 
PidObject pidRoll;
PidObject pidYaw;

PidObject pidHeightRate;
PidObject pidHeightHigh;

void pid_param_Init(void); 

/**************************************************************
 *  整个系统外设和传感器初始化
 * @param[in] 
 * @param[out] 
 * @return     
 ***************************************************************/
void Hardware_Init(void)
{
        /*中断*/
        Timer_B0_Init_ms_Interrupt();
  

        /*此处不需要修改*/
        U1_UCA0_Init(USCI_A_UART_CLOCKSOURCE_ACLK, 57600);
        U2_UCA1_Init(USCI_A_UART_CLOCKSOURCE_ACLK, 115200);

        /*此处不需要修改*/
        MotorInit();

        /*此处不需要修改，但是没有修改6050的初始化*/
        MpuInit();

#ifdef FBM320	
	delay_ms(5000);         //延时5S等待气压计稳定
#endif
    
#ifdef NRF24L01_ENABLE
	NRF24L01_init();		//2.4G遥控通信初始化
#endif
}


/**************************************************************
 *  初始化PID参数
 * @brief 如果需要自己修改PID值，调这里就可以了
 * @param[out] 
 * @return     
 ***************************************************************/
void pid_param_Init(void)
{
        pidRateX.kp = 2.0f;
        pidRateY.kp = 2.0f;
        pidRateZ.kp = 7.0f;

        pidRateX.ki = 0.05f;
        pidRateY.ki = 0.05f;
        pidRateZ.ki = 0.0f;	

        pidRateX.kd = 0.15f;
        pidRateY.kd = 0.15f;
        pidRateZ.kd = 0.2f;	

        pidPitch.kp = 3.5f;
        pidRoll.kp = 3.5f;
        pidYaw.kp = 6.0f;	

        pidHeightRate.kp = 3.5f; //0.5f
        pidHeightRate.kd = 0.5f;
        pidHeightHigh.kp = 0.3f;//0.32f		
}















