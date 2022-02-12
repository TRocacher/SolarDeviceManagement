
#include "clock.h"
#include "API_ADC.h"
#include "Timer_1234.h"
#include "USART_rev2021.h"
#include "FctDiverses.h"
#include "MyAPI_FSK.h"
#include "stdio.h"
#include "GPIO.h"
#include "MyLCD.h"
#include "NVIC_IT_Ext.h"

char* Reponse;  						// déclaration d'un pointeur de caractère 
 // réponse contient l'adresse du buffer qui contient
int time=0;
Status ReadCodeStatus;

void IT_Instrum(void)
{
	time++;
}


void IT_CarrierDetect(void);

int main (void)
{
  CLOCK_Configure();
	
	MyLCD_Init();
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("Recepteur...");
	MyLCD_Set_cursor(0, 1);	
  FSK_Init(38400, USART3, TIM4);
	
	// IT externe pour Carrier detect
	//NVIC_Ext_IT (GPIOB, 7, FALLING_RISING_EDGE, INPUT_FLOATING,1,IT_CarrierDetect);
	
	// instrum
	Timer_1234_Init(TIM2,1000000.0);
	Active_IT_Debordement_Timer( TIM2, 3, IT_Instrum);
	// fin instrum




	
while(1)
	{
	SM_Recept_FSK();
		
	if (IsCodeReadyForRead()==1)
		{
			ResetFlag_CodeReadyForRead();
			Reponse=GetCodeAdress(); 
			ReadCodeStatus=GetCodeStatus();	
			MyLCD_ClearLineDown();
			MyLCD_Set_cursor(0,1);
			if (ReadCodeStatus==OK)	MyLCD_Print(Reponse);	
			else if (ReadCodeStatus==WrongCRC) MyLCD_Print("Wrong CRC");	
			else MyLCD_Print("TimeOut");
		  
		}
	}	
}






void IT_CarrierDetect(void)
{
	if (GPIO_Read(GPIOB,7)==0) // on 
	{	
		USART3->CR1|=USART_CR1_RXNEIE; // UART on
	}
	else
	{	
		USART3->CR1&=~USART_CR1_RXNEIE; // UART off
	}
	// effacement flag
	Clear_Flag_IT_Ext_5_15(7);
}
