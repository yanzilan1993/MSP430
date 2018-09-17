#include "ALL_DEFINE.h"
#include "ALL_DATA.h"
#include "mpu6050.h"
#include "iic.h"
#include "filter.h"
#include <string.h>
#include "LED.h"
#include "myMath.h"
#include "kalman.h"
#include "gpio.h"

#define	SMPLRT_DIV		0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	CONFIGL			0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
#define	ACCEL_ADDRESS	0x3B
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define GYRO_ADDRESS  0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//��Դ��������ֵ��0x00(��������)
#define	WHO_AM_I		  0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)
#define MPU6050_PRODUCT_ID 0x68
#define MPU6052C_PRODUCT_ID 0x72

//#define   MPU6050_is_DRY()      GPIO_ReadOutBit(HT_GPIOC, GPIO_PIN_0)//IRQ������������
	#ifdef  	USE_I2C_HARDWARE
		
		#define MPU6050_ADDRESS 0xD0//0x68
	#else
		#define  MPU6050_ADDRESS 0xD0   //IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ
	#endif



int16_t MpuOffset[6] = {0};

static volatile int16_t *pMpu = (int16_t *)&MPU6050;

bool Mpu_Init_Done = false;

/****************************************************************************************
MPU6050��λ
*@brief  
*@brief   
*@param[in]
*****************************************************************************************/
int8_t mpu6050_rest(void)
{
	if(IICwriteByte(MPU6050_ADDRESS, PWR_MGMT_1, 0x80) == FAILED)
		return FAILED;	//��λ
	delay_ms(20);
	return SUCCESS;
}
/****************************************************************************************
MPU6050��ʼ��
*@brief   
*@brief  
*@param[in]
*****************************************************************************************/
int8_t MpuInit(void) //��ʼ��
{
    uint8_t date = SUCCESS;
  
    // Sensor MPU_Int Pin Initialize
     GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN2);
    
    // IIC Initialize
    IIC_Init(USCI_B_I2C_CLOCKSOURCE_ACLK, 400000);
    
    // Sensor Initialize
    do
    {
        date = IICwriteByte(MPU6050_ADDRESS, PWR_MGMT_1, 0x80);	  //��λ
        delay_ms(30);
        date += IICwriteByte(MPU6050_ADDRESS, SMPLRT_DIV, 0x02);  //�����ǲ����ʣ�0x00(333Hz)
        date += IICwriteByte(MPU6050_ADDRESS, PWR_MGMT_1, 0x03);  //�����豸ʱ��Դ��������Z��
        date += IICwriteByte(MPU6050_ADDRESS, CONFIGL, 0x03);     //��ͨ�˲�Ƶ�ʣ�0x03(42Hz)
        date += IICwriteByte(MPU6050_ADDRESS, GYRO_CONFIG, 0x18); //+-2000deg/s
        date += IICwriteByte(MPU6050_ADDRESS, ACCEL_CONFIG, 0x09);//+-4G
    }while(date != SUCCESS);  //�������������
    
    date = IIC_Read_One_Byte(MPU6050_ADDRESS, 0x75);  //�ж�MPU6050��ַ
    
    if(date!= MPU6050_PRODUCT_ID) //�����ַ��ȷ
    {
        return FAILED;
    }            
    else 
    {
        MpuGetOffset(); //����У׼����
        return SUCCESS;
    }   
}
/****************************************************************************************
��ȡMPU6050����
*@brief    
*@brief   
*@param[in]
*****************************************************************************************/

#define  Acc_Read() IICreadBytes(MPU6050_ADDRESS, 0X3B,6,buffer) //��ȡ���ٶ�
#define  Gyro_Read() IICreadBytes(MPU6050_ADDRESS, 0x43,6,&buffer[6])  //  ��ȡ���ٶ�

void MpuGetData(void) //��ȡ���������ݼ��˲�
{
      uint8_t i;
      uint8_t buffer[12];

      Acc_Read();
      Gyro_Read();

      for(i=0;i<6;i++)
      {
          pMpu[i] = (((int16_t)buffer[i<<1] << 8) | buffer[(i<<1)+1])-MpuOffset[i];	
          
          if(i < 3) //�˴��Լ��ٶ���һά�������˲�
          {
              {
                  static struct _1_ekf_filter ekf[3] = {{0.02,0,0,0,0.001,0.543},
                                                         {0.02,0,0,0,0.001,0.543},
                                                         {0.02,0,0,0,0.001,0.543}};	
                  kalman_1(&ekf[i],(float)pMpu[i]);  //һά������
                  pMpu[i] = (int16_t)ekf[i].out;
              }
          }
          if(i > 2)  //�˴��Խ��ٶ���һ���ͨ�˲�
          {	
              uint8_t k=i-3;
              const float factor = 0.15f;  //�˲�����			
              static float tBuff[3];		

              pMpu[i] = tBuff[k] = tBuff[k] * (1 - factor) + pMpu[i] * factor;                
          }
      }
}

/****************************************************************************************
*@brief   get mpu offset
*@brief   initial and cmd call this
*@param[in]
*****************************************************************************************/
void MpuGetOffset(void) //У׼
{
	int32_t buffer[6]={0};
	int16_t i;  
	uint8_t k=30;
	const int8_t MAX_GYRO_QUIET = 5;
	const int8_t MIN_GYRO_QUIET = -5;	
/*           wait for calm down    	                                                          */
	int16_t LastGyro[3] = {0};
	int16_t ErrorGyro[3];	
	/*           set offset initial to zero    		*/
	
	memset(MpuOffset,0,12);
	MpuOffset[2] = 8192;   //�����ֲ������趨���ٶȱ궨ֵ 
        
	while(k--)  //�жϷɿ��Ƿ��ھ�ֹ״̬
	{
		do
		{
			delay_ms(10);
			MpuGetData();
			for(i=0;i<3;i++)
			{
				ErrorGyro[i] = pMpu[i+3] - LastGyro[i];
				LastGyro[i] = pMpu[i+3];	
			}			
		}while ((ErrorGyro[0] >  MAX_GYRO_QUIET )|| (ErrorGyro[0] < MIN_GYRO_QUIET)
					||(ErrorGyro[1] > MAX_GYRO_QUIET )|| (ErrorGyro[1] < MIN_GYRO_QUIET)
					||(ErrorGyro[2] > MAX_GYRO_QUIET )|| (ErrorGyro[2] < MIN_GYRO_QUIET)
						);
	}	

/*           throw first 100  group data and make 256 group average as offset                    */	
	for(i=0;i<356;i++)  //ȡ��100����356���ƽ��ֵ��ΪУ׼ֵ
	{		
		MpuGetData();
		if(100 <= i)
		{
			uint8_t k;
			for(k=0;k<6;k++)
			{
				buffer[k] += pMpu[k];
			}
		}
	}

	for(i=0;i<6;i++)  //����У׼ֵ
	{
		MpuOffset[i] = buffer[i]>>8;
	}

      Mpu_Init_Done = true;
}
/**************************************END OF FILE*************************************/

