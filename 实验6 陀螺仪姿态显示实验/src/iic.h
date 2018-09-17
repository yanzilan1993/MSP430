#ifndef __IIC_H__
#define __IIC_H__

#include "usci_b_i2c.h"
#include "data_type.h"

extern void IIC_Init(uint8 ClockSource, uint32 IIC_Clock_Hz);

extern uint8 IICreadBytes(uint8 dev, uint8 reg, uint8 length, uint8 *data);
extern uint8 IIC_Read_One_Byte2(uint8 dev, uint8 reg);
extern unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data);
extern uint8 IICwriteBytes(uint8 dev, uint8 reg, uint8 length, uint8* data);
extern uint8 IICwriteBit(uint8 dev, uint8 reg, uint8 bitNum, uint8 data);
extern uint8 IICwriteBits(uint8 dev, uint8 reg, uint8 bitStart, uint8 length, uint8 data);

extern void IIC_Test(void);

#endif  // __IIC_H__
