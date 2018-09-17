
#include "imu.h"
#include "myMath.h"
#include <math.h>


static float NormAccz;

typedef volatile struct {  //四元数
  float q0;
  float q1;
  float q2;
  float q3;
} Quaternion;

/*****************************************************************************************
 * 四元数
 * @param[in] MPU6050数据
 * @param[out] 角度值
 * @brief 此函数有一定的难度，姿态解算就一个函数，采用四元数梯度下降法估计姿态。
					提取重力分量->获取加速度补偿值->角速度积分值->
                                        一介龙格库塔法补偿四元数->提取四元数旋转矩阵的姿态角
 * @return     
 ******************************************************************************************/
void GetAngle(const _st_Mpu *pMpu,_st_AngE *pAngE, float dt)
{		
    volatile struct V{
                            float x;
                            float y;
                            float z;
                            } Gravity,Acc,Gyro,AccGravity;

    static struct V GyroIntegError = {0};
    static  float KpDef = 0.8f ; //四元数收敛值
    static  float KiDef = 0.0003f; //加速度补偿角速度积分值
    static Quaternion NumQ = {1, 0, 0, 0};
    float q0_t,q1_t,q2_t,q3_t;

    float NormQuat; 
    float HalfTime = dt * 0.5f;

    Gravity.x = 2*(NumQ.q1 * NumQ.q3 - NumQ.q0 * NumQ.q2);								
    Gravity.y = 2*(NumQ.q0 * NumQ.q1 + NumQ.q2 * NumQ.q3);						  
    Gravity.z = 1-2*(NumQ.q1 * NumQ.q1 + NumQ.q2 * NumQ.q2);	

    NormQuat = Q_rsqrt(squa(MPU6050.accX)+ squa(MPU6050.accY) +squa(MPU6050.accZ));
	
    Acc.x = pMpu->accX * NormQuat; //归一后可化为单位向量下方向分量
    Acc.y = pMpu->accY * NormQuat;
    Acc.z = pMpu->accZ * NormQuat;	

    AccGravity.x = (Acc.y * Gravity.z - Acc.z * Gravity.y);
    AccGravity.y = (Acc.z * Gravity.x - Acc.x * Gravity.z);
    AccGravity.z = (Acc.x * Gravity.y - Acc.y * Gravity.x);

    GyroIntegError.x += AccGravity.x * KiDef;
    GyroIntegError.y += AccGravity.y * KiDef;
    GyroIntegError.z += AccGravity.z * KiDef;

    Gyro.x = pMpu->gyroX * Gyro_Gr + KpDef * AccGravity.x  +  GyroIntegError.x;//弧度制，，此处补偿的是角速度的漂移
    Gyro.y = pMpu->gyroY * Gyro_Gr + KpDef * AccGravity.y  +  GyroIntegError.y;
    Gyro.z = pMpu->gyroZ * Gyro_Gr + KpDef * AccGravity.z  +  GyroIntegError.z;		

    q0_t = (-NumQ.q1*Gyro.x - NumQ.q2*Gyro.y - NumQ.q3*Gyro.z) * HalfTime;
    q1_t = ( NumQ.q0*Gyro.x - NumQ.q3*Gyro.y + NumQ.q2*Gyro.z) * HalfTime;
    q2_t = ( NumQ.q3*Gyro.x + NumQ.q0*Gyro.y - NumQ.q1*Gyro.z) * HalfTime;
    q3_t = (-NumQ.q2*Gyro.x + NumQ.q1*Gyro.y + NumQ.q0*Gyro.z) * HalfTime;

    NumQ.q0 += q0_t; //积分后的值累加到上次的四元数中，即新的四元数
    NumQ.q1 += q1_t;
    NumQ.q2 += q2_t;
    NumQ.q3 += q3_t;

    NormQuat = Q_rsqrt(squa(NumQ.q0) + squa(NumQ.q1) + squa(NumQ.q2) + squa(NumQ.q3)); 
    NumQ.q0 *= NormQuat; //模长更新四元数值
    NumQ.q1 *= NormQuat;
    NumQ.q2 *= NormQuat;
    NumQ.q3 *= NormQuat;	

    {
        float vecxZ = 2 * NumQ.q0 *NumQ.q2 - 2 * NumQ.q1 * NumQ.q3 ;
        float vecyZ = 2 * NumQ.q2 *NumQ.q3 + 2 * NumQ.q0 * NumQ.q1;
        float veczZ = 1 - 2 * NumQ.q1 *NumQ.q1 - 2 * NumQ.q2 * NumQ.q2;		

#ifdef	YAW_GYRO
        *(float *)pAngE = atan2f(2 * NumQ.q1 *NumQ.q2 + 2 * NumQ.q0 * NumQ.q3, 1 - 2 * NumQ.q2 *NumQ.q2 - 2 * NumQ.q3 * NumQ.q3) * RtA;  //yaw
#else  
        float yaw_G = pMpu->gyroZ * Gyro_G;
        if((yaw_G > 1.0f) || (yaw_G < -1.0f)) 
        {
                pAngE->yaw  += yaw_G * dt;//角速度积分成偏航角			
        }
#endif
        pAngE->pitch  =  asin(vecxZ)* RtA;	 //俯仰角					

        pAngE->roll	= atan2f(vecyZ,veczZ) * RtA;	//横滚角

        NormAccz = pMpu->accX* vecxZ + pMpu->accY * vecyZ + pMpu->accZ * veczZ;	/*Z轴垂直方向上的加速度，此值涵盖了倾斜时在Z轴角速度的向量和，不是单纯重力感应得出的值*/				
    }
}


float GetNormAccz(void)
{
	return NormAccz;
}
/***************************************************END OF FILE***************************************************/




