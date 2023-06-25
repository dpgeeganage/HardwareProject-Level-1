#ifndef USART_RS232_H_FILE_H_								
#define USART_RS232_H_FILE_H_

#define BAUD_PRESCALE (((F_CPU/ (BAUDRATE * 16UL))) - 1)	

void USART_Init(unsigned long);								
char USART_RxChar();										
void USART_TxChar(char);									
void USART_SendString(char*);								


#endif														