#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "ultrasonicSensor.h"

int distance;

int ultraLeft() 
{
	DDRB|=(1<<PB5);
	
	PORTB|=(1<<PB5);
	_delay_us(15);
	PORTB&=~(1<<PB5);
	
	uint32_t i,result;
	for(i=0;i<600000;i++)
	{
		if(!(PINB & (1<<PB4)))
			continue;
		else
			break;
	}	
	if(i==600000)
		return ((US_ERROR*0.034)/2.0+1);
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);
	TCNT1=0x00;
	for(i=0;i<600000;i++)
	{
		if(PINB & (1<<PB4))
		{
			if(TCNT1 > 60000)				break;
			else
				continue;
		}
		else
			break;
	}
	if(i==600000)
		return ((US_NO_OBSTACLE*0.034)/2.0+1);
	result=TCNT1;
	TCCR1B=0x00;
	if(result > 60000)
		return ((US_NO_OBSTACLE*0.034)/2.0+1);
	else
		return ((result>>1)*0.034/2.0+1);	
}


int ultraRight()
{
	DDRB|=(1<<PB1);
		
	PORTB|=(1<<PB1);
	_delay_us(15);
	PORTB &= ~(1<<PB1);
	
	uint32_t i,result;
	for(i=0;i<600000;i++)
	{
		if(!(PINB & (1<<PB0)))
			continue;
		else
			break;
	}
	if(i==600000)		return ((US_ERROR*0.034)/2.0+1);
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);
	TCNT1=0x00;	for(i=0;i<600000;i++)
	{
		if(PINB & (1<<PB0))
		{
			if(TCNT1 > 60000)
				break;
			else
				continue;
		}
		else
			break;
	}	if(i==600000)
		return ((US_NO_OBSTACLE*0.034)/2.0+1);
	result=TCNT1;
	TCCR1B=0x00;
	if(result > 60000)
		return ((US_NO_OBSTACLE*0.034)/2.0+1);
	else
		return ((result>>1)*0.034/2.0+1);	
}

int ultraMiddle()
{
	DDRB|=(1<<PB3);
		
	PORTB|=(1<<PB3);
	_delay_us(15);
	PORTB &= ~(1<<PB3);
	
	uint32_t i,result;
	for(i=0;i<600000;i++)
	{
		if(!(PINB & (1<<PB2)))
			continue;
		else
			break;
	}
	if(i==600000)		return ((US_ERROR*0.034)/2.0+1);
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);
	TCNT1=0x00;	for(i=0;i<600000;i++)
	{
		if(PINB & (1<<PB2))
		{
			if(TCNT1 > 60000)
				break;
			else
				continue;
		}
		else
			break;
	}	if(i==600000)
		return ((US_NO_OBSTACLE*0.034)/2.0+1);
	result=TCNT1;
	TCCR1B=0x00;
	if(result > 60000)
		return ((US_NO_OBSTACLE*0.034)/2.0+1);
	else
		return ((result>>1)*0.034/2.0+1);	
}

