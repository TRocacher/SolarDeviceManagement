
#include "clock.h"
#include "API_ADC.h"
#include "Timer_1234.h"
#include "USART_rev2021.h"
#include "FctDiverses.h"
#include "API_FSK.h"
#include "stdio.h"
#include "LCD.h"

char* Reponse;  						// déclaration d'un pointeur de caractère 
 // réponse contient l'adresse du buffer qui contient
														// la chaîne de caractères
int i;

int Time;
char Messg[300];
void Tim_IT(void)
{
	Time++;
	sprintf(Messg,"1234#####NbSec = %d Sec\n\r", Time); 
  Messg[0]=0xFF;
	Messg[1]=0xFF;
	Messg[2]=0xFF;
	Messg[3]=0xFF;
	FSK_Send_Str(Messg);
}



int main (void)
{
CLOCK_Configure();
Timer_1234_Init(TIM2, 1000000.0 );
	Init_USART(USART2,38400, 0); // uart de l'xbee utilisé en réception
	Time=0;	
	
	lcd_init();
	lcd_clear();
	set_cursor(0, 0);
	lcd_print("Emetteur...");
	
FSK_Init();
Active_IT_Debordement_Timer( TIM2, 2, Tim_IT);
//	Reponse=Get_String(USART1);
	
while(1)
	{
		Reponse=Get_String(USART2);
	i++;
	}	
}


