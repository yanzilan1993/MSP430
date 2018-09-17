#ifndef SPL06_C
#define SPL06_C
#include "spl06.h"
#include "iic.h"
#include <math.h>

#define PRESSURE_REG    0X00
#define TEMP_REG        0X03
#define PRS_CFG         0x06
#define TMP_CFG         0x07
#define MEAS_CFG        0x08

#define SPL06_REST_VALUE 0x09
#define SPL06_REST_REG 0x0C

#define PRODUCT_ID 0X0D

#define uint32_t unsigned int

static struct SPL06_t SPL06;
static struct SPL06_t *p_SPL06;

void SPL06_write(uint8 hwadr, uint8 regadr, uint8 val);
uint8 SPL06_read(uint8 hwadr, uint8 regadr);
void SPL06_get_calib_param(void);


/*****************************************************************************
 �� �� ��  : SPL06_write
 ��������  : I2C �Ĵ���д���Ӻ���
 �������  : uint8 hwadr   Ӳ����ַ
             uint8 regadr  �Ĵ�����ַ
             uint8 val     ֵ
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��30��
    ��    ��   : WL
    �޸�����   : �����ɺ���
*****************************************************************************/
void SPL06_write(unsigned char hwadr, unsigned char regadr, unsigned char val)
{
//	IIC_Write_1Byte(hwadr,regadr,val);
        IICwriteByte(hwadr, regadr, val);

        
uint8 IIC_Read_One_Byte(uint8 dev, uint8 reg);
}

/*****************************************************************************
 �� �� ��  : SPL06_read
 ��������  : I2C �Ĵ�����ȡ�Ӻ���
 �������  : uint8 hwadr   Ӳ����ַ
             uint8 regadr  �Ĵ�����ַ
 �������  : 
 �� �� ֵ  : uint8 ����ֵ
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��30��
    ��    ��   : WL
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8 SPL06_read(unsigned char hwadr, unsigned char regadr)
{
	uint8 reg_data;
        reg_data = IIC_Read_One_Byte(hwadr, regadr);
	return reg_data;
}

/*****************************************************************************
 �� �� ��  : SPL06_init
 ��������  : SPL06-01 ��ʼ������
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��30��
    ��    ��   : WL
    �޸�����   : �����ɺ���

*****************************************************************************/
void SPL06_init(void)
{
    IIC_Init(USCI_B_I2C_CLOCKSOURCE_ACLK, 400000);
  
    p_SPL06 = &SPL06; /* read Chip Id */
    p_SPL06->i32rawPressure = 0;
    p_SPL06->i32rawTemperature = 0;
    p_SPL06->chip_id = 0x34;
    SPL06_get_calib_param();

    SPL06_rateset(PRESSURE_SENSOR,128, 32);   
    SPL06_rateset(TEMPERATURE_SENSOR,32, 8);
    SPL06_start_continuous(CONTINUOUS_P_AND_T);
    
}


/*****************************************************************************
 �� �� ��  : SPL06_rateset
 ��������  :  �����¶ȴ�������ÿ����������Լ���������
 �������  : uint8 u8OverSmpl  ��������         Maximal = 128
             uint8 u8SmplRate  ÿ���������(Hz) Maximal = 128
             uint8 iSensor     0: Pressure; 1: Temperature
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��11��24��
    ��    ��   : WL
    �޸�����   : �����ɺ���

*****************************************************************************/
void SPL06_rateset(uint8 iSensor, uint8 u8SmplRate, uint8 u8OverSmpl)
{
    uint8 reg = 0;
    int32_t i32kPkT = 0;
    switch(u8SmplRate)
    {
        case 2:
            reg |= (1<<5);
            break;
        case 4:
            reg |= (2<<5);
            break;
        case 8:
            reg |= (3<<5);
            break;
        case 16:
            reg |= (4<<5);
            break;
        case 32:
            reg |= (5<<5);
            break;
        case 64:
            reg |= (6<<5);
            break;
        case 128:
            reg |= (7<<5);
            break;
        case 1:
        default:
            break;
    }
    switch(u8OverSmpl)
    {
        case 2:
            reg |= 1;
            i32kPkT = 1572864;
            break;
        case 4:
            reg |= 2;
            i32kPkT = 3670016;
            break;
        case 8:
            reg |= 3;
            i32kPkT = 7864320;
            break;
        case 16:
            i32kPkT = 253952;
            reg |= 4;
            break;
        case 32:
            i32kPkT = 516096;
            reg |= 5;
            break;
        case 64:
            i32kPkT = 1040384;
            reg |= 6;
            break;
        case 128:
            i32kPkT = 2088960;
            reg |= 7;
            break;
        case 1:
        default:
            i32kPkT = 524288;
            break;
    }

    if(iSensor == 0)
    {
        p_SPL06->i32kP = i32kPkT;
        SPL06_write(HW_ADR, 0x06, reg);
        if(u8OverSmpl > 8)
        {
            reg = SPL06_read(HW_ADR, 0x09);
            SPL06_write(HW_ADR, 0x09, reg | 0x04);
        }
    }
    if(iSensor == 1)
    {
        p_SPL06->i32kT = i32kPkT;
        SPL06_write(HW_ADR, 0x07, reg|0x80);  //Using mems temperature
        if(u8OverSmpl > 8)
        {
            reg = SPL06_read(HW_ADR, 0x09);
            SPL06_write(HW_ADR, 0x09, reg | 0x08);
        }
    }
}

/*****************************************************************************
 �� �� ��  : SPL06_get_calib_param
 ��������  : ��ȡУ׼����
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��30��
    ��    ��   : WL
    �޸�����   : �����ɺ���

*****************************************************************************/
void SPL06_get_calib_param(void)
{
    uint32_t h;
    uint32_t m;
    uint32_t l;
    h =  SPL06_read(HW_ADR, 0x10);
    l  =  SPL06_read(HW_ADR, 0x11);
    p_SPL06->calib_param.c0 = (int16)h<<4 | l>>4;
    p_SPL06->calib_param.c0 = (p_SPL06->calib_param.c0&0x0800)?(0xF000|p_SPL06->calib_param.c0):p_SPL06->calib_param.c0;
    h =  SPL06_read(HW_ADR, 0x11);
    l  =  SPL06_read(HW_ADR, 0x12);
    p_SPL06->calib_param.c1 = (int16)(h&0x0F)<<8 | l;
    p_SPL06->calib_param.c1 = (p_SPL06->calib_param.c1&0x0800)?(0xF000|p_SPL06->calib_param.c1):p_SPL06->calib_param.c1;
    h =  SPL06_read(HW_ADR, 0x13);
    m =  SPL06_read(HW_ADR, 0x14);
    l =  SPL06_read(HW_ADR, 0x15);
    p_SPL06->calib_param.c00 = (int32_t)h<<12 | (int32_t)m<<4 | (int32_t)l>>4;
    p_SPL06->calib_param.c00 = (p_SPL06->calib_param.c00&0x080000)?(0xFFF00000|p_SPL06->calib_param.c00):p_SPL06->calib_param.c00;
    h =  SPL06_read(HW_ADR, 0x15);
    m =  SPL06_read(HW_ADR, 0x16);
    l =  SPL06_read(HW_ADR, 0x17);
    p_SPL06->calib_param.c10 = (int32_t)h<<16 | (int32_t)m<<8 | l;
    p_SPL06->calib_param.c10 = (p_SPL06->calib_param.c10&0x080000)?(0xFFF00000|p_SPL06->calib_param.c10):p_SPL06->calib_param.c10;
    h =  SPL06_read(HW_ADR, 0x18);
    l  =  SPL06_read(HW_ADR, 0x19);
    p_SPL06->calib_param.c01 = (int16)h<<8 | l;
    h =  SPL06_read(HW_ADR, 0x1A);
    l  =  SPL06_read(HW_ADR, 0x1B);
    p_SPL06->calib_param.c11 = (int16)h<<8 | l;
    h =  SPL06_read(HW_ADR, 0x1C);
    l  =  SPL06_read(HW_ADR, 0x1D);
    p_SPL06->calib_param.c20 = (int16)h<<8 | l;
    h =  SPL06_read(HW_ADR, 0x1E);
    l  =  SPL06_read(HW_ADR, 0x1F);
    p_SPL06->calib_param.c21 = (int16)h<<8 | l;
    h =  SPL06_read(HW_ADR, 0x20);
    l  =  SPL06_read(HW_ADR, 0x21);
    p_SPL06->calib_param.c30 = (int16)h<<8 | l;
}


/*****************************************************************************
 �� �� ��  : SPL06_start_temperature
 ��������  : ����һ���¶Ȳ���
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��30��
    ��    ��   : WL
    �޸�����   : �����ɺ���

*****************************************************************************/
void SPL06_start_temperature(void)
{
    SPL06_write(HW_ADR, 0x08, 0x02);
}

/*****************************************************************************
 �� �� ��  : SPL06_start_pressure
 ��������  : ����һ��ѹ��ֵ����
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��30��
    ��    ��   : WL
    �޸�����   : �����ɺ���

*****************************************************************************/
void SPL06_start_pressure(void)
{
    SPL06_write(HW_ADR, 0x08, 0x01);
}

/*****************************************************************************
 �� �� ��  : SPL06_start_continuous
 ��������  : Select node for the continuously measurement
 �������  : uint8 mode  1: pressure; 2: temperature; 3: pressure and temperature
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��25��
    ��    ��   : WL
    �޸�����   : �����ɺ���

*****************************************************************************/
void SPL06_start_continuous(uint8 mode)
{
    SPL06_write(HW_ADR, 0x08, mode+4);
}


/*****************************************************************************
 �� �� ��  : SPL06_get_raw_temp
 ��������  : ��ȡ�¶ȵ�ԭʼֵ����ת����32Bits����
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��30��
    ��    ��   : WL
    �޸�����   : �����ɺ���

*****************************************************************************/
void SPL06_get_raw_temp(void)
{
    uint8 h[3] = {0};
    
		h[0] = SPL06_read(HW_ADR, 0x03);
		h[1] = SPL06_read(HW_ADR, 0x04);
		h[2] = SPL06_read(HW_ADR, 0x05);

    p_SPL06->i32rawTemperature = (int32_t)h[0]<<16 | (int32_t)h[1]<<8 | (int32_t)h[2];
    p_SPL06->i32rawTemperature= (p_SPL06->i32rawTemperature&0x800000) ? (0xFF000000|p_SPL06->i32rawTemperature) : p_SPL06->i32rawTemperature;
}

/*****************************************************************************
 �� �� ��  : SPL06_get_raw_pressure
 ��������  : ��ȡѹ��ԭʼֵ����ת����32bits����
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��30��
    ��    ��   : WL
    �޸�����   : �����ɺ���

*****************************************************************************/
void SPL06_get_raw_pressure(void)
{
    uint8 h[3];
    
		h[0] = SPL06_read(HW_ADR, 0x00);
		h[1] = SPL06_read(HW_ADR, 0x01);
		h[2] = SPL06_read(HW_ADR, 0x02);
    
    p_SPL06->i32rawPressure = (int32_t)h[0]<<16 | (int32_t)h[1]<<8 | (int32_t)h[2];
    p_SPL06->i32rawPressure= (p_SPL06->i32rawPressure&0x800000) ? (0xFF000000|p_SPL06->i32rawPressure) : p_SPL06->i32rawPressure;
}


/*****************************************************************************
 �� �� ��  : SPL06_get_temperature
 ��������  : �ڻ�ȡԭʼֵ�Ļ����ϣ����ظ���У׼����¶�ֵ
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��30��
    ��    ��   : WL
    �޸�����   : �����ɺ���

*****************************************************************************/

float SPL06_get_temperature(void)
{
    float fTCompensate;
    float fTsc;

    fTsc = p_SPL06->i32rawTemperature / (float)p_SPL06->i32kT;
    fTCompensate =  p_SPL06->calib_param.c0 * 0.5 + p_SPL06->calib_param.c1 * fTsc;
    return fTCompensate;
}

/*****************************************************************************
 �� �� ��  : SPL06_get_pressure
 ��������  : �ڻ�ȡԭʼֵ�Ļ����ϣ����ظ���У׼���ѹ��ֵ
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��30��
    ��    ��   : WL
    �޸�����   : �����ɺ���

*****************************************************************************/

float SPL06_get_pressure(void)
{
    float fTsc, fPsc;
    float qua2, qua3;
    float fPCompensate;

    fTsc = p_SPL06->i32rawTemperature / (float)p_SPL06->i32kT;
    fPsc = p_SPL06->i32rawPressure / (float)p_SPL06->i32kP;
    qua2 = p_SPL06->calib_param.c10 + fPsc * (p_SPL06->calib_param.c20 + fPsc* p_SPL06->calib_param.c30);
    qua3 = fTsc * fPsc * (p_SPL06->calib_param.c11 + fPsc * p_SPL06->calib_param.c21);
		//qua3 = 0.9f *fTsc * fPsc * (p_SPL06->calib_param.c11 + fPsc * p_SPL06->calib_param.c21);
	
    fPCompensate = p_SPL06->calib_param.c00 + fPsc * qua2 + fTsc * p_SPL06->calib_param.c01 + qua3;
		//fPCompensate = p_SPL06->calib_param.c00 + fPsc * qua2 + 0.9f *fTsc  * p_SPL06->calib_param.c01 + qua3;
    return fPCompensate;
	
}

#define CONST_PF 0.1902630958	//(1/5.25588f) Pressure factor
#define FIX_TEMP 25				// Fixed Temperature. ASL is a function of pressure and temperature, but as the temperature changes so much (blow a little towards the flie and watch it drop 5 degrees) it corrupts the ASL estimates.
	
float SPL06PressureToAltitude(float pressure/*, float* groundPressure, float* groundTemp*/)
{
    if (pressure > 0)
    {
        return ((pow((1015.7f / pressure), CONST_PF) - 1.0f) * (FIX_TEMP + 273.15f)) / 0.0065f;
    }
    else
    {
        return 0;
    }
}

float temperature;
float pressure,alt_3,height;

void user_SPL06_get()
{
        SPL06_get_raw_temp();
        temperature = SPL06_get_temperature();

        SPL06_get_raw_pressure();
        pressure = SPL06_get_pressure();

        alt_3 = (101000 - pressure)/1000.0f;
        height = 0.0082f *alt_3 * alt_3 *alt_3 + 0.09f *(101000 - pressure)*100.0f ;

}	
#endif
