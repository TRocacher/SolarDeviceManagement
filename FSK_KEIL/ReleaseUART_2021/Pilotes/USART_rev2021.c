

#include "stm32f10x.h"
#include "Clock.h"
#include "GPIO.h"
#include "NVIC_IT_Ext.h"
#include "USART_rev2021.h"
#include "USART_User_Conf_2021.h"


// 13/10/2021 : ajout de la fonction de lecture du flag 
// Is_StringReceived(USART_TypeDef *USART)
// pour permettre une lecture non bloquante

//=======================================================================================
// 	Auteur T.R.
//  Sept 2018
//  Processeur : STM32F103RB
//  Logiciel dev : µVision 5.23
//  Valider Simu 3USART séparément, avec USART_Rev3b.h et USART_User_Conf_3.h
//  
//=======================================================================================




//=======================================================================================
// Rappel sur les ressources sur le STM32F103RB 
//
// 3 USART possibles :
// USART_1_TX = TIM1_CH2 = PA9
// USART_1_RX = TIM1_CH3 = PA10
// USART_2_TX = TIM2_CH3 = PA2
// USART_2_RX = TIM2_CH4 = PA3
// USART_3_TX = PB10
// USART_3_RX = PB11
//=======================================================================================






//=======================================================================================
// **************************************************************************************
//=======================================================================================
// Données structurées pour l'émission et la réception 
//=======================================================================================
// **************************************************************************************
//=======================================================================================


typedef struct
{
	int StringLen;		// public readable
	int StringMaxLen;
	} StringTransmissionData_Typedef;

StringTransmissionData_Typedef USART1_TransmData;
StringTransmissionData_Typedef USART2_TransmData;
StringTransmissionData_Typedef USART3_TransmData;
// tableau de pointeur sur donnée structurée (pour simplifier les fct)
StringTransmissionData_Typedef * StrTransmDataTable[3];

	
typedef struct
{
	char * PtrBuffer; // public readable
	int IndexStr;
	char CurrentByte; // public readable
	char EndCar1;
	char EndCar2;
	char FlagReception;
	char FlagOverWrite; 
	int  StringLen;		// public readable
	char EnableByteCallback;
	void (* ByteCallback)(void);
	char EnableStrCallback;
	void (* StrCallback)(void);
} StringReceptionData_Typedef;
/*
char * PtrBuffer
C'est un pointeur de caractère. Il contient l'adresse du premier caractère reçu.
Le simulateur est capable d'afficher non seulement le premier caractère, 
mais tous les autres ! On peut donc visualiser en temps réel la chaîne de caractère
en cours de réception. Très pratique pour débugger !

int IndexStr
C'est l'index qui parcourt la chaîne de caractère au fur et à mesure de la récpetion
d'octet.

char CurrentByte
C'est le dernier octet reçu dans la chaîne.

char EndCar1
C'est la valeur du premier caractère de fin. A l'initialisation, 
il est égal à  Term_Car_1_USARTx (x vallant 1,2 ou3), voir 3.1

char EndCar2
Idem pour le second caractère 

char FlagReception
Indicateur positionné à '1'  lorsque :
- le ou les deux caractère(s) de fin est (sont) trouvé(s), voir 3.1
- le nombre maximum autorisé de caractère est atteint
Remis à 0 lorsque :
- la lecture du string est faite

char FlagOverWrite 
Cet indicateur passe à '1' si un string est en cours de réception  
(buffer en cours de remplissage) et que le string précédent n'a pas été lu.

int  StringLen
C'est la longueur de la chaîne reçue (Null non compté bien sûr)

char EnableByteCallback
Ce paramètre  de validation indique que l'utilisateur a souhaité déclencher
un callback à la réception d'un octet.

void (* ByteCallback)(void)
Adresse de la fonction callback à appeler à la réception d'un octet

char EnableStrCallback;
Ce paramètre  de validation indique que l'utilisateur a souhaité déclencher 
un callback à la réception d'un string.

void (* StrCallback)(void)
Adresse de la fonction callback à appeler à la réception d'un string

*/



StringReceptionData_Typedef USART1_ReceptionData;
StringReceptionData_Typedef USART2_ReceptionData;
StringReceptionData_Typedef USART3_ReceptionData;
// tableau de pointeurs sur donnée structurée (pour simplifier les fct)
StringReceptionData_Typedef* StrReceptDataTable[3];


// les 3 buffers
char Buffer_USART1[Buf_Len_StrRec_USART1+2];
char Buffer_USART2[Buf_Len_StrRec_USART2+2];
char Buffer_USART3[Buf_Len_StrRec_USART3+2];




//=======================================================================================
// **************************************************************************************
//=======================================================================================
// Fonctions de Configuration USART
//=======================================================================================
// **************************************************************************************
//=======================================================================================


//======================================================================================
char QuelNumUSART(USART_TypeDef *USART)
{
	if (USART==USART1)
	{
		return 0;
	}
  if (USART==USART2)
	{
		return 1;
	}
	if (USART==USART3)
	{
		return 2;
	}
	else
	{
		while(1);
	}
}

//======================================================================================
void USART_Reset_Data(USART_TypeDef *USART)
{
	char N=QuelNumUSART(USART);
	StrReceptDataTable[N]->IndexStr=0;
	StrReceptDataTable[N]->CurrentByte=Null;
	if (N==0)
	{
		StrReceptDataTable[N]->EndCar1=Term_Car_1_USART1;
		StrReceptDataTable[N]->EndCar2=Term_Car_2_USART1;
		StrReceptDataTable[N]->PtrBuffer=Buffer_USART1;
		StrTransmDataTable[N]->StringMaxLen=Max_Car_In_String_Emission_1;
		}
	else if (N==1)
	{
		StrReceptDataTable[N]->EndCar1=Term_Car_1_USART2;
		StrReceptDataTable[N]->EndCar2=Term_Car_2_USART2;
		StrReceptDataTable[N]->PtrBuffer=Buffer_USART2;
		StrTransmDataTable[N]->StringMaxLen=Max_Car_In_String_Emission_2;
	}		
	else 
	{
		StrReceptDataTable[N]->EndCar1=Term_Car_1_USART3;
		StrReceptDataTable[N]->EndCar2=Term_Car_2_USART3;
		StrReceptDataTable[N]->PtrBuffer=Buffer_USART3;
		StrTransmDataTable[N]->StringMaxLen=Max_Car_In_String_Emission_3;
	}	
	StrReceptDataTable[N]->FlagReception=0;
	StrReceptDataTable[N]->FlagOverWrite=0;
	StrReceptDataTable[N]->StringLen=0;
	StrReceptDataTable[N]->ByteCallback=0;
	StrReceptDataTable[N]->EnableByteCallback=0;
	StrReceptDataTable[N]->StrCallback=0;
	StrReceptDataTable[N]->EnableStrCallback=0;
	StrTransmDataTable[N]->StringLen=0;
}


//======================================================================================
void Init_USART(USART_TypeDef *USART,int Baud_Rate_bits_par_Sec,  char Prio_USART)
/*
Active la bonne horloge
Détermine le Baud rate par lecture de la freq USART (Clock de Pascal / Seb)
configure les IO
Initialise les pointeurs de fonctions utiles pour la réception
Valide l'IT USART au niveau NVIC
Positionne les flags au départ
*/

{
unsigned int Frequence_Ck_USART_Hz;	
int USART_Div;
int Mantisse,Fract;

// Table de donnée structurée de réception
StrReceptDataTable[0]=&USART1_ReceptionData;
StrReceptDataTable[1]=&USART2_ReceptionData;
StrReceptDataTable[2]=&USART3_ReceptionData;

// Table de donnée structurée de transmission
StrTransmDataTable[0]=&USART1_TransmData;
StrTransmDataTable[1]=&USART1_TransmData;	
StrTransmDataTable[2]=&USART1_TransmData;
	
//  activation horloge periph + détermination Freq_Periph. + Config IO+ PtrFct_Recep + NVIC_Enable IT :
if (USART==USART1)  
{
	(RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_USART1EN;
	Frequence_Ck_USART_Hz=CLOCK_GetPCLK2();
	// USART_1_TX = TIM1_CH2 = PA9
	// USART_1_RX = TIM1_CH3 = PA10
	GPIO_Configure(GPIOA, 9, OUTPUT, ALT_PPULL);
	GPIO_Configure(GPIOA, 10, INPUT, INPUT_FLOATING);
  NVIC_Enable_IT(37);	
	NVIC_Prio_IT(37, Prio_USART );
	// reset des variables 
	USART_Reset_Data(USART1);
}

if (USART==USART2)
{
  (RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_USART2EN;
	Frequence_Ck_USART_Hz=CLOCK_GetPCLK1();
	// USART_2_TX = TIM2_CH3 = PA2
	// USART_2_RX = TIM2_CH4 = PA3
	GPIO_Configure(GPIOA, 2, OUTPUT, ALT_PPULL);
	GPIO_Configure(GPIOA, 3, INPUT, INPUT_FLOATING);
	NVIC_Enable_IT(38);	
	NVIC_Prio_IT(38, Prio_USART );
	// reset des variables 
	USART_Reset_Data(USART2);
}
if (USART==USART3)  
{
	(RCC->APB1ENR)=(RCC->APB1ENR) | RCC_APB1ENR_USART3EN;
	Frequence_Ck_USART_Hz=CLOCK_GetPCLK1();
	// USART_3_TX = PB10
	// USART_3_RX = PB11
	GPIO_Configure(GPIOB, 10, OUTPUT, ALT_PPULL);
	GPIO_Configure(GPIOB, 11, INPUT, INPUT_FLOATING);
	NVIC_Enable_IT(39);
  NVIC_Prio_IT(39, Prio_USART );	
	// reset des variables 
	USART_Reset_Data(USART3);
}


// Détermination du Baud Rate:
USART_Div=(Frequence_Ck_USART_Hz)/(Baud_Rate_bits_par_Sec); 
//  USART_Div est en format 28.4
Mantisse = USART_Div>>4;
Fract=USART_Div&0x0000000F; 
// on ne garde que les 4 bits fract de poids fort

// configuration émission
USART->CR1=(USART->CR1)|USART_CR1_UE; // UART On
USART->CR2=(USART->CR1)&~(0x1<<12); // 8 bits de data !!!!!!!!!!!! to check
USART->CR2=(USART->CR2)&~(0x3<<12); // 1 bit de stop
USART->BRR=(Mantisse<<4)+Fract; // Baud Rate
USART->CR1=(USART->CR1)|USART_CR1_TE; // Transmit Enable

//Configuration réception
USART->CR1=(USART->CR1)|USART_CR1_RE; // Transmit Enable
USART->CR1=(USART->CR1)|USART_CR1_RXNEIE; // validation IT locale en réception


}

//======================================================================================
void Init_IT_Serial_Receive_Byte(USART_TypeDef *USART, void (*IT_function) (void))
{
	char N= QuelNumUSART(USART);
	StrReceptDataTable[N]->ByteCallback =IT_function;	
	StrReceptDataTable[N]->EnableByteCallback =1;
}

//======================================================================================
void Init_IT_Serial_Receive_Str(USART_TypeDef *USART, void (*IT_function) (void))
{
	char N= QuelNumUSART(USART);
	StrReceptDataTable[N]->StrCallback =IT_function;	
	StrReceptDataTable[N]->EnableStrCallback =1;
}




//=======================================================================================
// **************************************************************************************
//=======================================================================================
// Fonctions d'émission USART
//=======================================================================================
// **************************************************************************************
//=======================================================================================


//======================================================================================
void Put_Char(USART_TypeDef *USART,char Car)
{
	while (((USART->SR)& USART_SR_TXE)==0) // attendre que le buffer soit vide
	{
	}
	USART->DR = Car;

}



//======================================================================================
int Put_String_FixedLen(USART_TypeDef *USART,char * pCar,int Len)
{
	int i;
	i=0;
	char N= QuelNumUSART(USART);
		// // **********  TEST FIN TRANSMISSION  ****************
		while (!(i==Len))
		{
			// émission car en cours (putchar):
		Put_Char(USART,* pCar);
		i++;
		pCar++;
		}
		// fin de transmission : on verrouille la longueur du string reçu
		StrTransmDataTable[N]->StringLen=i;
		return i;		
}


//======================================================================================
int Put_String(USART_TypeDef *USART,char * pCar)
{
	int i;
	i=0;
	char N= QuelNumUSART(USART);
		// // **********  TEST FIN TRANSMISSION  ****************
		while (!((i==Max_Car_In_String_Emission_1)||(*pCar==Null)))
		{
			// émission car en cours (putchar):
		Put_Char(USART,* pCar);
		i++;
		pCar++;
		}
		// fin de transmission : on verrouille la longueur du string reçu
		StrTransmDataTable[N]->StringLen=i;
		return i;	
}


//======================================================================================
int Read_Transmitted_StrLen(USART_TypeDef *USART)
{
	char N= QuelNumUSART(USART);
	return StrTransmDataTable[N]->StringLen;
}





//=======================================================================================
// **************************************************************************************
//=======================================================================================
// Fonctions de Réception USART
//=======================================================================================
// **************************************************************************************
//=======================================================================================

//======================================================================================
char * Get_String(USART_TypeDef *USART)
{
	char N= QuelNumUSART(USART);
	// attente flag
	while(StrReceptDataTable[N]->FlagReception ==0)
	{
	}
	StrReceptDataTable[N]->FlagReception =0;
	return StrReceptDataTable[N]->PtrBuffer;	
}

//======================================================================================
char Is_Str_Received(USART_TypeDef *USART)
{
	char N= QuelNumUSART(USART);
	return StrReceptDataTable[N]->FlagReception;	
}


//======================================================================================
void Flush(USART_TypeDef *USART)
{
	int i;
	char N= QuelNumUSART(USART);
	if (N==0) 
	{	
		for (i=0;i<(Buf_Len_StrRec_USART2+2);i++) Buffer_USART1[i] =Null;
	}
	else if (N==1) 
	{
		for (i=0;i<(Buf_Len_StrRec_USART3+2);i++) Buffer_USART2[i]=Null;
	}
	else 
	{
		for (i=0;i<(Buf_Len_StrRec_USART3+2);i++) Buffer_USART3[i]=Null;
	}
	// Reset variables réception
  StrReceptDataTable[0]->IndexStr=0;
	StrReceptDataTable[0]->FlagReception=0;
	StrReceptDataTable[0]->StringLen=0;

}

//======================================================================================
int Read_Received_StrLen(USART_TypeDef *USART)
{
	char N= QuelNumUSART(USART);
	return StrReceptDataTable[N]->StringLen;
}


//======================================================================================
char Read_Received_Current_Byte(USART_TypeDef *USART)
{
	char N= QuelNumUSART(USART);
	return StrReceptDataTable[N]->CurrentByte;
}


//======================================================================================
char * Read_Received_Current_String(USART_TypeDef *USART)
{
	char N= QuelNumUSART(USART);
	return StrReceptDataTable[N]->PtrBuffer;
}

//======================================================================================
void Write_EndCar1(USART_TypeDef *USART, char EndCar)
{
	char N= QuelNumUSART(USART);
	StrReceptDataTable[N]->EndCar1=EndCar;
}

//======================================================================================
void Write_EndCar2(USART_TypeDef *USART, char EndCar)
{
	char N= QuelNumUSART(USART);
	StrReceptDataTable[N]->EndCar2=EndCar;
}



//=======================================================================================
// **************************************************************************************
//=======================================================================================
// Les fonctions d'IT de l'USART
//=======================================================================================
// **************************************************************************************
//=======================================================================================

//=======================================================================================
// Les fonctions d'IT de l'USART1
//=======================================================================================

void USART1_IRQHandler(void)
{
	if (((USART1->SR)&USART_SR_RXNE)==USART_SR_RXNE)
	{
			// Stockage octet, MAJ cpt
			Buffer_USART1[USART1_ReceptionData.IndexStr]=USART1->DR;
			USART1_ReceptionData.CurrentByte=USART1->DR;
			USART1_ReceptionData.IndexStr++;
		  // test overrun
			if (USART1_ReceptionData.FlagReception ==1)
			{
				USART1_ReceptionData.FlagOverWrite =1;
			}
			USART1_ReceptionData.FlagReception=0;
				
#ifdef Dble_Term_Car_USART1 		
			if (USART1_ReceptionData.IndexStr>1)
			{
#endif
				// **********   TEST FIN RECEPTION  ****************
#ifndef Dble_Term_Car_USART1 // un seul caractère de fin
				if (USART1_ReceptionData.CurrentByte==Term_Car_1_USART1)
				{
					Buffer_USART1[USART1_ReceptionData.IndexStr-1]=Null;
					USART1_ReceptionData.FlagReception=1;
					USART1_ReceptionData.StringLen =USART1_ReceptionData.IndexStr-1;
				}
#else // deux caractères de fin
				if (USART1_ReceptionData.CurrentByte==Term_Car_2_USART1)
				{
					if (Buffer_USART1[USART1_ReceptionData.IndexStr-2]==Term_Car_1_USART1)
					{
						Buffer_USART1[USART1_ReceptionData.IndexStr-1]=Null;
						Buffer_USART1[USART1_ReceptionData.IndexStr-2]=Null;
						USART1_ReceptionData.FlagReception =1;
						USART1_ReceptionData.StringLen=USART1_ReceptionData.IndexStr-2;
					}
				}			
#endif
							
				else if (USART1_ReceptionData.IndexStr==Buf_Len_StrRec_USART1+2) 
				{
					Buffer_USART1[USART1_ReceptionData.IndexStr-1]=Null;
					Buffer_USART1[USART1_ReceptionData.IndexStr-2]=Null;
					USART1_ReceptionData.FlagReception =1;
					USART1_ReceptionData.StringLen=Buf_Len_StrRec_USART1;
				}
				
				// gestion fin str
				if (USART1_ReceptionData.FlagReception==1)
				{
					USART1_ReceptionData.IndexStr=0; // préparer prochain str
					// levée callback si validée	
					if 	(USART1_ReceptionData.EnableStrCallback==1) 
					{
						USART1_ReceptionData.StrCallback() ; // appel callback
					}
				}
#ifdef Dble_Term_Car_USART1				
			}
#endif					
			// Gestion octet
			if 	(USART1_ReceptionData.EnableByteCallback ==1)
					{
						USART1_ReceptionData.ByteCallback() ;
					}	
	}
}

//=======================================================================================
// Les fonctions d'IT de l'USART2
//=======================================================================================

void USART2_IRQHandler(void)
{
	if (((USART2->SR)&USART_SR_RXNE)==USART_SR_RXNE)
	{
			// Stockage octet, MAJ cpt
			Buffer_USART2[USART2_ReceptionData.IndexStr]=USART2->DR;
			USART2_ReceptionData.CurrentByte=USART2->DR;
			USART2_ReceptionData.IndexStr++;
		  // test overrun
			if (USART2_ReceptionData.FlagReception ==1)
			{
				USART2_ReceptionData.FlagOverWrite =1;
			}
			USART2_ReceptionData.FlagReception=0;
				
#ifdef Dble_Term_Car_USART2 		
			if (USART2_ReceptionData.IndexStr>1)
			{
#endif
				// **********   TEST FIN RECEPTION  ****************
#ifndef Dble_Term_Car_USART2 // un seul caractère de fin
				if (USART2_ReceptionData.CurrentByte==Term_Car_1_USART2)
				{
					Buffer_USART2[USART2_ReceptionData.IndexStr-1]=Null;
					USART2_ReceptionData.FlagReception=1;
					USART2_ReceptionData.StringLen =USART2_ReceptionData.IndexStr-1;
				}
#else // deux caractères de fin
				if (USART2_ReceptionData.CurrentByte==Term_Car_2_USART2)
				{
					if (Buffer_USART2[USART2_ReceptionData.IndexStr-2]==Term_Car_1_USART2)
					{
						Buffer_USART2[USART2_ReceptionData.IndexStr-1]=Null;
						Buffer_USART2[USART2_ReceptionData.IndexStr-2]=Null;
						USART2_ReceptionData.FlagReception =1;
						USART2_ReceptionData.StringLen=USART2_ReceptionData.IndexStr-2;
					}
				}			
#endif
							
				else if (USART2_ReceptionData.IndexStr==Buf_Len_StrRec_USART2+2) 
				{
					Buffer_USART2[USART2_ReceptionData.IndexStr-1]=Null;
					Buffer_USART2[USART2_ReceptionData.IndexStr-2]=Null;
					USART2_ReceptionData.FlagReception =1;
					USART2_ReceptionData.StringLen=Buf_Len_StrRec_USART2;
				}
				
				// gestion fin str
				if (USART2_ReceptionData.FlagReception==1)
				{
					USART2_ReceptionData.IndexStr=0; // préparer prochain str
					// levée callback si validée	
					if 	(USART2_ReceptionData.EnableStrCallback==1) 
					{
						USART2_ReceptionData.StrCallback() ; // appel callback
					}
				}
#ifdef Dble_Term_Car_USART2				
			}
#endif					
			// Gestion octet
			if 	(USART2_ReceptionData.EnableByteCallback ==1)
					{
						USART2_ReceptionData.ByteCallback() ;
					}	
	}
}


//=======================================================================================
// Les fonctions d'IT de l'USART3
//=======================================================================================

void USART3_IRQHandler(void)
{
	if (((USART3->SR)&USART_SR_RXNE)==USART_SR_RXNE)
	{
			// Stockage octet, MAJ cpt
			Buffer_USART3[USART3_ReceptionData.IndexStr]=USART3->DR;
			USART3_ReceptionData.CurrentByte=USART3->DR;
			USART3_ReceptionData.IndexStr++;
		  // test overrun
			if (USART3_ReceptionData.FlagReception ==1)
			{
				USART3_ReceptionData.FlagOverWrite =1;
			}
			USART3_ReceptionData.FlagReception=0;
				
#ifdef Dble_Term_Car_USART3 		
			if (USART3_ReceptionData.IndexStr>1)
			{
#endif
				// **********   TEST FIN RECEPTION  ****************
#ifndef Dble_Term_Car_USART3 // un seul caractère de fin
				if (USART3_ReceptionData.CurrentByte==Term_Car_1_USART3)
				{
					Buffer_USART3[USART3_ReceptionData.IndexStr-1]=Null;
					USART3_ReceptionData.FlagReception=1;
					USART3_ReceptionData.StringLen =USART3_ReceptionData.IndexStr-1;
				}
#else // deux caractères de fin
				if (USART3_ReceptionData.CurrentByte==Term_Car_2_USART3)
				{
					if (Buffer_USART3[USART3_ReceptionData.IndexStr-2]==Term_Car_1_USART3)
					{
						Buffer_USART3[USART3_ReceptionData.IndexStr-1]=Null;
						Buffer_USART3[USART3_ReceptionData.IndexStr-2]=Null;
						USART3_ReceptionData.FlagReception =1;
						USART3_ReceptionData.StringLen=USART3_ReceptionData.IndexStr-2;
					}
				}			
#endif
							
				else if (USART3_ReceptionData.IndexStr==Buf_Len_StrRec_USART3+2) 
				{
					Buffer_USART3[USART3_ReceptionData.IndexStr-1]=Null;
					Buffer_USART3[USART3_ReceptionData.IndexStr-2]=Null;
					USART3_ReceptionData.FlagReception =1;
					USART3_ReceptionData.StringLen=Buf_Len_StrRec_USART3;
				}
				
				// gestion fin str
				if (USART3_ReceptionData.FlagReception==1)
				{
					USART3_ReceptionData.IndexStr=0; // préparer prochain str
					// levée callback si validée	
					if 	(USART3_ReceptionData.EnableStrCallback==1) 
					{
						USART3_ReceptionData.StrCallback() ; // appel callback
					}
				}
#ifdef Dble_Term_Car_USART3				
			}
#endif					
			// Gestion octet
			if 	(USART3_ReceptionData.EnableByteCallback ==1)
					{
						USART3_ReceptionData.ByteCallback() ;
					}	
	}
}
