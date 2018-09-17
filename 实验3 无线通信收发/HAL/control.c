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

PidObject *(pPidObject[])={&pidRateX,&pidRateY,&pidRateZ,&pidRoll,&pidPitch,&pidYaw   //结构体数组，将每一个数组放一个pid结构体，这样就可以批量操作各个PID的数据了  比如解锁时批量复位pid控制数据，新手明白这句话的作用就可以了
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
    
    //获取当前高度数据
    {	
        uint8_t data[3];

        if(IICreadBytes(0xc0,0x33,3,data) == FAILED)
        {
            pidHeightRate.out = Remote.thr -1000;//高度错误则改为油门输出
            return;//如果不支持定高，或者气压计有错误则直接返回。
        }

        high = ((u32)data[0]<<16) | ((u16)data[1]<<8) | data[2];   //海拔高度
    }

    //----------------------------------------------	
    { //获取垂直速度数据
        acc = (int16_t)GetNormAccz();//提取重力向量
              
        if(!ALL_flag.unlock) //取得静态加速度值
        {
            acc_offset = acc;
        }
        acc_error = acc - acc_offset;	
                    
        {//滤波 
            static float last_high;
            pidHeightRate.measured = (pidHeightRate.measured + acc_error * dt)*0.98f+0.02f*(high - last_high)/dt; //速度环永远是主调，所以互补滤波关键就抓在速度环这里
            last_high =  pidHeightHigh.measured = high;  
        }	
    }
	//----------------------------------------------紧急终止飞行
	if(ALL_flag.unlock == EMERGENT) //意外情况，请使用遥控紧急上锁，飞控就可以在任何情况下紧急中止飞行，锁定飞行器，退出PID控制
		status = EXIT_255;
	//----------------------------------------------控制
	switch(status)
	{
		case WAITING_1: 
		  if(ALL_flag.height_lock && ALL_flag.unlock) 
			{
#ifdef  LED_EXCHANGE
				LED.status = WARNING;//LED单闪为定高
#endif                                 
				status = WAITING_2;
                               
			}
			break;
		case WAITING_2: //定高前准备
			if(ALL_flag.take_off)//使能一键起飞
			{
                            thr_hold = 500;
			}
			else//如果不是一键起飞，则油门设定为进入定高时的油门遥杆值
                        {
                            thr_hold = Remote.thr -1000;  //记录进入定高时的油门
                        }
			cnt_take_off = 0;//起飞时间清0
			status = PROCESS_31;
			break;
		
		case PROCESS_31://进入定高	
			
			if(ALL_flag.take_off)
			{	
                            if(cnt_take_off<500)
                            {
                                Remote.thr  = 2500;//起飞速度 
                                cnt_take_off++;
                            }
                            else//起飞完成后等待油门遥杆回中
                            {
                                if(Remote.thr >1500)//起飞前的遥杆位置未知
                                {
                                    ALL_flag.take_off = 0;
                                }
                                
                                Remote.thr = 1500;//遥杆未回中
                            }
			}			

            {
                static uint8_t set_high = 0;

                if(Remote.thr<1850 && Remote.thr>1150) //如果油门已回中，不调高度
                {
                    if(set_high == 0) //如果刚退出调高
                    {
                        set_high = 1;
                        pidHeightHigh.desired = pidHeightHigh.measured;//记录油门回中
                    }
                    pidUpdate(&pidHeightHigh,dt);    
                    pidHeightRate.desired = pidHeightHigh.out;  
                }
                else if(Remote.thr>1850) // 调整高度
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
				 
            pidHeightRate.out += thr_hold;//加入进入定高时的油门值
				 
			if(!ALL_flag.height_lock)  //退出定高
			{
#ifdef  LED_EXCHANGE
                          LED.status = AlwaysOn ;
#endif                          
				status = EXIT_255;
			}
			break;
		case EXIT_255: //退出定高
			pidRest(&pPidObject[6],2);	//清除当前的定高输出值
			status = WAITING_1;//回到等待进入定高
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
            case WAITING_1: //等待解锁
                if(ALL_flag.unlock)
                {
                    status = READY_11;	
                }			
                break;
            case READY_11:  //准备进入控制
                pidRest(pPidObject,8); //批量复位PID数据，防止上次遗留的数据影响本次控制
                Angle.yaw = pidYaw.desired =  pidYaw.measured = 0;   //锁定偏航角
                status = PROCESS_31;
                break;			
            case PROCESS_31: //正式进入控制
			
            pidRateX.measured = MPU6050.gyroX * Gyro_G; //内环测量值 角度/秒
            pidRateY.measured = MPU6050.gyroY * Gyro_G;
            pidRateZ.measured = MPU6050.gyroZ * Gyro_G;

            pidPitch.measured = Angle.pitch; 
            pidRoll.measured = Angle.roll;
            pidYaw.measured = Angle.yaw;

            pidUpdate(&pidRoll,dt);    //横滚角PID		
            pidRateX.desired = pidRoll.out; //期望值即为串级PID
            pidUpdate(&pidRateX,dt);  

            pidUpdate(&pidPitch,dt);    //调用PID处理函数来处理外环	俯仰角PID	
            pidRateY.desired = pidPitch.out;  
            pidUpdate(&pidRateY,dt); //再调用内环

            CascadePID(&pidRateZ,&pidYaw,dt);	//也可以直接调用串级PID函数来处理
			break;
            case EXIT_255:  //退出控制
                  pidRest(pPidObject,8);
                  status = WAITING_1;//返回等待解锁
		  break;
            default:
                  status = EXIT_255;
                  break;
	}
        
	if(ALL_flag.unlock == EMERGENT) //意外情况，请使用遥控紧急上锁，飞控就可以在任何情况下紧急中止飞行，锁定飞行器，退出PID控制
		status = EXIT_255;
}

/**************************************************************
 *  电机输出
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
	
	
	if(ALL_flag.unlock == EMERGENT) //意外情况，请使用遥控紧急上锁，飞控就可以在任何情况下紧急中止飞行，锁定飞行器，退出PID控制
		status = EXIT_255;	
	switch(status)
	{		
		case WAITING_1: //等待解锁	
			MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0;  //如果锁定，则电机输出都为0
			if(ALL_flag.unlock)
			{
                            ALL_flag.take_off = 0;	//刚解锁，复位一键起飞
                            ALL_flag.height_lock = 0; //刚解锁，复位定高
                            status = WAITING_2;//解锁后，遥控可以进行操作
			}
		case WAITING_2: //解锁完成后判断使用者是否开始拨动遥杆进行飞行控制
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
					temp = pidHeightRate.out; //输出给电机的是定高输出值
				}
				else //正常飞行状态，油门正常使用
				{
					if(Remote.thr<1040) //油门太低了
					{
						MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0;//清0
						break;
					}
					temp = Remote.thr -1000; //输出给电机的是油门输出值
				}
				
				//将油门值作为基础值给PWM
				MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = LIMIT(temp,0,800); 
				
		
				MOTOR1 +=    + pidRateX.out - pidRateY.out - pidRateZ.out;//; 1号电机分配，姿态输出分配给各个电机的控制量,请看飞控坐标体系图解，与四个电机PWM分布分布
				MOTOR2 +=    + pidRateX.out + pidRateY.out + pidRateZ.out ;//;2号电机分配，
				MOTOR3 +=    - pidRateX.out + pidRateY.out - pidRateZ.out;//3号电机分配，
				MOTOR4 +=    - pidRateX.out - pidRateY.out + pidRateZ.out;//;4号电机分配，
			}	
			break;
		case EXIT_255:
			MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0;  //如果锁定，则电机输出都为0
			status = WAITING_1;	//返回等待解锁
			break;
		default:
			break;
	}
        
        MotorPwmFlash(MOTOR1,MOTOR2,MOTOR3,MOTOR4);
} 
/************************************END OF FILE********************************************/ 
