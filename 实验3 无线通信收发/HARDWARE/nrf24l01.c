#include "NRF24L01.h"

uint8_t TX_Address[5]={0x34,0x43,0x10,0x10,0x01};//设置发送端的地址
uint8_t RX_Address[5]={0xE1,0xE2,0xE3,0xE4,0xE5};//设置接收端的地址

uint8_t tx_buf[TX_PLOAD_WIDTH]={0x00};
uint8_t rx_buf[RX_PLOAD_WIDTH]={0x00};

/****************************STM32程序***************************************/
//初始化24L01的IO口
void NRF_Init(void)
{
  P4SEL=0;
  P4DIR |= CSN + MOSI + SCK;  //SOMI , IRQ INPUT
  P4OUT&=~(CSN + MOSI + SCK);
  
  P2DIR |= CE;  //SOMI , IRQ INPUT
  P2OUT&=~(CE);
  
  CSN_H;
  //P4IES |= IRQ;         //下降沿置标志位
  //P4IE |= IRQ;
  P1DIR |= BIT0;        //LED
  P1OUT |= BIT0;        //LED
}

void RX_Mode(void)
{  
    CE_L;

    //写RX节点地址
    SPI_Write_Buf(WRITE_REG_NRF+RX_ADDR_P0,RX_Address,RX_ADR_WIDTH);

    //使能通道0的自动应答    
    SPI_WR_Reg(WRITE_REG_NRF+EN_AA,0x01);

    //使能通道0的接收地址  	 
    SPI_WR_Reg(WRITE_REG_NRF+EN_RXADDR,0x01);

    //设置RF通信频率		  
    SPI_WR_Reg(WRITE_REG_NRF+RF_CH,40);	   

    //选择通道0的有效数据宽度 	    
    SPI_WR_Reg(WRITE_REG_NRF+RX_PW_P0,RX_PLOAD_WIDTH);

    //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
    SPI_WR_Reg(WRITE_REG_NRF+RF_SETUP,0x0f);

    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX接收模式 
    SPI_WR_Reg(WRITE_REG_NRF+CONFIG, 0x0f); 

    CE_H; // Set CE pin high to enable RX device  

}

void TX_Mode(void)
{
  CE_L;
   //设置发送端的地址   
  SPI_Write_Buf(WRITE_REG_NRF+TX_ADDR,TX_Address,TX_ADR_WIDTH);//写TX节点地址 
  SPI_Write_Buf(WRITE_REG_NRF+RX_ADDR_P0, RX_Address,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  
   //初始化配置字     
  SPI_WR_Reg(WRITE_REG_NRF + EN_AA, 0x01); // Enable Auto.Ack:Pipe0//
  SPI_WR_Reg(WRITE_REG_NRF + EN_RXADDR, 0x01); // Enable Pipe0
  SPI_WR_Reg(WRITE_REG_NRF + SETUP_AW,0x03);
  SPI_WR_Reg(WRITE_REG_NRF + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...
  SPI_WR_Reg(WRITE_REG_NRF + RF_CH, 40); // Select RF channel 2.45G
  SPI_WR_Reg(WRITE_REG_NRF + RF_SETUP, 0x0f); // TX_PWR:0dBm, Datarate:2Mbps,LNA:HCURR
  SPI_WR_Reg(WRITE_REG_NRF + CONFIG, 0x0e); // Set PWR_UP bit, enable CRC(2 bytes)& Prim:TX. MAX_RT & TX_DS enabled..//
  //清除TX_DS标志位
  uint8_t sta=SPI_RD_Reg(STATUS);  //读取状态寄存器的值	   
  SPI_WR_Reg(WRITE_REG_NRF+STATUS,sta); //清除TX_DS或MAX_RT中断标志
  //CE_H;
}

//检测24L01是否存在
//返回值:0，成功;1，失败	
uint8_t NRF_Check(void)
{
    uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
    uint8_t buf2[5];
    
    uint8_t i;
    SPI_Write_Buf(WRITE_REG_NRF+TX_ADDR,buf,5);//写入5个字节的地址.	
    SPI_Read_Buf(TX_ADDR,buf2,5); //读出写入的地址  
    for(i=0;i<5;i++)
    {
      if(buf[i]!=0XA5)
        break;	 							           
    }
    
    if(i!=5)
      return 1;//检测24L01错误	
    return 0;		 //检测到24L01
}
				   

uint8_t NRF_TxPacket(uint8_t *txbuf)
{
    uint8_t sta;
    CE_L;
    SPI_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
    CE_H;//启动发送	
    delay_us(10);   
    while((P2IN&IRQ)!=0);//等待发送完成
    sta=SPI_RD_Reg(STATUS);  //读取状态寄存器的值	   
    SPI_WR_Reg(WRITE_REG_NRF+STATUS,sta); //清除TX_DS或MAX_RT中断标志

    if(sta&MAX_TX!=0)//达到最大重发次数
    {
      SPI_WR_Reg(FLUSH_TX,0xff);//清除TX FIFO寄存器 
      return MAX_TX; 
    }
    if(sta&TX_OK)//发送完成
    {
      LED_J;
      
      return TX_OK;
    }
    return 0xff;//其他原因发送失败
}


uint8_t NRF_RxPacket(uint8_t *rxbuf)
{
  uint8_t sta;		    							   
 // CE_H;       //CE 高电平>=130us，接收模式  
  delay_us(130);

  sta=SPI_RD_Reg(STATUS);  //读取状态寄存器的值    	 
  SPI_WR_Reg(WRITE_REG_NRF+STATUS,sta); //清除TX_DS或MAX_RT中断标志
  
  if(sta&RX_OK) 
  {
//    CE_L;
    
    LED_J;
     
    //rx_buf[0] = SPI_RD_Reg(RD_RX_PLOAD);  //读取状态寄存器的值   
    SPI_Read_Buf(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);//读取数据
    SPI_WR_Reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
    return 0;
  }
  
  else return 1;
}

/***************************spi程序**************************/
uint8_t SPI_RW(uint8_t data)
{
  uint8_t bit;
  for(bit=0;bit<8;bit++)
  {                           //先高位，后低位
    if(data & BIT7) MOSI_H; 
    else MOSI_L;
    delay_us(2);  //数据建立时间 1us
    SCK_H;
    delay_us(2);
    data <<= 1;     //一条指令的时间
    if(SPI_IN) data |= BIT0; // capture current MISO bit
    else data &=~BIT0;
    SCK_L;
    delay_us(1);
  } 
  delay_us(1);
  return(data); // return read data
}

uint8_t SPI_WR_Reg(uint8_t reg, uint8_t value)
{
    uint8_t status;
    CSN_L; // CSN low, init SPI transaction
    status = SPI_RW(reg); // select register
    SPI_RW(value); // ..and write value to it..
    CSN_H; // CSN high again
    return(status); // return nRF24L01 status byte
}

uint8_t SPI_RD_Reg(uint8_t reg)
{
  uint8_t reg_val;
  CSN_L; // CSN low, initialize SPI communication
  SPI_RW(reg); // Select register to read from..
  reg_val = SPI_RW(0); // ..then read registervalue
  CSN_H; // CSN high, terminate SPI communication
  return(reg_val); // return register value
}

/*接收缓冲区访问函数：主要用来在接收时读取FIFO 缓冲区中的值。
基本思路就是通过READ_REG 命令把数据从接收FIFO（RD_RX_PLOAD）中读出并存到数组里面去。*/
uint8_t SPI_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
  uint8_t status,byte_ctr;
  CSN_L; // Set CSN low, init SPI tranaction
  status = SPI_RW(reg); // Select register to write to and read status byte
  for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    pBuf[byte_ctr] = SPI_RW(0); // Perform SPI_RW to read byte from nRF24L01
  CSN_H; // Set CSN high again
  return(status); // return nRF24L01 status byte
}
/*发射缓冲区访问函数：主要用来把数组里的数放到发射FIFO 缓冲区中。
基本思路就是通过WRITE_REG 命令把数据存到发射FIFO（WR_TX_PLOAD）中去。*/
uint8_t SPI_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t bytes)
{
  uint8_t status,byte_ctr;
  CSN_L; // Set CSN low, init SPI tranaction
  status = SPI_RW(reg); // Select register to write to and read status byte
  delay_us(1);
  for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // then write all byte in buffer(*pBuf)
    SPI_RW(*pBuf++);
  CSN_H; // Set CSN high again
  return(status); // return nRF24L01 status byte
}

/***************************中断程序**************************/
#pragma vector=PORT2_VECTOR
__interrupt void p2(void)
{
  LED_J;  //LED
  P2IFG =0;
}

