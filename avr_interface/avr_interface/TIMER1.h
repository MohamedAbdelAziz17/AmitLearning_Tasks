/*
 * TIMER1.h
 *
 *  Created on: Jul 24, 2021
 *      Author: m7mdAG
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#include "TIMER1_CONFIG.h"

extern void MCAL_TIMER1_WITH_INTERRUPT_Initialization(void);
extern void MCAL_TIMER1_WITH_INTERRUPT_Set_Delay(uint32_t Desired, uint8_t ocr,void (*ptrToFunction)(void));

extern void MCAL_TIMER1_FastPWM_Initialization(void);

extern void MCAL_TIMER1_PhasePWM_Initialization(void);

extern void MCAL_TIMER1_PWM_Set_Duty(uint8_t desired_Duty,uint16_t desired_freq);

extern void MCAL_TIMER1_RESUME();
extern void MCAL_TIMER1_STOP();


#endif /* TIMER1_H_ */
