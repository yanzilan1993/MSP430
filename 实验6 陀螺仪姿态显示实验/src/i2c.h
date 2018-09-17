#ifndef _I2C_H
#define _I2C_H

#include "board.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "gpio.h"
#include "msp430.h"

#define TRUE  0
#define FALSE -1

//GPIO_setAsOutputPin(LED[i].Port, LED[i].Pin);
//GPIO_setOutputHighOnPin(LED_x->Port, LED_x->Pin);
//GPIO_setOutputLowOnPin(LED_x->Port, LED_x->Pin);

#define SCL_H         GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN1)          
#define SCL_L         GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN1)

#define SDA_H         GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN0)       
#define SDA_L         GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0)

//#define SDA_read      GPIOB->IDR  & GPIO_Pin_7 /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_11) */
#define SDA_read      (P3IN & GPIO_PIN0)

//Assign I2C pins to USCI_B0    P3.0 IIC_SDA   P3.1 IIC_SCL
//0表示写

#define	I2C_Direction_Trans   0
//１表示读
#define	I2C_Direction_Rec      1	 
/*====================================================================================================*/
/*====================================================================================================*/
//PB6 SCL
//PB7 SDA
//return 0:success   1:failed
extern void I2C_Init(void);
extern uint8_t I2c_Start(void);
extern void I2c_Stop(void);
extern void I2c_Ack(void);
extern void I2c_NoAck(void);
extern uint8_t I2c_WaitAck(void);
extern void I2c_SendByte(uint8_t byte);
extern uint8_t I2c_ReadByte(void);
//----------------------------------------------------------------------
extern int8_t IIC_Write_One_Byte(uint8_t addr,uint8_t reg,uint8_t data);
extern int8_t IIC_Read_One_Byte(uint8_t addr,uint8_t reg);	 
extern int8_t IIC_Write_Bytes(uint8_t addr,uint8_t reg,uint8_t *data,uint8_t len);
extern int8_t IIC_read_Bytes(uint8_t addr,uint8_t reg,uint8_t *data,uint8_t len);
extern uint8_t IIC_Read_1Byte(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t *REG_data);
extern uint8_t IIC_Write_1Byte(uint8_t SlaveAddress,uint8_t REG_Address,uint8_t REG_data);
//----------------------------------------------------------------------f













#endif