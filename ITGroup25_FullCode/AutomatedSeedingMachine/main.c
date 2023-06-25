#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bluetoothModule/USART_RS232_H_file.h"
#include "temperatureSensor/temperatureSensor.h"
#include "dcMotor/dcMotor.h"
#include "ultrasonicSensor/ultrasonicSensor.h"
#include "servoMotor/servoMotor.h"
#include "soilMoistureSensor/soilMoistureSensor.h"

char Temperature[10], height[10], array[10];
float celsius, moisture;
char Data_in;
int distanceLeft, distanceRight, distanceMiddle;
int dType=0, noSeeds=0, fill=1, hBed=0;

void menu1();
void dmenu();
void nmenu();
void menu2();
void waterPump();
void measure();
void manualMode();
int check();											// IR Sensor
void buzzer();
void screw();											// Stepper Motor
int seedingProcess();
void rObstacle();
void lObstacle();

int main(void)
{
	ADC_Init();											
	USART_Init(9600);									
	activeServoMotors();
	
	while(1)
	{
		menu1();
		
		Data_in=USART_RxChar();
		
		if (Data_in=='1')
		{
			while (1)
			{
				dmenu();
				dType=USART_RxChar()-'0';
				if ((dType<=0) || (dType>=6))
				{
					USART_SendString("Invalid Entry!\r\n\r\n");
					continue;
				}
				break;
			}
		}
		else if (Data_in=='2')
		{
			while (1)
			{
				nmenu();
				noSeeds=USART_RxChar()-'0';
				if ((noSeeds<=0) || (noSeeds>=6))
				{
					USART_SendString("Invalid Entry!\r\n\r\n");
					continue;
				}
				break;
			}
		}
		else if (Data_in=='3')
			measure();
		else if (Data_in=='4')
		{
			if ((distanceMiddle<=hBed && hBed!=0) && (check()) && (noSeeds!=0) && (dType!=0))
				USART_SendString("Auto Mode Activated!\r\n");
			
			while((distanceMiddle<=hBed && hBed!=0) && (check()) && (noSeeds!=0) && (dType!=0))
			{
				distanceLeft=ultraLeft();
				distanceRight=ultraRight();
				
				if((distanceRight>51) && (distanceLeft>51))
				{
					if (seedingProcess()==0)
						break;
				}
				else if (distanceRight<=51)
					rObstacle();
				else if (distanceLeft<=51)
					lObstacle();
				
				distanceMiddle=ultraMiddle();
			}
			
			stopDcMotors();
			
			if (check()==0)
			{
				USART_SendString("Fill the Seed Container!\r\n");
				buzzer();
			}
			if (hBed==0)
				USART_SendString("Measure the height of the bed!\r\n");
			if (noSeeds==0)
				USART_SendString("Enter number of seeds!\r\n");
			if (dType==0)
				USART_SendString("Enter distance between two holes!\r\n");
			USART_SendString("\r\n");
			if (distanceMiddle>hBed)
				USART_SendString("End of the Bed!\r\nAuto Mode Deactivated!\r\n\r\n");
			
			_delay_ms(2000);
		}
		else if (Data_in=='5')
			manualMode();
		else
			USART_SendString("Invalid Entry!\r\n\r\n");
	}
}


void menu1()
{
	_delay_ms(500);
	USART_SendString("***Automatic Seeding Machine***\r\n");
	USART_SendString("1-Enter distance between two holes\r\n2-Enter number of seeds\r\n");
	USART_SendString("3-Measure the Temperature,Soil Moisture & height of the bed\r\n4-Auto Mode\r\n5-Manual Mode\r\n\r\n");
	_delay_ms(500);
}

void dmenu()
{
	_delay_ms(500);
	USART_SendString("1-44cm\r\n2-88cm\r\n3-132cm\r\n4-176cm\r\n5-220cm\r\n\r\n");
	_delay_ms(500);
}

void nmenu()
{
	_delay_ms(500);
	USART_SendString("1-1 seed\r\n2-2 seeds\r\n3-3 seeds\r\n4-4 seeds\r\n5-5 seeds\r\n\r\n");
	_delay_ms(500);
}

void menu2()
{
	USART_SendString("\r\n***Manual Mode***\r\n8-Move Forward\r\n2-Move Backward\r\n6-Turn Right\r\n4-Turn Left\r\n5-Stop\r\n0-Deactivate Manual Mode\r\n\r\n");
}

void waterPump()
{
	DDRD |= (1<<PD3);
	
	if(moisture<40)
	{
		PORTD |= (1<<PD3);
		USART_SendString("Water Pump Activated!\r\n");
	}
	else
	{
		PORTD &= ~(1<<PD3);
		USART_SendString("Water Pump Not Activated!\r\n");
	}
}

void measure()
{
	celsius = (ADC_Read(0)*4.88);
	celsius = (celsius/10.00);
	
	sprintf(Temperature,"Temperature %d°C\r\n",(int)celsius);	
	USART_SendString(Temperature);
	
	_delay_ms(1000);
	
	ADC_Init1();												

	USART_SendString("Moving Down...\r\n");
	moveDown();
	straight();
	USART_SendString("Moving Up...\r\n");
	moveUp();
	
	moisture = ADC_Read1();
	moisture = (moisture*100.00)/1023.00;						

	dtostrf(moisture,3,2,array);

	strcat(array,"%\r\n");										
	
	USART_SendString("Moisture ");
	USART_SendString(array);
	
	waterPump();
	
	distanceMiddle=ultraMiddle();
	hBed=distanceMiddle;
	
	sprintf(height,"Height %d cm\r\n\r\n",(int)hBed);
	USART_SendString(height);
	_delay_ms(1000);
}

void manualMode()
{
	USART_SendString("Manual Mode Activated!\r\n");
	menu2();
	activeDcMotors();
	while (1)
	{
		Data_in=USART_RxChar();
		
		if (Data_in=='8')
		{
			USART_SendString("Moving Forward...\r\n");
			moveForward();
		}
		else if (Data_in=='2')
		{
			USART_SendString("Moving Backward...\r\n");
			moveBackward();
		}
		else if (Data_in=='5')
		{
			USART_SendString("Moving Stopped!\r\n");
			stopDcMotors();
			straight();
		}
		else if (Data_in=='6')
		{
			USART_SendString("Turn Right!\r\n");
			turnRight();
		}
		else if (Data_in=='4')
		{
			USART_SendString("Turn Left!\r\n");
			turnLeft();
		}
		else if (Data_in=='0')
		{
			USART_SendString("Manual Mode Deactivated!\r\n\r\n");
			break;
		}
		else
		{
			USART_SendString("\r\nInvalid Entry!\r\n");
			menu2();
		}
		
	}
}

int check()
{
	DDRB  &= ~(1<PB6);
	
	if ((PINB & (1<<PB6)) != 0)
		return 0;	else
		return 1;
}

void buzzer()
{
	DDRB |= (1<<PB7);
	PORTB |= (1<<PB7);
	_delay_ms(1000);
	PORTB &= ~(1<<PB7);
}

void screw()
{
	DDRC |= (1<<PC4) | (1<<PC5) | (1<<PC6) | (1<<PC7);
	
	USART_SendString("Screw moving down!\r\n");
	PORTC = (1<<PC7);    
	_delay_ms(150);
	PORTC = (1<<PC4);      
	_delay_ms(2000);
	USART_SendString("Screw moving up!\r\n\r\n");
	PORTC = (1<<PC7);        
	_delay_ms(150);
	PORTC = (1<<PC5);       
	_delay_ms(2000);
}

int seedingProcess()
{
	USART_SendString("Moving Forward...\r\n");
	activeDcMotors();
	moveForward();
	
	for (int x=0;x<dType;x++)
		_delay_ms(535);
	
	USART_SendString("Moving Stopped!\r\n");
	stopDcMotors();
	_delay_ms(2000);
	
	if (distanceMiddle<=hBed)
	{
		USART_SendString("Seeds coming out from the container!\r\n");
		activeDcMotors();
		cogWheel();
		
		for (int x=0;x<noSeeds;x++)
			_delay_ms(470);
		
		USART_SendString("Cog wheel Stopped!\r\n");
		stopDcMotors();
		_delay_ms(2000);
		
		USART_SendString("Digging the hole!\r\n");
		screw();
		return 1;
	}
	else
		return 0;
}

void rObstacle()
{
	activeDcMotors();
	USART_SendString("Obstacle detected!\r\n");
	USART_SendString("Moving Stopped!\r\n");
	stopDcMotors();
	_delay_ms(1000);
	USART_SendString("Turn Right...\r\n");
	turnRight();
	_delay_ms(1000);
	USART_SendString("Moving Forward...\r\n");
	moveForward();
	_delay_ms(2000);
	USART_SendString("Moving Stopped!\r\n");
	stopDcMotors();
	_delay_ms(1000);
	straight();
	_delay_ms(1000);
	USART_SendString("Turn Left...\r\n");
	turnLeft();
	_delay_ms(1000);
	USART_SendString("Moving Forward...\r\n");
	moveForward();
	_delay_ms(2000);
	USART_SendString("Moving Stopped!\r\n\r\n");
	stopDcMotors();
	_delay_ms(1000);
	straight();
	_delay_ms(1000);
}

void lObstacle()
{
	activeDcMotors();
	USART_SendString("Obstacle detected!\r\n");
	USART_SendString("Moving Stopped!\r\n");
	stopDcMotors();
	_delay_ms(1000);
	USART_SendString("Turn Left...\r\n");
	turnLeft();
	_delay_ms(1000);
	USART_SendString("Moving Forward...\r\n");
	moveForward();
	_delay_ms(2000);
	USART_SendString("Moving Stopped!\r\n");
	stopDcMotors();
	_delay_ms(1000);
	straight();
	_delay_ms(1000);
	USART_SendString("Turn Right...\r\n");
	turnRight();
	_delay_ms(1000);
	USART_SendString("Moving Forward...\r\n");
	moveForward();
	_delay_ms(2000);
	USART_SendString("Moving Stopped!\r\n\r\n");
	stopDcMotors();
	_delay_ms(1000);
	straight();
	_delay_ms(1000);
}
