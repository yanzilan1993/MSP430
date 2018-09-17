#include "pid.h"
#include "myMath.h"	

/**************************************************************
 *批量复位PID函数
 * @param[in] 
 * @param[out] 
 * @return     
 ***************************************************************/	
void pidRest(PidObject **pid,const uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
	  	pid[i]->integ = 0;
	    pid[i]->prevError = 0;
	    pid[i]->out = 0;
			pid[i]->offset = 0;
	}
}

/**************************************************************
 * Update the PID parameters.
 *
 * @param[in] pid         A pointer to the pid object.
 * @param[in] measured    The measured value
 * @param[in] updateError Set to TRUE if error should be calculated.
 *                        Set to False if pidSetError() has been used.
 * @return PID algorithm output
 ***************************************************************/	
void pidUpdate(PidObject* pid,const float dt)
{
   float error;
   float deriv;
	
    error = pid->desired - pid->measured + pid->offset;         //计算误差

    pid->integ += error * dt;	        //计算积分量
  
    deriv = (error - pid->prevError)/dt;        //计算微分量
	
    pid->out = pid->kp * error + pid->ki * pid->integ + pid->kd * deriv;//PID最终输出
		
    pid->prevError = error;  	        //保存此次误差
}

/**************************************************************
 *  CascadePID
 * @param[in] 
 * @param[out] 
 * @return     
 ***************************************************************/	
void CascadePID(PidObject* pidRate,PidObject* pidAngE,const float dt)  //串级PID
{	 
    pidUpdate(pidAngE,dt);   
    pidRate->desired = pidAngE->out;
    pidUpdate(pidRate,dt);    
}

/*******************************END*********************************/



