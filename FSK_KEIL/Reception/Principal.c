
#include "clock.h"
#include "API_ADC.h"
#include "Timer_1234.h"
#include "USART_rev2021_b.h"
#include "FctDiverses.h"
#include "API_FSK.h"
#include "stdio.h"
#include "GPIO.h"
#include "MyLCD.h"
#include "NVIC_IT_Ext.h"





void IT_CarrierDetect(void);

int main (void)
{
  CLOCK_Configure();
	Init_USART(USART1,38400, 0); // uart pour check xctu
	
//	MyLCD_Init();
//	MyLCD_Clear();
//	MyLCD_Set_cursor(0, 0);
//	MyLCD_Print("Recepteur...");
//	MyLCD_Set_cursor(0, 1);	
//  FSK_Init();
	
	// IT externe pour Carrier detect
	//NVIC_Ext_IT (GPIOB, 7, FALLING_RISING_EDGE, INPUT_FLOATING,1,IT_CarrierDetect);
	



	// RxCmde = 1
	Port_IO_Set(GPIOB,8);

	
while(1)
	{

	
//	if (Is_Str_Received(USART3)==1)
//		{
//			Reponse=Get_String(USART3); // jamais bloquant puisque un string est reçu !
//			LongRep=Read_Received_StrLen(USART3);
//			
//			ResultatParsing=ReponseParsing();
//			
//			if (ResultatParsing!=-1)
//			{
//			Reponse=Reponse+ResultatParsing;
//			MyLCD_ClearLineDown();
//			MyLCD_Set_cursor(0,1);
//			MyLCD_Print(Reponse);			
//		  
//			}
//      Flush(USART3);
//		}
		if (GetStatus()!=NoReception)
		{
			ResetStatus();
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
