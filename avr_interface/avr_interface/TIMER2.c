/*
 * TIMER2.c
 *
 * Created: 11/3/2021 6:28:09 PM
 *  Author: m7mdAG
 */ 
#include "TIMER2_CONFIG.h"
#include "TIMER2.h"
#include "DIO_interface.h"
#include "Common.h"

uint32_t TIMER2_Num_OVF_ = 0;
uint32_t TIMER2_Num_CTC = 0;
uint32_t TIMER2_Initial_Value = 0;  // you can choose it

void (*Timer2_g_ptr_to_func)(void) = NULL_PTR;

/*Polling Mode*/
void MCAL_TIMER2_WITHOUT_INTERRUPT_Initialization(void)
{
	#if WAVE_GENERATION_MODE == NORMAL_MODE
	CLR_BIT(TCCR2, WGM21);CLR_BIT(TCCR2, WGM20);
	#elif WAVE_GENERATION_MODE == CTC_MODE
	SET_BIT(TCCR2, WGM21);CLR_BIT(TCCR2, WGM20);
	#endif
	SET_BIT(TCCR2, CS22);CLR_BIT(TCCR2, CS21);SET_BIT(TCCR2, CS20);
}

void MCAL_TIMER2_WITHOUT_INTERRUPT_Set_Delay(uint32_t Delay, uint8_t ocr)
{
	#if WAVE_GENERATION_MODE == NORMAL_MODE
	uint32_t counter = 0;
	uint8_t Tick = PRESCALER / F_OSC;
	uint32_t Count = (Delay * MISECOND) / Tick;
	uint32_t OVF = Count / BITS;
	OVF++;
	uint32_t InitialValue = BITS - (Count % BITS);
	TCNT2 = InitialValue;
	/*Polling Mode*/
	while (counter != OVF)
	{
		/*Waiting flag after overflow*/
		while (GET_BIT(TIFR, TOV2) != 1);
		/*Re-initialize flag, down flag*/
		SET_BIT(TIFR, TOV2); // set 1 to down flag manually in polling
		counter++;
	}
	#elif WAVE_GENERATION_MODE == CTC_MODE
	uint32_t counter = 0;
	uint8_t Tick = PRESCALER / F_OSC;
	uint32_t Count = (Delay * MISECOND) / Tick;
	OCR2 = ocr;
	uint32_t CTC = Count / OCR2;
	CTC++;
	/*Polling Mode*/
	while (counter != CTC)
	{
		while (GET_BIT(TIFR, OCF2) != 1);
		SET_BIT(TIFR, OCF2);
		counter++;
	}
	#endif
	return;
}

/*Interrupt Mode*/
void MCAL_TIMER2_WITH_INTERRUPT_Initialization(void)
{
	
#if WAVE_GENERATION_MODE == NORMAL_MODE
	
	CLR_BIT(TCCR2, WGM21);CLR_BIT(TCCR2, WGM20);
	
	/*Enable OverFlow Local Interrupt*/
	SET_BIT(TIMSK, TOIE2);
	
#elif WAVE_GENERATION_MODE == CTC_MODE
	
	SET_BIT(TCCR2, WGM21);CLR_BIT(TCCR2, WGM20);
	
	/*Enable OutputCompare Local Interrupt*/
	SET_BIT(TIMSK, OCIE2);
	
#endif
	
	/*Prescalar set to 1024*/
	SET_BIT(TCCR2, CS22);SET_BIT(TCCR2, CS21);SET_BIT(TCCR2, CS20);
	
	/*Enable Global Interrupt*/
	SET_BIT(SREG,7);
	
}

void MCAL_TIMER2_WITH_INTERRUPT_Set_Delay(uint32_t Delay, uint8_t ocr , void (*ptr_to_func)(void))
{
	
	Timer2_g_ptr_to_func = ptr_to_func;	//set user's function as ISR
	
#if WAVE_GENERATION_MODE == NORMAL_MODE
	
	uint8_t Tick = PRESCALER / F_OSC;
	uint32_t Counts = (Delay * MISECOND) / Tick;
	TIMER2_Num_OVF_ = Counts / BITS;
	TIMER2_Initial_Value = BITS - (Counts % BITS); //compensate counting the decimal value by pre-loading
	TCNT2 = TIMER2_Initial_Value; // set data register to initial value
	TIMER2_Num_OVF_++; // must increased by one because the decimal value
	
#elif WAVE_GENERATION_MODE == CTC_MODE
	
	uint8_t Tick = PRESCALER / F_OSC;
	uint32_t Counts = (Delay * MISECOND) / Tick;
	OCR2 = ocr;
	Timer0_Num_CTC = Counts / OCR2;
	Timer0_Num_CTC++;
	
#endif
}

ISR(TIMER2_OVF_vect)
{
#if WAVE_GENERATION_MODE == NORMAL_MODE

	static uint32_t Timer2_desired_counts = 0;
	
	Timer2_desired_counts++;
	
	if(Timer2_desired_counts == TIMER2_Num_OVF_ && Timer2_g_ptr_to_func != NULL_PTR)
	{
		(*Timer2_g_ptr_to_func)();
		Timer2_desired_counts = 0;
		TCNT2 = TIMER2_Initial_Value;
	}
	
#endif
}

void MCAL_TIMER2_FastPWM_Initialization(void)
{
	SET_BIT(DDRD,7);      // OC2 PIN output

	SET_BIT(TCCR2, WGM21);SET_BIT(TCCR2, WGM20);/*Choose Fast PWM*/
	
#if PWM_MODE == NON_INVERTING
	SET_BIT(TCCR2, COM21);CLR_BIT(TCCR2, COM20);/*Non- Inverting Mode*/
	
#elif PWM_MODE == INVERTING
	SET_BIT(TCCR2, COM21);SET_BIT(TCCR2, COM20);
	
#endif

	SET_BIT(TCCR2, CS22);CLR_BIT(TCCR2, CS21);CLR_BIT(TCCR2, CS20); /*Prescalar 1024*/
}

void MCAL_TIMER2_FastPWM_Set_Duty(uint8_t Duty)
{
	#if PWM_MODE == NON_INVERTING
	OCR2 = ((BITS * Duty) / 100) - 1;
	#elif PWM_MODE == INVERTING
	OCR2 = (BITS - 1) - (((BITS * Duty) / 100) - 1);
	#endif
}

void MCAL_TIMER2_PhasePWM_Initialization(void)
{
	
		CLR_BIT(TCCR2, WGM21);SET_BIT(TCCR2, WGM20); // choose PhaseCorrectPWM
		#if PWM_MODE == NON_INVERTING
		SET_BIT(TCCR2, COM21);CLR_BIT(TCCR2, COM20);
		#elif PWM_MODE == INVERTING
		SET_BIT(TCCR2, COM21);SET_BIT(TCCR2, COM20);
		#endif
		
		#if TIMER2_PRESCALER == PRE_CLK1024			// choose prescalar
		TCCR2 = (TCCR2 & 0xF8) | 0x05;				// CS12 CS11 CS10
		#elif TIMER2_PRESCALER == PRE_CLK256
		TCCR2 = (TCCR2 & 0xF8) | 0x04;
		#elif TIMER2_PRESCALER == PRE_CLK64
		TCCR2 = (TCCR2 & 0xF8) | 0x03;
		#elif TIMER2_PRESCALER == PRE_CLK8
		TCCR2 = (TCCR2 & 0xF8) | 0x02;
		#elif TIMER2_PRESCALER == PRE_NOPRE
		TCCR2 = (TCCR2 & 0xF8) | 0x01;
		#endif
		

		MCAL_DIO_voidSetPinDirection(DIO_PORTD,DIO_PIN_7,DIO_OUTPUT); // OC2 pin as output
		
}

void MCAL_TIMER2_PhasePWM_Set_Duty(uint8_t desired_Duty)
{
	OCR2 = ((desired_Duty * BITS) / 100) - 1;
}

void MCAL_TIMER2_STOP()
{
	CLR_BIT(TCCR0 , CS20);
	CLR_BIT(TCCR0 , CS21);
	CLR_BIT(TCCR0 , CS22);
}

void MCAL_TIMER2_RESUME()
{
	#if TIMER0_PRESCALER == PRE_CLK1024       // choose prescalar
	TCCR2 = (TCCR2 & 0xF8) | 0x05;				// CS12 CS11 CS10
	#elif TIMER0_PRESCALER == PRE_CLK256
	TCCR2 = (TCCR2 & 0xF8) | 0x04;
	#elif TIMER0_PRESCALER == PRE_CLK64
	TCCR2 = (TCCR2 & 0xF8) | 0x03;
	#elif TIMER0_PRESCALER == PRE_CLK8
	TCCR2 = (TCCR2 & 0xF8) | 0x02;
	#elif TIMER0_PRESCALER == PRE_NOPRE
	TCCR2 = (TCCR2 & 0xF8) | 0x01;
	#endif
}