#include <avr/io.h>											
#include <stdio.h>											
#include <util/delay.h>										
#include "dcMotor.h"

void activeDcMotors()
{
	//TT Gear Motors
	DDRC |=(1<<DDC0) | (1<<DDC1);
	TCCR1B &= ~(1<<WGM13);									
	TCCR1B |= (1<<WGM12);
	TCCR1A |= (1<<WGM10)|(1<<WGM11);						
	TCCR1A |= (1<<COM1A1);									
	TCCR1A &= ~(1<<COM1A0);									
	TCCR1B |= (1<<CS10) | (1<<CS11);						
	TCCR1B &= ~(1<<CS12);									
	DDRD |= (1<<DDD5);										
	OCR1A = 512;
	
	//Cog Wheel
	DDRC |=(1<<DDC2) | (1<<DDC3);
	TCCR1A |= (1<<COM1B1);									
	TCCR1A &= ~(1<<COM1B0);									
	DDRD |= (1<<DDD4);										
	OCR1B = 512;
}

void moveForward()
{
	PORTC |=(1<<PC0);
}

void moveBackward()
{
	PORTC |= (1<<PC1);
}

void cogWheel()
{			
	PORTC |=(1<<PC2);
}

void stopDcMotors()
{
	PORTC &=(~(1<<PC0));
	PORTC &=(~(1<<PC1));
	PORTC &=(~(1<<PC2));
}

