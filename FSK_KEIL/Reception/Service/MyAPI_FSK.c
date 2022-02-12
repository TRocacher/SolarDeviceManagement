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
char FlagUART_FSK_RecevChar;
char UART_FSK_RecevChar;
int Count_T_Char;

#define HeaderCar '#'
int HeaderCarCpt;
#define HeaderCarLenMax 5

#define LenCode (2+16) // CRC * 2 + 16 octets de donn�e
#define LenCodeMax (LenCode+10) // s�curit�
char Code[LenCodeMax]; 
int CodeIndex;
#define StopReadingLen (LenCode+1) // on se donne deux T_Char de plus pour lire la trame (s�curit�)

char CodeReadyForRead;




// getter setter
void ResetFlag_CodeReadyForRead (void)
{
	CodeReadyForRead=0;
}

char IsCodeReadyForRead(void)
{
	return CodeReadyForRead;
}	

char * GetCodeAdress(void)
{
	return (Code+2); // pour enlever CRC code
}







void IT_USART_FSK(void)
{
	FlagUART_FSK_RecevChar=1;
}


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
	Init_IT_Serial_Receive_Byte(USART_FSK, IT_USART_FSK);
	GPIO_Configure(GPIOB, 8, OUTPUT, OUTPUT_PPULL ); // RxCmde
	GPIO_Configure(GPIOB, 9, OUTPUT, OUTPUT_PPULL ); // TxCmde
	
	// *****************************
	//  Gestion Timing Timeout UART
	// *****************************
	duree_us=10000000.0/(float)Baud_Rate_bits_par_Sec;  // 10 car start + 8 bits + stop
	Timer_1234_Init(FSK_Timer,duree_us);
	Bloque_Timer(FSK_Timer);
	Active_IT_Debordement_Timer( FSK_Timer, 0, IT_FSK_Timer);
	Count_T_Char=0;
	StateMachine=HeaderWait;
	FlagUART_FSK_RecevChar=0;
	HeaderCarCpt=0;
	CodeIndex=0;
	CodeReadyForRead=0;
	// set module en r�ception par d�faut
	// RxCmde = 1
	Port_IO_Set(GPIOB,8);
}


void SM_Recept_FSK(void)
{
	switch (StateMachine)
	{
		case HeaderWait:
		{
			if (FlagUART_FSK_RecevChar==1)
			{
				FlagUART_FSK_RecevChar=0;
				UART_FSK_RecevChar=Read_Received_Current_Byte(USART_FSK); 
				if (UART_FSK_RecevChar==HeaderCar)HeaderCarCpt++;
				else HeaderCarCpt=0;
				if (HeaderCarCpt==HeaderCarLenMax) 
				{
					// lancement du Timer
					Count_T_Char=0;
					HeaderCarCpt=0;
					Run_Timer(FSK_Timer);
					// Init code
					CodeIndex=0;
					// franchissement �tat
					StateMachine=ReadingCode;
				}
					
			}
			break;
		}
		case ReadingCode:
		{
		if (FlagUART_FSK_RecevChar==1)
			{
				FlagUART_FSK_RecevChar=0;
				Code[CodeIndex]=Read_Received_Current_Byte(USART_FSK); 
				CodeIndex++;
				if (CodeIndex==LenCodeMax)
				{
					CodeIndex=LenCodeMax-1;
					//Ajouter un code erreur !!!
					while(1); /// bug !!!! d�bordement tableau, Time out mal g�r�, visiblement trop long !
				}
				
			}
			if (Count_T_Char==StopReadingLen)
				{
					CodeIndex=0;
					Count_T_Char=0;
					Bloque_Timer(FSK_Timer);
					// franchissement �tat
					StateMachine=ProcessingCode;
				}			
			
			break;
		}
		case ProcessingCode:
		{
			
			// signalisation code pr�t
			CodeReadyForRead=1;
			// franchissement �tat
			StateMachine=WaitForCodeReading;
			break;
		}
		case WaitForCodeReading:
		{
			// franchissement �tat
			if (CodeReadyForRead==0) // le code a �t� lu
			{
			  StateMachine=HeaderWait;
			}	
			break;
		}
		default: break;
	}
}




void FSK_Send_Str(char *Msg)
/*La fonction envoie une cha�ne de caract�re dont l'adresse 
du premier caract�re sera pass� dans le param�tre Msg (pointeur de caract�re).*/ 
{
	// TxCmde = 1
	Port_IO_Set(GPIOB,9);
	Put_String(USART_FSK,Msg);
	GPIO_Clear(GPIOB,9);
}



