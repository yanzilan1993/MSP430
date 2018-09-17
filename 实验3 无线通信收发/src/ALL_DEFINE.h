#ifndef __ALL_DEFINE_H__
#define __ALL_DEFINE_H__
#include "ALL_DATA.h"
#include "INIT.h"
#include "iic.h"
#include "Spi.h"
#include "nrf24l01.h"
#include "USART.h"
#include "timer.h"
#include "led.h"
#include "mpu6050.h"
#include "imu.h"
#include "Remote.h"
#include "control.h"
#include "INIT.h"
#include "board.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "motor.h"
#include "mpu6050.h"

#include "data_type.h"

/*UWB引用*/
#ifdef UWB_ENABLE

#include "Trilateration_Core.h"
#include "UWB_Server.h"
#include "UWB_Data_Receive.h"
#include "Trilateration.h"
#include "Formation_Systen.h"
#include "rt_heap.h"
#endif



/*系统文件*/
#include <stdio.h>                                                                                                        
#include <string.h>
#include <stdlib.h>
#include "math.h"
#include "myMath.h"

#undef SUCCESS
#define SUCCESS 0
#undef FAILED
#define FAILED  1

//#define NRF24L01_ENABLE
//#define FBM320

#endif

