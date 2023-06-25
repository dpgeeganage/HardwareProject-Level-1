#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void ADC_Init1()
{
	DDRA &= ~(1<<PA1);		
	ADCSRA = 0x87;	
}

int ADC_Read1()
{
	ADMUX = 0x41;					
	ADCSRA |= (1<<ADSC);			
	while ((ADCSRA &(1<<ADIF))==0);	
	ADCSRA |=(1<<ADIF);				
	return(ADCW);					
}