#include "nrf24l01.h"
#include "spi.h"
#include "ucs.h"
#include "gpio.h"

// 修改该接收和发送地址，可以供多个飞行器在同一区域飞行，数据不受干扰
extern uint8 RX_ADDRESS[RX_ADR_WIDTH]= {0x34, 0xc3, 0x10, 0x10, 0x00};	//接收地址				

#define CE_H            GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN5)        
#define CE_L            GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN5)

void NRF24L01_Init()
{
  volatile int a = 0;
  GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN5);
  
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN0);
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN1);
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN2);
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN3);
  
  USCI_B_SPI_initMasterParam param;
  
  param.selectClockSource = USCI_B_SPI_CLOCKSOURCE_SMCLK;
  param.clockSourceFrequency = UCS_getSMCLK();
  param.desiredSpiClock = 100000;
  param.msbFirst = USCI_B_SPI_MSB_FIRST;
  param.clockPhase = USCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
  param.clockPolarity = USCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
  
  if(USCI_B_SPI_initMaster(USCI_B1_BASE, &param))
  {
    
    if(!NRF24L01_Check())
    {
      
     CE_L;	  
    //写RX节点地址
    NRF24L01_Write_Buf(SPI_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);
    //使能通道0的自动应答    
    NRF24L01_Write_Reg(SPI_WRITE_REG+EN_AA,0x01);    
    //使能通道0的接收地址  	 
    NRF24L01_Write_Reg(SPI_WRITE_REG+EN_RXADDR,0x01);
    //设置RF通信频率		  
    NRF24L01_Write_Reg(SPI_WRITE_REG+RF_CH,40);	     
    //选择通道0的有效数据宽度 	    
    NRF24L01_Write_Reg(SPI_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);
    //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
    NRF24L01_Write_Reg(SPI_WRITE_REG+RF_SETUP,0x0f);
    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX接收模式 
    NRF24L01_Write_Reg(SPI_WRITE_REG+NCONFIG, 0x0f); 
    //CE为高,进入接收模式 
    CE_H;        
    }
    
    
  }
}

void NRF24L01_Write_Reg(uint8_t regaddr,uint8_t data)
{
    CE_L;       //使能SPI传输
    USCI_B_SPI_transmitData(USCI_B1_BASE, regaddr);
    USCI_B_SPI_transmitData(USCI_B1_BASE, data);
     CE_H;  
}

void NRF24L01_Read_Buf(uint8_t regaddr,uint8_t *pBuf,uint8_t datalen)
{
      uint8_t uint8_t_ctr;	       

      USCI_B_SPI_transmitData(USCI_B1_BASE, regaddr);
      
      for(uint8_t_ctr = 0; uint8_t_ctr < datalen; uint8_t_ctr++)
      {
          pBuf[uint8_t_ctr] = USCI_B_SPI_receiveData(USCI_B1_BASE);
      }
}

void NRF24L01_Write_Buf(uint8_t regaddr, uint8_t *pbuf, uint8_t datalen)
{
	uint8_t uint8_t_ctr;	    
        CE_L;
  	USCI_B_SPI_transmitData(USCI_B1_BASE, regaddr);
        
  	for(uint8_t_ctr=0; uint8_t_ctr<datalen; uint8_t_ctr++)
          USCI_B_SPI_transmitData(USCI_B1_BASE, *pbuf++);
        CE_H;                                  //关闭spi传输
//  	return status;                                       //返回读到的状态值
}		


uint8_t NRF24L01_Check(void)
{
    uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
    uint8_t buf1[5];
    uint8_t i;   	 
    NRF24L01_Write_Buf(SPI_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
    NRF24L01_Read_Buf(TX_ADDR,buf1,5);              //读出写入的地址  	
    for(i=0;i<5;i++)if(buf1[i]!=0XA5)break;					   
    if(i!=5)return 1;                               //NRF24L01不在位	
    return 0;		                                //NRF24L01在位
}	