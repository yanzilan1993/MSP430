#include "mpu6050.h"
#include "IIC.h"
#include "timer.h"
#include "gpio.h"

/**************************实现函数********************************************
*函数原型:		void MPU6050_initialize(void)
*功　　能:	    初始化 	MPU6050 以进入可用状态。
*******************************************************************************/
void MPU6050_Init(void)
{    
    // Sensor MPU_Int Pin Initialize
    GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN2);
    
    // IIC Initialize
    IIC_Init(USCI_B_I2C_CLOCKSOURCE_ACLK, 400000);
    
    // Sensor Initialize
    
    // 通过软件复位器件
    IICwriteByte(devAddr, MPU6050_RA_PWR_MGMT_1_0x6B, 0x80);      
    delay_ms(5);
    IICwriteByte(devAddr, MPU6050_RA_SMPLRT_DIV_0x19, 0x00);      
    IICwriteByte(devAddr, MPU6050_RA_PWR_MGMT_1_0x6B, 0x03);      
    IICwriteByte(devAddr, MPU6050_RA_INT_PIN_CFG_0x37, 0x40); 
    IICwriteByte(devAddr, MPU6050_RA_CONFIG_0x1A, MPU6050_DLPF_BW_42_0x03);  
    IICwriteByte(devAddr, MPU6050_RA_ACCEL_CONFIG_0x1C, 2 << 3);
}

void MPU6050_Read_Accelerometer(int16 *accData)
{
    static uint8 buf[8];

    IICreadBytes(devAddr, MPU6050_RA_ACCEL_XOUT_H_0x3B, 7, buf);
    
    accData[0] = (buf[1] << 8) | buf[0];
    accData[1] = (buf[3] << 8) | buf[2];
    accData[2] = (buf[5] << 8) | buf[4];
}

void MPU6050_Read_Gyroscope(int16 *gyroData)
{
    static uint8 buf[8];

    IICreadBytes(devAddr, MPU6050_RA_GYRO_XOUT_H, 7, buf);
    
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}
