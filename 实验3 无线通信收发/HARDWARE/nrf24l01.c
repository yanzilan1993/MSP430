#include "NRF24L01.h"

uint8_t TX_Address[5]={0x34,0x43,0x10,0x10,0x01};//���÷��Ͷ˵ĵ�ַ
uint8_t RX_Address[5]={0xE1,0xE2,0xE3,0xE4,0xE5};//���ý��ն˵ĵ�ַ

uint8_t tx_buf[TX_PLOAD_WIDTH]={0x00};
uint8_t rx_buf[RX_PLOAD_WIDTH]={0x00};

/****************************STM32����***************************************/
//��ʼ��24L01��IO��
void NRF_Init(void)
{
  P4SEL=0;
  P4DIR |= CSN + MOSI + SCK;  //SOMI , IRQ INPUT
  P4OUT&=~(CSN + MOSI + SCK);
  
  P2DIR |= CE;  //SOMI , IRQ INPUT
  P2OUT&=~(CE);
  
  CSN_H;
  //P4IES |= IRQ;         //�½����ñ�־λ
  //P4IE |= IRQ;
  P1DIR |= BIT0;        //LED
  P1OUT |= BIT0;        //LED
}

void RX_Mode(void)
{  
    CE_L;

    //дRX�ڵ��ַ
    SPI_Write_Buf(WRITE_REG_NRF+RX_ADDR_P0,RX_Address,RX_ADR_WIDTH);

    //ʹ��ͨ��0���Զ�Ӧ��    
    SPI_WR_Reg(WRITE_REG_NRF+EN_AA,0x01);

    //ʹ��ͨ��0�Ľ��յ�ַ  	 
    SPI_WR_Reg(WRITE_REG_NRF+EN_RXADDR,0x01);

    //����RFͨ��Ƶ��		  
    SPI_WR_Reg(WRITE_REG_NRF+RF_CH,40);	   

    //ѡ��ͨ��0����Ч���ݿ�� 	    
    SPI_WR_Reg(WRITE_REG_NRF+RX_PW_P0,RX_PLOAD_WIDTH);

    //����TX�������,0db����,2Mbps,���������濪��   
    SPI_WR_Reg(WRITE_REG_NRF+RF_SETUP,0x0f);

    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,PRIM_RX����ģʽ 
    SPI_WR_Reg(WRITE_REG_NRF+CONFIG, 0x0f); 

    CE_H; // Set CE pin high to enable RX device  

}

void TX_Mode(void)
{
  CE_L;
   //���÷��Ͷ˵ĵ�ַ   
  SPI_Write_Buf(WRITE_REG_NRF+TX_ADDR,TX_Address,TX_ADR_WIDTH);//дTX�ڵ��ַ 
  SPI_Write_Buf(WRITE_REG_NRF+RX_ADDR_P0, RX_Address,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
   //��ʼ��������     
  SPI_WR_Reg(WRITE_REG_NRF + EN_AA, 0x01); // Enable Auto.Ack:Pipe0//
  SPI_WR_Reg(WRITE_REG_NRF + EN_RXADDR, 0x01); // Enable Pipe0
  SPI_WR_Reg(WRITE_REG_NRF + SETUP_AW,0x03);
  SPI_WR_Reg(WRITE_REG_NRF + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...
  SPI_WR_Reg(WRITE_REG_NRF + RF_CH, 40); // Select RF channel 2.45G
  SPI_WR_Reg(WRITE_REG_NRF + RF_SETUP, 0x0f); // TX_PWR:0dBm, Datarate:2Mbps,LNA:HCURR
  SPI_WR_Reg(WRITE_REG_NRF + CONFIG, 0x0e); // Set PWR_UP bit, enable CRC(2 bytes)& Prim:TX. MAX_RT & TX_DS enabled..//
  //���TX_DS��־λ
  uint8_t sta=SPI_RD_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
  SPI_WR_Reg(WRITE_REG_NRF+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
  //CE_H;
}

//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
uint8_t NRF_Check(void)
{
    uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
    uint8_t buf2[5];
    
    uint8_t i;
    SPI_Write_Buf(WRITE_REG_NRF+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
    SPI_Read_Buf(TX_ADDR,buf2,5); //����д��ĵ�ַ  
    for(i=0;i<5;i++)
    {
      if(buf[i]!=0XA5)
        break;	 							           
    }
    
    if(i!=5)
      return 1;//���24L01����	
    return 0;		 //��⵽24L01
}
				   

uint8_t NRF_TxPacket(uint8_t *txbuf)
{
    uint8_t sta;
    CE_L;
    SPI_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
    CE_H;//��������	
    delay_us(10);   
    while((P2IN&IRQ)!=0);//�ȴ��������
    sta=SPI_RD_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
    SPI_WR_Reg(WRITE_REG_NRF+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־

    if(sta&MAX_TX!=0)//�ﵽ����ط�����
    {
      SPI_WR_Reg(FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
      return MAX_TX; 
    }
    if(sta&TX_OK)//�������
    {
      LED_J;
      
      return TX_OK;
    }
    return 0xff;//����ԭ����ʧ��
}


uint8_t NRF_RxPacket(uint8_t *rxbuf)
{
  uint8_t sta;		    							   
 // CE_H;       //CE �ߵ�ƽ>=130us������ģʽ  
  delay_us(130);

  sta=SPI_RD_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
  SPI_WR_Reg(WRITE_REG_NRF+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
  
  if(sta&RX_OK) 
  {
//    CE_L;
    
    LED_J;
     
    //rx_buf[0] = SPI_RD_Reg(RD_RX_PLOAD);  //��ȡ״̬�Ĵ�����ֵ   
    SPI_Read_Buf(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);//��ȡ����
    SPI_WR_Reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
    return 0;
  }
  
  else return 1;
}

/***************************spi����**************************/
uint8_t SPI_RW(uint8_t data)
{
  uint8_t bit;
  for(bit=0;bit<8;bit++)
  {                           //�ȸ�λ�����λ
    if(data & BIT7) MOSI_H; 
    else MOSI_L;
    delay_us(2);  //���ݽ���ʱ�� 1us
    SCK_H;
    delay_us(2);
    data <<= 1;     //һ��ָ���ʱ��
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

/*���ջ��������ʺ�������Ҫ�����ڽ���ʱ��ȡFIFO �������е�ֵ��
����˼·����ͨ��READ_REG ��������ݴӽ���FIFO��RD_RX_PLOAD���ж������浽��������ȥ��*/
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
/*���仺�������ʺ�������Ҫ����������������ŵ�����FIFO �������С�
����˼·����ͨ��WRITE_REG ��������ݴ浽����FIFO��WR_TX_PLOAD����ȥ��*/
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

/***************************�жϳ���**************************/
#pragma vector=PORT2_VECTOR
__interrupt void p2(void)
{
  LED_J;  //LED
  P2IFG =0;
}

