#include "mpu6050.h"
#include "IIC.h"
#include "timer.h"
#include "gpio.h"

static uint8 buffer[14];

/**************************实现函数********************************************
*函数原型:		unsigned char MPU6050_is_DRY(void)
*功　　能:	    检查 MPU6050的中断引脚，测试是否完成转换
返回 1  转换完成
0 数据寄存器还没有更新
*******************************************************************************/
unsigned char MPU6050_is_DRY(void)
{
    // remember_to_update
//    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == Bit_SET)
//        return 1;
//    else
        return 0;
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_setClockSource(uint8_tsource)
*功　　能:	    设置  MPU6050 的时钟源
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Stops the clock and keeps the timing generator in reset
*******************************************************************************/
void MPU6050_setClockSource(uint8 source)
{
    IICwriteBits(devAddr, MPU6050_RA_PWR_MGMT_1_0x6B, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

/** Trigger a full device reset.
 * A small delay of ~50ms may be desirable after triggering a reset.
 * @see MPU6050_RA_PWR_MGMT_1_0x6B
 * @see MPU6050_PWR1_DEVICE_RESET_BIT
 */
void MPU6050_reset(void)
{
    IICwriteBit(devAddr, MPU6050_RA_PWR_MGMT_1_0x6B, MPU6050_PWR1_DEVICE_RESET_BIT, 1);
}

/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU6050_GYRO_FS_250
 * @see MPU6050_RA_GYRO_CONFIG_0x1B
 * @see MPU6050_GCONFIG_FS_SEL_BIT
 * @see MPU6050_GCONFIG_FS_SEL_LENGTH
 */
void MPU6050_setFullScaleGyroRange(uint8 range)
{
    IICwriteBits(devAddr, MPU6050_RA_GYRO_CONFIG_0x1B, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_setFullScaleAccelRange(uint8_trange)
*功　　能:	    设置  MPU6050 加速度计的最大量程
*******************************************************************************/
void MPU6050_setFullScaleAccelRange(uint8 range)
{
    IICwriteBits(devAddr, MPU6050_RA_ACCEL_CONFIG_0x1C, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_setSleepEnabled(uint8_tenabled)
*功　　能:	    设置  MPU6050 是否进入睡眠模式
				enabled =1   睡觉
			    enabled =0   工作
*******************************************************************************/
void MPU6050_setSleepEnabled(uint8 enabled)
{
    IICwriteBit(devAddr, MPU6050_RA_PWR_MGMT_1_0x6B, MPU6050_PWR1_SLEEP_BIT, enabled);
}

/**************************实现函数********************************************
*函数原型:		uint8_tMPU6050_getDeviceID(void)
*功　　能:	    读取  MPU6050 WHO_AM_I 标识	 将返回 0x68
*******************************************************************************/
uint8 MPU6050_getDeviceID(void)
{
    IICreadBytes(devAddr, MPU6050_RA_WHO_AM_I, 1, buffer);
    
    return buffer[0];
}

/**************************实现函数********************************************
*函数原型:		uint8_tMPU6050_testConnection(void)
*功　　能:	    检测MPU6050 是否已经连接
*******************************************************************************/
uint8 MPU6050_testConnection(void)
{
   if(MPU6050_getDeviceID() == 0x68)
        return 1;
   else
        return 0;
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_setI2CMasterModeEnabled(uint8_tenabled)
*功　　能:	    设置 MPU6050 是否为AUX I2C线的主机
*******************************************************************************/
void MPU6050_setI2CMasterModeEnabled(uint8 enabled)
{
    IICwriteBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_setI2CBypassEnabled(uint8_tenabled)
*功　　能:	    设置 MPU6050 是否为AUX I2C线的主机
*******************************************************************************/
void MPU6050_setI2CBypassEnabled(uint8 enabled)
{
    IICwriteBit(devAddr, MPU6050_RA_INT_PIN_CFG_0x37, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_Check()
*功　　能:	  检测IIC总线上的MPU6050是否存在
*******************************************************************************/
void MPU6050_Check(void) 
{ 

} 
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
    IICwriteByte(devAddr, MPU6050_RA_PWR_MGMT_1_0x6B, 0x80);      //PWR_MGMT_1    -- DEVICE_RESET 1
    delay_ms(5);
    IICwriteByte(devAddr, MPU6050_RA_SMPLRT_DIV_0x19, 0x00);      
    IICwriteByte(devAddr, MPU6050_RA_PWR_MGMT_1_0x6B, 0x03);      
    IICwriteByte(devAddr, MPU6050_RA_INT_PIN_CFG_0x37, 0x40); 
    IICwriteByte(devAddr, MPU6050_RA_CONFIG_0x1A, MPU6050_DLPF_BW_42_0x03);  
    
    MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_2000_0x03);
    
    // Accel scale 8g (4096 LSB/g)    
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
