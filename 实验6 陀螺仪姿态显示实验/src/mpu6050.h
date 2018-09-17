#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "data_type.h"
#include "i2c.h"

extern int16_t MpuOffset[6];

extern int8_t MpuInit(void);
extern void MpuGetData(void);
extern void MpuGetOffset(void);

#endif  // __MPU6050_H__

