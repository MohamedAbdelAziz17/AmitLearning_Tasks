/*
 * MAX7221_MODULE.c
 *
 * Created: 3/7/2022 5:55:14 PM
 *  Author: m7mdAG
 */ 

#include "CPU_CONFIGURATION.h"
#include "MAX7221_MODULE.h"

void HAL_MAX7221_Init()
{
	/*Normal Operation (TURN ON)*/
	MCAL_SPI_SS_ENABLE(0);
	MCAL_SPI_Transmit(0x0C);	/*ADDRESS*/
	MCAL_SPI_Transmit(0x01);	/*DATA*/
	MCAL_SPI_SS_DISABLE(0);
	
	/*Scan Limit , Choose number of digits will work*/
	MCAL_SPI_SS_ENABLE(0);
	MCAL_SPI_Transmit(0x0B);
	MCAL_SPI_Transmit(0x02);	/*0 , 1 , 2*/
	MCAL_SPI_SS_DISABLE(0);
	
	/*Decode Mode For Digits*/
	MCAL_SPI_SS_ENABLE(0);
	MCAL_SPI_Transmit(0x09);
	MCAL_SPI_Transmit(0xFF);
	MCAL_SPI_SS_DISABLE(0);
	
	/*Intensity*/
	MCAL_SPI_SS_ENABLE(0);
	MCAL_SPI_Transmit(0x0A);
	MCAL_SPI_Transmit(0x0F);		/*max*/
	MCAL_SPI_SS_DISABLE(0);
}

void HAL_MAX7221_DISPLAY(enumDigits _digit , uint8_t _number)
{
	MCAL_SPI_SS_ENABLE(0);
	MCAL_SPI_Transmit(_digit);	/*Digit1*/
	MCAL_SPI_Transmit(_number);	/*number  = 1*/
	MCAL_SPI_SS_DISABLE(0);
}

void HAL_MAX7221_DISPLAY_ON()
{
	MCAL_SPI_SS_ENABLE(0);
	MCAL_SPI_Transmit(0x0C);	/*ADDRESS*/
	MCAL_SPI_Transmit(0x01);	/*DATA*/
	MCAL_SPI_SS_DISABLE(0);
}

void HAL_MAX7221_DISPLAY_OFF()
{
	MCAL_SPI_SS_ENABLE(0);
	MCAL_SPI_Transmit(0x0C);	/*ADDRESS*/
	MCAL_SPI_Transmit(0x00);	/*DATA*/
	MCAL_SPI_SS_DISABLE(0);
}

void HAL_MAX7221_PUT_NUMBER(uint16_t _number)
{
	
	if( _number >= 75)
	{
		_number = 75;
	}
	else if(_number <= 35)
	{
		_number = 35;
	}
	
	
	MCAL_SPI_SS_ENABLE(0);
	MCAL_SPI_Transmit(DIG1);	/*Digit1*/
	MCAL_SPI_Transmit(_number/10);	/*number  = 1*/
	MCAL_SPI_SS_DISABLE(0);
		
	MCAL_SPI_SS_ENABLE(0);
	MCAL_SPI_Transmit(DIG2);	/*Digit1*/
	MCAL_SPI_Transmit(_number%10);	/*number  = 1*/
	MCAL_SPI_SS_DISABLE(0);
}