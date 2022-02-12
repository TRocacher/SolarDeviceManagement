#include "GPIO.h"
#include "FctDiverses.h"
#include "USART_rev2021.h"
#include "Timer_1234.h"

USART_TypeDef * USART_FSK;
TIM_TypeDef * FSK_Timer;
float duree_us;


	// *****************************
	//  D�finition graphe d'�tats
	// *****************************
typedef enum {
	HeaderWait,
	ReadingCode,
	ProcessingCode,
	WaitForCodeReading
}RecepSM;

RecepSM StateMachine;

int Count_T_Char;
void IT_FSK_Timer(void)
{
	Count_T_Char++;
	if (Count_T_Char==100) while (1); // s�curit� si la variable n'est pas g�r�e proprement...
																		// Si on tombe l� c'est que la variable s'est envol�e
}

void FSK_Init(int Baud_Rate_bits_par_Sec, USART_TypeDef * USART_FSK_, TIM_TypeDef * FSK_Timer_)
{
	USART_FSK=USART_FSK_;
	FSK_Timer=FSK_Timer_;
	Init_USART(USART_FSK,38400, 0); 
	GPIO_Configure(GPIOB, 8, OUTPUT, OUTPUT_PPULL ); // RxCmde
	GPIO_Configure(GPIOB, 9, OUTPUT, OUTPUT_PPULL ); // TxCmde
	
	// *****************************
	//  Gestion Timing Timeout UART
	// *****************************
	duree_us=10.0/(float)Baud_Rate_bits_par_Sec;  // 10 car start + 8 bits + stop
	Timer_1234_Init(FSK_Timer,duree_us);
	Bloque_Timer(FSK_Timer);
	Active_IT_Debordement_Timer( FSK_Timer, 0, IT_FSK_Timer);
	Count_T_Char=0;
	StateMachine=HeaderWait;
	
}




void FSK_Send_Str(char *Msg)
/*La fonction envoie une cha�ne de caract�re dont l'adresse 
du premier caract�re sera pass� dans le param�tre Msg (pointeur de caract�re).*/ 
{
	// TxCmde = 1
	Port_IO_Set(GPIOB,9);
	Delay_x_ms(10);
	Put_String(USART_FSK,Msg);
	Delay_x_ms(3);
	GPIO_Clear(GPIOB,9);
}


char * FSK_Get_Str(void);

/*La fonction utilise directement la fonction 
char * Get_String(USART_TypeDef *USART) de la lib USART_rev. 
Notez que par d�faut, une cha�ne de caract�res est consid�r�e 
compl�te losque le caract�re <CR> est re�u. Ce caract�re de fin 
de cha�ne est modifiable dans la fichier USART_User_Conf_2018.h.*/

