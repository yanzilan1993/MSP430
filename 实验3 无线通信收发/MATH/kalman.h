#ifndef _KALMAN_H
#define _KALMAN_H

struct _1_ekf_filter
{
      float LastP;      //< �ϴι���Э����
      float Now_P;      //< ��ǰ����Э����
      float out;        //< �������˲������ֵ
      float Kg;         //< ����������
      float Q;          //< ��������Э����
      float R;	        //< �۲�����Э����        
};

extern void kalman_1(struct _1_ekf_filter *ekf,float input);  //һά������

#endif


