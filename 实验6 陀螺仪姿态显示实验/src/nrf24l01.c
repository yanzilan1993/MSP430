#include "nrf24l01.h"
#include "spi.h"

// 修改该接收和发送地址，可以供多个飞行器在同一区域飞行，数据不受干扰
extern uint8 RX_ADDRESS[RX_ADR_WIDTH]= {0x34, 0xc3, 0x10, 0x10, 0x00};	//接收地址				
