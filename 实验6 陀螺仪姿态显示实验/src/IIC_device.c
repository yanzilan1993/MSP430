/*
 * IIC_device.c
 *
 *  Created on: 2015Äê6ÔÂ11ÈÕ
 *      Author: yu
 */
#include "IIC_device.h"
#include "Delay.h"
#include "usci_b_i2c.h"

unsigned char TXByteCtr=0,RXByteCtr=0;
unsigned char TxData[10]={0};
unsigned char RxBuf[10]={0};

void iic_init(uint8_t SLAVE_ADDRESS)
{
    //Assign I2C pins to USCI_B0    P3.0 IIC_SDA   P3.1 IIC_SCL
    P3SEL |= 0x03;
    UCB0CTL1 |= UCSWRST;                      // Enable SW reset
    UCB0CTL0 |= UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
    UCB0CTL1 |= UCSSEL_2 + UCSWRST;            // Use SMCLK
    UCB0BR0 = 60;                            // fSCL = SMCLK/240 = 100kHz
    UCB0BR1 = 0;
    UCB0I2CSA = SLAVE_ADDRESS;                // Slave Address
    UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
}

void iic_WriteMode(void)
{
    UCB0CTL1 |= UCTR;
    USCI_B_I2C_clearInterrupt(USCI_B0_BASE,USCI_B_I2C_TRANSMIT_INTERRUPT);
    UCB0IE &= ~UCRXIE;                        // Disable RX interrupt
    UCB0IE |= UCTXIE;                         // Enable TX interrupt
}
void iic_ReadMode(void)
{
    UCB0CTL1 &=~ UCTR;
    USCI_B_I2C_clearInterrupt(USCI_B0_BASE,USCI_B_I2C_RECEIVE_INTERRUPT);
    UCB0IE &= ~UCTXIE;                        // Disable TX interrupt
    UCB0IE |= UCRXIE;                         // Enable RX interrupt
}

void iic_writebyte(uint8_t reg_address,uint8_t write_number,uint8_t *data_to_write)
{
    uint8_t i=0;
    while(UCB0STAT & UCBBUSY);
    iic_WriteMode();
    TxData[write_number]=reg_address;
    for(i=write_number;i>0;i--)
    {
        TxData[i-1]= *data_to_write++;
    }
    TXByteCtr=write_number+1;            // Load TX byte counter
    UCB0CTL1 |= UCTXSTT;                 // I2C TX, start condition
    while (UCB0CTL1 & UCTXSTP);
}

void iic_readbyte(uint8_t reg_address,uint8_t read_number)
{
    //while(UCB0STAT & UCBBUSY);
    iic_WriteMode();
    TxData[0]=reg_address;
    TXByteCtr=1;                                    // Load TX byte counter
    UCB0CTL1 |= UCTXSTT;                            // I2C TX, start condition
    while (UCB0CTL1 & UCTXSTP);
    Delay_us(200);
    iic_ReadMode();
    RXByteCtr=read_number;
    UCB0CTL1 |= UCTXSTT;
    while (USCI_B_I2C_getInterruptStatus(USCI_B0_BASE,USCI_B_I2C_RECEIVE_INTERRUPT));
    while (UCB0CTL1 & UCTXSTP);
}

//
//
//
//
//uint8_t recive = 0;
//if (I2c_Start() == FAILED)
//return FAILED;
//I2c_SendByte(addr);
//if (I2c_WaitAck() == FAILED) {
//I2c_Stop();
//return FAILED;
//}
//I2c_SendByte(reg);
//I2c_WaitAck();
//I2c_Stop();
//I2c_Start();
//I2c_SendByte(addr+1);
//if (I2c_WaitAck() == FAILED) {
//I2c_Stop();
//return FAILED;
//}
//recive = I2c_ReadByte();
//I2c_NoAck();
//I2c_Stop();
//return recive;
//

