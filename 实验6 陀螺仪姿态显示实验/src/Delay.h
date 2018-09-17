/*
 * Delay.h
 *
 *  Created on: 2015��6��10��
 *      Author: Administrator
 */
// 4M����
#ifndef __DELAY_H_
#define __DELAY_H_

#define CPU_F ((double)1000000)
#define Delay_us(x) __delay_cycles((long)(24*CPU_F*(double)x/1000000.0))
#define Delay_ms(x) __delay_cycles((long)(24*CPU_F*(double)x/1000.0))

#endif /* DELAY_H_ */
