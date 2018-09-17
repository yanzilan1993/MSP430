#include "ALL_DEFINE.h" 
#include "control.h"
#include "pid.h"
#include "math.h"
#include "stdint.h"
//------------------------------------------------------------------------------
#undef NULL
#define NULL 0
#undef DISABLE 
#define DISABLE 0
#undef ENABLE 
#define ENABLE 1
#undef REST
#define REST 0
#undef SET 
#define SET 1 
#undef EMERGENT
#define EMERGENT 0
//------------------------------------------------------------------------------

PidObject *(pPidObject[])={&pidRateX,&pidRateY,&pidRateZ,&pidRoll,&pidPitch,&pidYaw   //�ṹ�����飬��ÿһ�������һ��pid�ṹ�壬�����Ϳ���������������PID��������  �������ʱ������λpid�������ݣ�����������仰�����þͿ�����
		,&pidHeightRate
		,&pidHeightHigh
};



void HeightPidControl(float dt)
{
    volatile static uint8_t status=WAITING_1;
    int16_t acc;       
    int16_t acc_error; 
    static int16_t acc_offset;
    static uint32_t high = 0; 
    static float thr_hold = 0; 
    static uint16_t cnt_take_off;	
    
    //��ȡ��ǰ�߶�����
    {	
        uint8_t data[3];

        if(IICreadBytes(0xc0,0x33,3,data) == FAILED)
        {
            pidHeightRate.out = Remote.thr -1000;//�߶ȴ������Ϊ�������
            return;//�����֧�ֶ��ߣ�������ѹ���д�����ֱ�ӷ��ء�
        }

        high = ((u32)data[0]<<16) | ((u16)data[1]<<8) | data[2];   //���θ߶�
    }

    //----------------------------------------------	
    { //��ȡ��ֱ�ٶ�����
        acc = (int16_t)GetNormAccz();//��ȡ��������
              
        if(!ALL_flag.unlock) //ȡ�þ�̬���ٶ�ֵ
        {
            acc_offset = acc;
        }
        acc_error = acc - acc_offset;	
                    
        {//�˲� 
            static float last_high;
            pidHeightRate.measured = (pidHeightRate.measured + acc_error * dt)*0.98f+0.02f*(high - last_high)/dt; //�ٶȻ���Զ�����������Ի����˲��ؼ���ץ���ٶȻ�����
            last_high =  pidHeightHigh.measured = high;  
        }	
    }
	//----------------------------------------------������ֹ����
	if(ALL_flag.unlock == EMERGENT) //�����������ʹ��ң�ؽ����������ɿؾͿ������κ�����½�����ֹ���У��������������˳�PID����
		status = EXIT_255;
	//----------------------------------------------����
	switch(status)
	{
		case WAITING_1: 
		  if(ALL_flag.height_lock && ALL_flag.unlock) 
			{
#ifdef  LED_EXCHANGE
				LED.status = WARNING;//LED����Ϊ����
#endif                                 
				status = WAITING_2;
                               
			}
			break;
		case WAITING_2: //����ǰ׼��
			if(ALL_flag.take_off)//ʹ��һ�����
			{
                            thr_hold = 500;
			}
			else//�������һ����ɣ��������趨Ϊ���붨��ʱ������ң��ֵ
                        {
                            thr_hold = Remote.thr -1000;  //��¼���붨��ʱ������
                        }
			cnt_take_off = 0;//���ʱ����0
			status = PROCESS_31;
			break;
		
		case PROCESS_31://���붨��	
			
			if(ALL_flag.take_off)
			{	
                            if(cnt_take_off<500)
                            {
                                Remote.thr  = 2500;//����ٶ� 
                                cnt_take_off++;
                            }
                            else//�����ɺ�ȴ�����ң�˻���
                            {
                                if(Remote.thr >1500)//���ǰ��ң��λ��δ֪
                                {
                                    ALL_flag.take_off = 0;
                                }
                                
                                Remote.thr = 1500;//ң��δ����
                            }
			}			

            {
                static uint8_t set_high = 0;

                if(Remote.thr<1850 && Remote.thr>1150) //��������ѻ��У������߶�
                {
                    if(set_high == 0) //������˳�����
                    {
                        set_high = 1;
                        pidHeightHigh.desired = pidHeightHigh.measured;//��¼���Ż���
                    }
                    pidUpdate(&pidHeightHigh,dt);    
                    pidHeightRate.desired = pidHeightHigh.out;  
                }
                else if(Remote.thr>1850) // �����߶�
                {
                    set_high = 0;
                    pidHeightRate.desired = 300; 
                }
                else if	(Remote.thr<1150) 
                {
                    set_high = 0;
                    pidHeightRate.desired = -300; 
                }					 
            }
								 
			pidUpdate(&pidHeightRate,dt); //
				 
            pidHeightRate.out += thr_hold;//������붨��ʱ������ֵ
				 
			if(!ALL_flag.height_lock)  //�˳�����
			{
#ifdef  LED_EXCHANGE
                          LED.status = AlwaysOn ;
#endif                          
				status = EXIT_255;
			}
			break;
		case EXIT_255: //�˳�����
			pidRest(&pPidObject[6],2);	//�����ǰ�Ķ������ֵ
			status = WAITING_1;//�ص��ȴ����붨��
			break;
		default:
			status = WAITING_1;
			break;	
	}	
}

/**************************************************************
 *  flight control
 * @param[in] 
 * @param[out] 
 * @return     
 ***************************************************************/
void FlightPidControl(float dt)
{
	volatile static uint8_t status=WAITING_1;

	switch(status)
	{		
            case WAITING_1: //�ȴ�����
                if(ALL_flag.unlock)
                {
                    status = READY_11;	
                }			
                break;
            case READY_11:  //׼���������
                pidRest(pPidObject,8); //������λPID���ݣ���ֹ�ϴ�����������Ӱ�챾�ο���
                Angle.yaw = pidYaw.desired =  pidYaw.measured = 0;   //����ƫ����
                status = PROCESS_31;
                break;			
            case PROCESS_31: //��ʽ�������
			
            pidRateX.measured = MPU6050.gyroX * Gyro_G; //�ڻ�����ֵ �Ƕ�/��
            pidRateY.measured = MPU6050.gyroY * Gyro_G;
            pidRateZ.measured = MPU6050.gyroZ * Gyro_G;

            pidPitch.measured = Angle.pitch; 
            pidRoll.measured = Angle.roll;
            pidYaw.measured = Angle.yaw;

            pidUpdate(&pidRoll,dt);    //�����PID		
            pidRateX.desired = pidRoll.out; //����ֵ��Ϊ����PID
            pidUpdate(&pidRateX,dt);  

            pidUpdate(&pidPitch,dt);    //����PID�������������⻷	������PID	
            pidRateY.desired = pidPitch.out;  
            pidUpdate(&pidRateY,dt); //�ٵ����ڻ�

            CascadePID(&pidRateZ,&pidYaw,dt);	//Ҳ����ֱ�ӵ��ô���PID����������
			break;
            case EXIT_255:  //�˳�����
                  pidRest(pPidObject,8);
                  status = WAITING_1;//���صȴ�����
		  break;
            default:
                  status = EXIT_255;
                  break;
	}
        
	if(ALL_flag.unlock == EMERGENT) //�����������ʹ��ң�ؽ����������ɿؾͿ������κ�����½�����ֹ���У��������������˳�PID����
		status = EXIT_255;
}

/**************************************************************
 *  ������
 * @param[in] 
 * @param[out] 
 * @return     
 ***************************************************************/
int16_t motor[4];
#define MOTOR1 motor[0] 
#define MOTOR2 motor[1] 
#define MOTOR3 motor[2] 
#define MOTOR4 motor[3] 

void MotorControl(void)
{	
	volatile static uint8_t status=WAITING_1;
	
	
	if(ALL_flag.unlock == EMERGENT) //�����������ʹ��ң�ؽ����������ɿؾͿ������κ�����½�����ֹ���У��������������˳�PID����
		status = EXIT_255;	
	switch(status)
	{		
		case WAITING_1: //�ȴ�����	
			MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0;  //������������������Ϊ0
			if(ALL_flag.unlock)
			{
                            ALL_flag.take_off = 0;	//�ս�������λһ�����
                            ALL_flag.height_lock = 0; //�ս�������λ����
                            status = WAITING_2;//������ң�ؿ��Խ��в���
			}
		case WAITING_2: //������ɺ��ж�ʹ�����Ƿ�ʼ����ң�˽��з��п���
			if(Remote.thr>1100&&!ALL_flag.take_off) 
			{
                            status = PROCESS_31;
			}
			else if(ALL_flag.take_off)
			{
                            ALL_flag.height_lock = 1;
                            status = PROCESS_31;							
			}
			else
			{
                            break;
			}	
		case PROCESS_31:
			{
				int16_t temp;
				if(ALL_flag.height_lock) 
				{		
					temp = pidHeightRate.out; //�����������Ƕ������ֵ
				}
				else //��������״̬����������ʹ��
				{
					if(Remote.thr<1040) //����̫����
					{
						MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0;//��0
						break;
					}
					temp = Remote.thr -1000; //�������������������ֵ
				}
				
				//������ֵ��Ϊ����ֵ��PWM
				MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = LIMIT(temp,0,800); 
				
		
				MOTOR1 +=    + pidRateX.out - pidRateY.out - pidRateZ.out;//; 1�ŵ�����䣬��̬����������������Ŀ�����,�뿴�ɿ�������ϵͼ�⣬���ĸ����PWM�ֲ��ֲ�
				MOTOR2 +=    + pidRateX.out + pidRateY.out + pidRateZ.out ;//;2�ŵ�����䣬
				MOTOR3 +=    - pidRateX.out + pidRateY.out - pidRateZ.out;//3�ŵ�����䣬
				MOTOR4 +=    - pidRateX.out - pidRateY.out + pidRateZ.out;//;4�ŵ�����䣬
			}	
			break;
		case EXIT_255:
			MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0;  //������������������Ϊ0
			status = WAITING_1;	//���صȴ�����
			break;
		default:
			break;
	}
        
        MotorPwmFlash(MOTOR1,MOTOR2,MOTOR3,MOTOR4);
} 
/************************************END OF FILE********************************************/ 
