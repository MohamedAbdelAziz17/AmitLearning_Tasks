/*
 * avr_interface.c
 *
 * Created: 2/11/2022 12:13:46 AM
 * Author : m7mdAG
 */ 

/*Compiler Libraries*/
#include <stdlib.h>
#include <stdbool.h>

/*Common Libraries*/
#include "STD_TYPES.h"
#include "CPU_CONFIGURATION.h"
#include "Common.h"

/*Peripherals Drivers*/
#include "DIO_interface.h"
#include "INT_interface.h"
#include "ADC_Interface.h"
#include "TIMER0.h"
#include "TIMER1.h"
#include "TIMER2.h"
#include "UART.h"
#include "SPI.h"
#include "TWI.h"

/*Modules Drivers*/
#include "EEPROM_24C16.h"
#include "MAX7221_MODULE.h"


/*INPUT OUTPUT Ports and Pins*/
#define UP_BUTTON_PORT			DIO_PORTB,DIO_PIN_0
#define DOWN_BUTTON_PORT		DIO_PORTB,DIO_PIN_1
#define ON_OFF_SWITCH_PORT		DIO_PORTB,DIO_PIN_2

#define Cooling_EL_PORT			DIO_PORTA,DIO_PIN_5
#define Heating_EL_PORT			DIO_PORTA,DIO_PIN_6
#define Heating_EL_LED_PORT		DIO_PORTA,DIO_PIN_7

/*Global Variables*/
bool Heater_State = false;
bool Cooling_State = false;
bool Electric_Heater_State = false;

 uint16_t	Set_Temp		= 60;
 uint16_t	Current_Temp	= 0;

/*Application Functions*/
void Heater_Turn_On()
{
	Heater_State = true;
	
	MCAL_TIMER0_RESUME();
	MCAL_DIO_voidSetPinValue(Heating_EL_PORT,DIO_HIGH);
}

void Heater_Turn_Off()
{
	Heater_State = false;
	
	MCAL_TIMER0_STOP();
	MCAL_DIO_voidSetPinValue(Heating_EL_PORT,DIO_LOW);
}

void Cooling_Turn_On()
{
	Cooling_State = true;
	
	MCAL_TIMER0_STOP();
	MCAL_DIO_voidSetPinValue(Heating_EL_LED_PORT , DIO_HIGH);
	MCAL_DIO_voidSetPinValue(Cooling_EL_PORT	 , DIO_HIGH);
}

void Cooling_Turn_Off()
{
	Cooling_State = false;
	
	MCAL_DIO_voidSetPinValue(Heating_EL_LED_PORT , DIO_LOW);
	MCAL_DIO_voidSetPinValue(Cooling_EL_PORT	 , DIO_LOW);
	MCAL_TIMER0_RESUME();	
}

void Timer1_ISR()
{
	static uint8_t logic = 0;
	
	logic ^= 1 ;
	
	if(logic)
		HAL_MAX7221_DISPLAY_ON();

	else
		HAL_MAX7221_DISPLAY_OFF();
			
}

void POWER_ISR()
{
	
	MCAL_DIO_voidGetPinValue(Cooling_EL_PORT , &Heater_State);
	MCAL_DIO_voidGetPinValue(Heating_EL_PORT , &Cooling_State);
	
	if( (Heater_State | Cooling_State) == 1)  /*electric heater is on*/
	{
		Cooling_Turn_Off();		
		Heater_Turn_Off();
		
		HAL_MAX7221_DISPLAY_OFF();
		
		Electric_Heater_State = false;
	}
	else
	{
		Heater_Turn_On();
		
		HAL_MAX7221_DISPLAY_ON();
		
		Electric_Heater_State = true;
	}
}

void Timer2_ISR()
{
	HAL_MAX7221_PUT_NUMBER(Current_Temp);
	
}

void UP_BUTTON_ISR()
{
	Set_Temp += 5 ;
	
	if( Set_Temp > 75)
	{
		Set_Temp = 75;
	}
	
	HAL_MAX7221_PUT_NUMBER(Set_Temp);

	HAL_EEPROM_WR(EEPROM_PAGE1_ADDRESS , 0 , Set_Temp);				/*Store new set Temperature*/
	
	MCAL_TIMER2_WITH_INTERRUPT_Set_Delay(1000 , 0 , Timer2_ISR);	/*recount time at displaying Current Temperature*/
	
		
}

void DOWN_BUTTON_ISR()
{
	Set_Temp -= 5 ;
	
	if(Set_Temp < 35)
	{
		Set_Temp = 35;
	}
	
	HAL_MAX7221_PUT_NUMBER(Set_Temp);
	
	HAL_EEPROM_WR(EEPROM_PAGE1_ADDRESS , 0 , Set_Temp);				/*Store new set Temperature*/
	
	MCAL_TIMER2_WITH_INTERRUPT_Set_Delay(1000 , 0 , Timer2_ISR);	/*recount time at displaying Current Temperature*/
	
}

void Heating_LED_ISR()
{
	 static uint8_t toggle_led = 0;
	 
	 toggle_led ^= 1;
	 
	 MCAL_DIO_voidSetPinValue(Heating_EL_LED_PORT , toggle_led);
}




int main(void)
{	
	/**UART Display for Debugging on Virtual Terminal**/
	MCAL_UART_Init(NULL_PTR);
	
	
	/** Seven SEGMENT DISPLAY **/
	MCAL_SPI_Init(NULL_PTR);
	HAL_MAX7221_Init();
	HAL_MAX7221_PUT_NUMBER(Set_Temp);
	
	
	/** Heating Element , Cooling Element**/
	MCAL_DIO_voidSetPinDirection(Cooling_EL_PORT	 ,DIO_OUTPUT);
	MCAL_DIO_voidSetPinDirection(Heating_EL_PORT	 ,DIO_OUTPUT);
	MCAL_DIO_voidSetPinDirection(Heating_EL_LED_PORT ,DIO_OUTPUT);
	
	
	/** LM35 ADC Initialize **/
	ADC_ConfigStr_t LM35_Config = {ADC_AVCC_WITH_EXTERNAL_CAPACITORS_AT_VREF , ADC_POLLING , ADC_NO_AUTO_TRIGGER , ADC_PRESCALAR_64 , ADC_RESOLUTION10};
	MCAL_ADC_Init ( &LM35_Config );
	
	
	/** External EEPROM **/
	MCAL_TWI_MASTER_Init();
	HAL_EEPROM_RD (EEPROM_PAGE1_ADDRESS , 0 , &Set_Temp);	/*Retrieve set temperature*/
	
	
	/**	ON/OFF Button Interrupt**/
	MCAL_EXTINT_u8Init(EXTINT_2 , EXTINT_FALLING_EDGE);
	MCAL_EXTINT_u8SetCallBack(EXTINT_2 , POWER_ISR);
	/** UP BUTTON Interrupt**/
	MCAL_EXTINT_u8Init(EXTINT_0 , EXTINT_RISING_EDGE);
	MCAL_EXTINT_u8SetCallBack(EXTINT_0 , UP_BUTTON_ISR);
	/** DOWN BUTTON Interrupt**/
	MCAL_EXTINT_u8Init(EXTINT_1 , EXTINT_RISING_EDGE);
	MCAL_EXTINT_u8SetCallBack(EXTINT_1 , DOWN_BUTTON_ISR);
	
	
	/**Timer0 Initialize**/
	MCAL_TIMER0_WITH_INTERRUPT_Initialization();
	MCAL_TIMER0_WITH_INTERRUPT_Set_Delay(150, 0, Heating_LED_ISR);	/*Must be 1 second but 0.15 second chosen for a better simulation on Proteus*/

	/**Timer1 Initialize**/
	//MCAL_TIMER1_WITH_INTERRUPT_Initialization();
	//MCAL_TIMER1_WITH_INTERRUPT_Set_Delay(200 , 0 , Timer1_ISR);
	//MCAL_TIMER1_STOP();
	
	/**Timer2 Initialize**/
	MCAL_TIMER2_WITH_INTERRUPT_Initialization();
	MCAL_TIMER2_WITH_INTERRUPT_Set_Delay(500 , 0 , Timer2_ISR);    /*Must be 5 seconds but 1 second chosen for a better simulation on Proteus*/
	
	
	/*********Start Up Configurations*********/
		Cooling_Turn_Off();
		Heater_Turn_Off(); 
		HAL_MAX7221_DISPLAY_OFF();
	
	/*Read water temperature and display it at start*/
	
		_delay_ms(300);		/*LM35 takes time to start sending correct signals*/
	
		MCAL_ADC_StartConversionBusyWait( ADC_CHANNEL0 , &Current_Temp );	/*read LM35 Signal*/
	
		Current_Temp = ( (uint32_t) Current_Temp * 500 ) / 1024 ;
		
		HAL_MAX7221_PUT_NUMBER ( Current_Temp ) ;			/*display Current Temperature at start up*/
	
	
    while (1) 
	{	
				
		if( Electric_Heater_State )					/*if power is on*/
		{
			
			MCAL_ADC_StartConversionBusyWait( ADC_CHANNEL0 , &Current_Temp );	/*read water temperature*/
			Current_Temp = ( (uint32_t) Current_Temp * 500 ) / 1024 ;
	
			/*Choose Heating or Cooling*/
			if( Current_Temp < Set_Temp )			/*turn on heating*/
			{
				Heater_Turn_On();
				Cooling_Turn_Off();
			}
			else if( Current_Temp > Set_Temp )		/*turn on cooling*/
			{
				Heater_Turn_Off();
				Cooling_Turn_On();
			}
			else if( Current_Temp == Set_Temp)		/*turn off electric heater*/
			{
				Cooling_Turn_Off();
				Heater_Turn_Off();
			}
		
		
			_delay_ms(100);		/*read temperature every 100ms*/	/*for better time handling better using ADC interrupt timer*/
		}
		

    }
	
}




