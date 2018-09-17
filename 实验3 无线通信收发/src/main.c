#include "ALL_DEFINE.h"

//��ɿص�ͨ��Э��
typedef struct
{
    int16_t Start;        //��ʼλ 0XAAAA
    int16_t ContrlBit;    //У��λ ox01
    int16_t THROTTLE;	  
    int16_t PITCH;
    int16_t ROLL;
    int16_t YAW;
    int16_t SW_TWO;       //�����������
    int16_t SW_THREE;	  //�Ҳ���������
    int16_t LEFT;		  //��ದ��
    int16_t RIGHT;		  //�Ҳದ��
    int16_t CaliFlag;	  //У׼����
    int16_t Arm_State;	  //��������
    int16_t Stop;         //ֹͣλ 0x5555
}send_Data_t;


uint8_t Rx_Buff[32];
volatile int a = 0;
int main(void)
{
    send_Data_t *Rx_Ptr;
  
    System_Clock_Init();//��ʼ��ʱ��
    
    _EINT();
    
    /*�ж�*/
    Timer_B0_Init_ms_Interrupt();
    
    LED_Init();
    NRF_Init();
    a = NRF_Check();
    RX_Mode();
    
    while(1)
    {      
      if(NRF_RxPacket(Rx_Buff))
      {
         LED_Off(&LED[LED_B]);
      }else
      {
         Rx_Ptr = (send_Data_t*)Rx_Buff;  
         
         if(Rx_Ptr->Start == 0xAAAA &&
            Rx_Ptr->Stop  == 0x5555)
         {
           if(Rx_Ptr->SW_TWO == 1000)
           {
              LED_On(&LED[LED_B]);
           }else if(Rx_Ptr->SW_TWO == 2000)
           {
             LED_Off(&LED[LED_B]);
           }
         }
      }
      
      delay_ms(500);
    }
}


