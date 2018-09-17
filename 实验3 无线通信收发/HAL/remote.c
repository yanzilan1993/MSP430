
#include "ALL_DATA.h"
#include "nrf24l01.h"
#include "control.h"
#include <math.h>
#include "myMath.h"
#include "LED.h"
#include "Remote.h"


#undef SUCCESS
#define SUCCESS 0
#undef FAILED
#define FAILED  1

/*****************************************************************************************
 *  ͨ�����ݴ���
 * @param[in] 
 * @param[out] 
 * @return     
 ******************************************************************************************/	
	uint8_t RC_rxData[32];
void remote_unlock(void);	
void RC_Analy(void)  
{
		static uint16_t cnt_15s;
/*             Receive  and check RC data                               */	
	if(NRF24L01_RxPacket(RC_rxData)==SUCCESS)//���NRF24L01�Ľ��ܰ���Ϊ��
	{ 	

		uint8_t i;
		uint8_t CheckSum=0;
		cnt_15s = 0;
		for(i=0;i<31;i++)//У���
		{
			CheckSum +=  RC_rxData[i];
		}
	
		if(RC_rxData[31]==CheckSum && RC_rxData[0]==0xAA && RC_rxData[1]==0xAF)  //������յ���ң��������ȷ
		{   
#warning ����ط���Ҫ�ĳ�MSP430�ڵĺ궨��
                //STU2_LED_Toggle();
                
                Remote.roll = ((uint16_t)RC_rxData[4]<<8) | RC_rxData[5];  //ͨ��1��

                Remote.pitch = ((uint16_t)RC_rxData[6]<<8) | RC_rxData[7];  //ͨ��2��

                Remote.thr = 	((uint16_t)RC_rxData[8]<<8) | RC_rxData[9];   //ͨ��3��

                Remote.yaw =  ((uint16_t)RC_rxData[10]<<8) | RC_rxData[11];   //ͨ��4

                Remote.AUX1 =  ((uint16_t)RC_rxData[12]<<8) | RC_rxData[13];   //ͨ��5  

                Remote.AUX2 =  ((uint16_t)RC_rxData[14]<<8) | RC_rxData[15];   //ͨ��6   

				//---------------------------------------------------------------------------------
				//��ʼ����ң������
				{
							const float roll_pitch_ratio = 0.04f;  //ң�ؿ�����̬����
					
							pidPitch.desired =-(Remote.pitch-1500)*roll_pitch_ratio;	 
							pidRoll.desired = -(Remote.roll-1500)*roll_pitch_ratio;
					    if(Remote.yaw>1820 )
							{							
								pidYaw.desired += 0.45f;	
							}
							else if(Remote.yaw <1180)
							{
								pidYaw.desired -= 0.45f;	
							}

						
							{
								static uint16_t last_AUX1 = 1000;
								static uint16_t last_AUX2 = 1000;
				
								//���߰���
								if(last_AUX1!= Remote.AUX1)
								{
									if(!ALL_flag.take_off)
										ALL_flag.height_lock ^= 1;
								}	
						
								if(last_AUX2!= Remote.AUX2)//Ԥ��һ�����
								{
										ALL_flag.take_off ^= 1;
								}

								last_AUX1 = Remote.AUX1;
								last_AUX2 = Remote.AUX2;
							}

				}		
		}
  }
	else //���û���յ�ң���źţ����ж�Ϊʧ��
	{
		cnt_15s++;
		if(cnt_15s>5000)
		{
			cnt_15s = 0;
			ALL_flag.unlock = 0;  //ʧ������
#ifdef NRF24L01_ENABLE                        
			NRF24L01_init();  //��ʼNRF
#endif
		}
	}
	remote_unlock();
}

/*****************************************************************************************
 *  �����ж�
 * @param[in] 
 * @param[out] 
 * @return     
 ******************************************************************************************/	
void remote_unlock(void)
{
	volatile static uint8_t status=WAITING_1;
	static uint16_t cnt=0;

	if(Remote.thr<1150 &&Remote.yaw<1200)                         //����ң�����½�����
	{
		status = EXIT_255;
	}

#ifdef LED_EXCHANGE_	
	switch(status)
	{
		case WAITING_1://�ȴ�����
 //���������࣬�������->�������->������� ����LED�Ʋ����� ����ɽ���			
			if(Remote.thr<1100)  //��һ��        
			{			 
                               status = WAITING_2;				 
			}		
			break;
		case WAITING_2://�ڶ��� 
			if(Remote.thr>1800)          
			{		
                              static uint8_t cnt = 0;
                              cnt++;		
                              if(cnt>5) //��������豣��200ms���ϣ���ֹң�ؿ�����ʼ��δ��ɵĴ�������
                              {	
                                              cnt=0;
                                              status = WAITING_3;
                              }
			}			
			break;
		case WAITING_3:
			if(Remote.thr<1100)  //������        
			{			 
					 status = WAITING_4;				 
			}			
			break;			
		case WAITING_4:	//����ǰ׼��	               
				ALL_flag.unlock = 1;   //������־λ
				status = PROCESS_31;   //�������
				LED.status = AlwaysOn; //LED����									
				 break;		
		case PROCESS_31:	//�������״̬
				if(Remote.thr<1020&&!ALL_flag.height_lock)    //��ң��������� ������״̬
				{
                                    if(cnt++ > 2000)                                     // ����ң�˴������6S�Զ�����
                                    {								
                                            status = EXIT_255;								
                                    }
				}
				else if(!ALL_flag.unlock)                           //�������������ֱ�������ɿ�
				{
					status = EXIT_255;				
				}
				else					
					cnt = 0;
			break;
		case EXIT_255: //��������
			LED.status = AllFlashLight;	//LED������״̬������˸                                     //exit
			cnt = 0;
			LED.FlashTime = 100;  		 //LED������״̬������˸ʱ����Ϊ300ms                                 
			ALL_flag.unlock = 0;      //����
			status = WAITING_1;     //���صȴ�����
			break;
		default:
			status = EXIT_255;
			break;
	}
#endif
}
/***********************END OF FILE*************************************/







