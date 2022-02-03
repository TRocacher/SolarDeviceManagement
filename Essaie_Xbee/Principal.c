
#include "clock.h"
#include "API_ADC.h"
#include "Timer_1234.h"
#include "USART_rev2018.h"
#include "FctDiverses.h"
#include "API_Xbee_ModeAT.h"
#include "GPIO.h"

/* Config Xbee

My AA
Dest BB
Channel C 
PanID  2000


*/



char* Reponse;  						// d�claration d'un pointeur de caract�re 
 // r�ponse contient l'adresse du buffer qui contient
														// la cha�ne de caract�res


int main (void)
{
CLOCK_Configure();
// test dur�e prog
Port_IO_Init(GPIOC,	2, OUTPUT, OUTPUT_PPULL);
GPIO_Clear(GPIOC,2);

Port_IO_Set(GPIOC,2);	
Xbee_Init(0xD, 0x3332, 0xCC);	
Xbee_Fix_DestAdress(0xAA);
GPIO_Clear(GPIOC,2);
//Reponse=Xbee_Get_Str();
	
while(1)
	{
		//Xbee_Send_Str(Reponse);
  	Xbee_Send_Str("Coucou");
		Delay_x_ms(500.0);

	}	
}


