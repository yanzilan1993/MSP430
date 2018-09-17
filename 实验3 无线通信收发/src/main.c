#include "ALL_DEFINE.h"

//与飞控的通信协议
typedef struct
{
    int16_t Start;        //起始位 0XAAAA
    int16_t ContrlBit;    //校验位 ox01
    int16_t THROTTLE;	  
    int16_t PITCH;
    int16_t ROLL;
    int16_t YAW;
    int16_t SW_TWO;       //左侧两档开关
    int16_t SW_THREE;	  //右侧三档开关
    int16_t LEFT;		  //左侧拨盘
    int16_t RIGHT;		  //右侧拨盘
    int16_t CaliFlag;	  //校准按键
    int16_t Arm_State;	  //启动按键
    int16_t Stop;         //停止位 0x5555
}send_Data_t;


uint8_t Rx_Buff[32];
volatile int a = 0;
int main(void)
{
    send_Data_t *Rx_Ptr;
  
    System_Clock_Init();//初始化时钟
    
    _EINT();
    
    /*中断*/
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


