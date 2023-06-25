#include <avr/io.h>
#include <util/delay.h>
#include "temperatureSensor.h"

void ADC_Init(){										
	DDRA &= ~(1<<PA0);							       
	ADCSRA = 0x87;									
	ADMUX = 0x40;									
}

int ADC_Read(char channel)							
{
	ADMUX = 0x40 | (channel & 0x07);			
	ADCSRA |= (1<<ADSC);							
	while (!(ADCSRA & (1<<ADIF)));					
	ADCSRA |= (1<<ADIF);							
	_delay_ms(1);									
	return ADCW;									
}


