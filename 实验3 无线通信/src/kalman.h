#ifndef _KALMAN_H
#define _KALMAN_H

struct _1_ekf_filter
{
      float LastP;      //< 上次估计协方差
      float Now_P;      //< 当前估计协方差
      float out;        //< 卡尔曼滤波器输出值
      float Kg;         //< 卡尔曼增益
      float Q;          //< 过程噪声协方差
      float R;	        //< 观测噪声协方差        
};

extern void kalman_1(struct _1_ekf_filter *ekf,float input);  //一维卡尔曼

#endif


