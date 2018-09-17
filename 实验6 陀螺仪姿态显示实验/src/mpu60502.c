#include "mpu60502.h"
#include "IIC_device.h"








void Mpu_Init()
{
    iic_init(0xD0);
    
    
}

