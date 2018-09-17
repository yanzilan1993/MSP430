//#include "Spi.h"
//#include "driverlib.h"
//
//#define SPICLK                          500000
//uint8_t returnValue = 0x00;
//
//void Spi_Init()
//{
//    //Set P1.1 for slave reset
//    GPIO_setOutputHighOnPin(    
//        GPIO_PORT_P4,
//        GPIO_PIN1
//        );
//
//    //Set P1.1 for slave reset
//    //Set P1.0 to output direction
//    GPIO_setAsOutputPin(
//        GPIO_PORT_P4,
//        GPIO_PIN0+GPIO_PIN1+GPIO_PIN3
//        );
//
//    //P3.5,4,0 option select
//    GPIO_setAsPeripheralModuleFunctionInputPin(
//        GPIO_PORT_P4,
//        GPIO_PIN2
//        );
//
//    //Initialize Master
//    USCI_B_SPI_initMasterParam param = {0};
//    param.selectClockSource = USCI_B_SPI_CLOCKSOURCE_SMCLK;
//    param.clockSourceFrequency = UCS_getSMCLK();
//    param.desiredSpiClock = SPICLK;
//    param.msbFirst = USCI_B_SPI_MSB_FIRST;
//    param.clockPhase = USCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
//    param.clockPolarity = USCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
//    returnValue = USCI_B_SPI_initMaster(USCI_B1_BASE, &param);
//
//    if(STATUS_FAIL == returnValue)
//    {
//        return;
//    }
//    
//    //Enable SPI module
//    USCI_B_SPI_enable(USCI_B1_BASE);
//    
//    USCI_B_SPI_clearInterrupt(USCI_B1_BASE, USCI_B_SPI_RECEIVE_INTERRUPT);
//    USCI_B_SPI_enableInterrupt(USCI_B1_BASE, USCI_B_SPI_RECEIVE_INTERRUPT);
//}
//
//void USCI_B1_ISR()
//{
//  
//}
//
//uint8_t SPI_RW(uint8_t reg,uint8_t data)
//{      
//      USCI_B_SPI_transmitData(USCI_B1_BASE,reg);
//      USCI_B_SPI_transmitData(USCI_B1_BASE,data);
//      return USCI_B_SPI_receiveData(USCI_B1_BASE);
//}
//
//uint8_t SPI_Read(uint8_t reg)
//{
//      USCI_B_SPI_transmitData(USCI_B1_BASE,reg);
//      return USCI_B_SPI_receiveData(USCI_B1_BASE);
//}
//
//uint8_t SPI_Write_Buff(uint8_t reg,uint8_t *ptr,uint8_t length)
//{
//  for(int i = 0;i<length;i++)
//  {
//    SPI_RW(reg,*ptr++);
//  }
//}
//
//uint8_t SPI_Read_Buff(uint8_t reg,uint8_t *ptr,uint8_t length)
//{
//      uint8_t status,uint8_t_ctr;	       
//      status=SPI_RW(reg,0xFF);   //发送寄存器值(位置),并读取状态值   	   
//      for(uint8_t_ctr=0;uint8_t_ctr<length;uint8_t_ctr++)
//        ptr[uint8_t_ctr]=SPI_RW(reg,0xFF);//读出数据
//      return status;                        //返回读到的状态值
//}