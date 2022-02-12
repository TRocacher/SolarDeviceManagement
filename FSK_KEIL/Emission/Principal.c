
#include "clock.h"
#include "API_ADC.h"
#include "Timer_1234.h"
#include "USART_rev2021.h"
#include "FctDiverses.h"
#include "API_FSK.h"
#include "stdio.h"
#include "LCD.h"


char* Reponse;  						// d�claration d'un pointeur de caract�re 
 // r�ponse contient l'adresse du buffer qui contient
														// la cha�ne de caract�res
int i;
int CRC_Val, Sum;
int Time;
char Messg[300];
#define MessgLen 16

void Tim_IT(void)
{
	Time++;
	sprintf(Messg,"123456#####yyNbSec = %d Sec    \n\r", Time); 
  Messg[0]=0xFF;
	Messg[1]=0xAF;
	Messg[2]=0xAF;
	Messg[3]=0xFF;
	Messg[4]=0xAA;
	Messg[5]=0xAA;
	
	//CRC_Val=Messg[2]+Messg[1]*256;
	Sum=0;
			for (i=13;i<(13+MessgLen);i++) // on commence � i = 3 pour �viter 0 et 1 qui contiennent le CRC et 2 qui contient #
			{
				Sum=Sum+Messg[i];
			}
	Messg[11]=(char)(Sum/256);
	Messg[12]= (char)(Sum-256*	Messg[11]);
	
	
	FSK_Send_Str(Messg);
}



int main (void)
{
CLOCK_Configure();
Timer_1234_Init(TIM2, 1000000.0 );
	Init_USART(USART2,38400, 0); // uart de l'xbee utilis� en r�ception
	Time=0;	
	
	lcd_init();
	lcd_clear();
	set_cursor(0, 0);
	lcd_print("Emetteur 16 CRC...");
	
FSK_Init();
Active_IT_Debordement_Timer( TIM2, 2, Tim_IT);
//	Reponse=Get_String(USART1);
	
while(1)
	{
		Reponse=Get_String(USART2);
	i++;
	}	
}


