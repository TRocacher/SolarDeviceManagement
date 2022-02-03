
#include "clock.h"
#include "API_ADC.h"
#include "Timer_1234.h"
#include "USART_rev2018.h"
#include "FctDiverses.h"
#include "API_FSK.h"
#include "stdio.h"


char* Reponse;  						// déclaration d'un pointeur de caractère 
 // réponse contient l'adresse du buffer qui contient
														// la chaîne de caractères
int i;

int Time;
char Messg[300];
void Tim_IT(void)
{
	Time++;
	sprintf(Messg,"                coucou youhou, test FSK depuis la 119 NbSec  = %d Sec ... 	\n \r", Time); 
	Messg[0]=0xFF;
	Messg[1]=0xFF;
	Messg[2]=0x55;
	Messg[3]=0x55;
	Messg[4]=0xFF;
	Messg[5]=0xFF;
	Messg[6]=0x55;
	Messg[7]=0x55;
	Messg[8]=0xFF;
	Messg[9]=0xFF;
	Messg[10]=0x55;
	Messg[11]=0x55;
	Messg[12]=0xFF;
	Messg[13]=0xFF;
	Messg[14]=0x55;
	FSK_Send_Str(Messg);
}



int main (void)
{
CLOCK_Configure();
Timer_1234_Init(TIM2, 1000000.0 );
	Init_USART(USART2,9600, 0); // uart de l'xbee utilisé en réception
	Time=0;	
FSK_Init();
Active_IT_Debordement_Timer( TIM2, 2, Tim_IT);
//	Reponse=Get_String(USART1);
	
while(1)
	{
		Reponse=Get_String(USART2);
	i++;
	}	
}


