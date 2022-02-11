// v.a : polling flage APRES emission et non avant pour �mettre l'avant dernier octet sans pb sur un print
// Egalement dans le print, on teste TC (transmission complete pour �tre s�r que tout est parti

// v.b : UART3 compl�tement revue (pour FSK)
// la trame re�ue a la forme suivante:
// 0xFF 0xFF 0xFF 0xFF "#####"yyxxxxxxxxxxxxx" 
// 0xFF pour synchroniser l'UART
// "#####" Header HeaderNb =5 ici
// "xxxxxxxxxx" Data 
// "yy" 2 octets CRC
// MessgLen = 2 + len (data), 
//  Variable Messg[MessgLen] 

#include "stm32f10x.h"
#include "Clock.h"
#include "GPIO.h"
#include "Timer_1234.h"
#include "NVIC_IT_Ext.h"
#include "USART_rev2021_b.h"
#include "USART_User_Conf_2021.h"


// 13/10/2021 : ajout de la fonction de lecture du flag 
// Is_StringReceived(USART_TypeDef *USART)
// pour permettre une lecture non bloquante

//=======================================================================================
// 	Auteur T.R.
//  Sept 2018
//  Processeur : STM32F103RB
//  Logiciel dev : �Vision 5.23
//  Valider Simu 3USART s�par�ment, avec USART_Rev3b.h et USART_User_Conf_3.h
//  
//=======================================================================================

#define HeaderCar '#'
#define HeaderCarLen 5  
#define MessgLen (1+2+16) //  |Header Car|CRCh|CRLCl| data|  (le dernier HeaderCar est compt� ds la cha�ne...
int HeaderCarOccur;
char Messg[MessgLen]; 
												

int IndexMessg;
char DataUART;
char HeaderOK;




StatusRecept Status;
int CRC_Val, Sum;
int i;


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
// Donn�es structur�es pour l'�mission et la r�ception 
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
// tableau de pointeur sur donn�e structur�e (pour simplifier les fct)
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
C'est un pointeur de caract�re. Il contient l'adresse du premier caract�re re�u.
Le simulateur est capable d'afficher non seulement le premier caract�re, 
mais tous les autres ! On peut donc visualiser en temps r�el la cha�ne de caract�re
en cours de r�ception. Tr�s pratique pour d�bugger !

int IndexStr
C'est l'index qui parcourt la cha�ne de caract�re au fur et � mesure de la r�cpetion
d'octet.

char CurrentByte
C'est le dernier octet re�u dans la cha�ne.

char EndCar1
C'est la valeur du premier caract�re de fin. A l'initialisation, 
il est �gal �  Term_Car_1_USARTx (x vallant 1,2 ou3), voir 3.1

char EndCar2
Idem pour le second caract�re 

char FlagReception
Indicateur positionn� � '1'  lorsque :
- le ou les deux caract�re(s) de fin est (sont) trouv�(s), voir 3.1
- le nombre maximum autoris� de caract�re est atteint
Remis � 0 lorsque :
- la lecture du string est faite

char FlagOverWrite 
Cet indicateur passe � '1' si un string est en cours de r�ception  
(buffer en cours de remplissage) et que le string pr�c�dent n'a pas �t� lu.

int  StringLen
C'est la longueur de la cha�ne re�ue (Null non compt� bien s�r)

char EnableByteCallback
Ce param�tre  de validation indique que l'utilisateur a souhait� d�clencher
un callback � la r�ception d'un octet.

void (* ByteCallback)(void)
Adresse de la fonction callback � appeler � la r�ception d'un octet

char EnableStrCallback;
Ce param�tre  de validation indique que l'utilisateur a souhait� d�clencher 
un callback � la r�ception d'un string.

void (* StrCallback)(void)
Adresse de la fonction callback � appeler � la r�ception d'un string

*/



StringReceptionData_Typedef USART1_ReceptionData;
StringReceptionData_Typedef USART2_ReceptionData;
StringReceptionData_Typedef USART3_ReceptionData;
// tableau de pointeurs sur donn�e structur�e (pour simplifier les fct)
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
int TimeOut_BitRate;
void MyTimeOut_IT(void)
{
	TimeOut_BitRate++;
	if ( TimeOut_BitRate==MessgLen+2)
		{
			Status = TimeOut;
			// stop timer
			Bloque_Timer(TIM4);
		}	
}

void Init_USART(USART_TypeDef *USART,int Baud_Rate_bits_par_Sec,  char Prio_USART)
/*
Active la bonne horloge
D�termine le Baud rate par lecture de la freq USART (Clock de Pascal / Seb)
configure les IO
Initialise les pointeurs de fonctions utiles pour la r�ception
Valide l'IT USART au niveau NVIC
Positionne les flags au d�part
*/

{
unsigned int Frequence_Ck_USART_Hz;	
int USART_Div;
int Mantisse,Fract;

float duree_us;	
	
// Table de donn�e structur�e de r�ception
StrReceptDataTable[0]=&USART1_ReceptionData;
StrReceptDataTable[1]=&USART2_ReceptionData;
StrReceptDataTable[2]=&USART3_ReceptionData;

// Table de donn�e structur�e de transmission
StrTransmDataTable[0]=&USART1_TransmData;
StrTransmDataTable[1]=&USART1_TransmData;	
StrTransmDataTable[2]=&USART1_TransmData;
	
//  activation horloge periph + d�termination Freq_Periph. + Config IO+ PtrFct_Recep + NVIC_Enable IT :
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
	
		// *****************************
	//  PERIPH GESITON TIMEOUT, TIM4
	// *****************************
	// TIM4 r�gl� � la p�riode Baudrate
	duree_us=1.0/(float)Baud_Rate_bits_par_Sec;
	Timer_1234_Init(TIM4,duree_us);
	Bloque_Timer(TIM4);
	Active_IT_Debordement_Timer( TIM4, 0, MyTimeOut_IT);
	
	
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


// D�termination du Baud Rate:
USART_Div=(Frequence_Ck_USART_Hz)/(Baud_Rate_bits_par_Sec); 
//  USART_Div est en format 28.4
Mantisse = USART_Div>>4;
Fract=USART_Div&0x0000000F; 
// on ne garde que les 4 bits fract de poids fort

// configuration �mission
USART->CR1=(USART->CR1)|USART_CR1_UE; // UART On
USART->CR2=(USART->CR1)&~(0x1<<12); // 8 bits de data !!!!!!!!!!!! to check
USART->CR2=(USART->CR2)&~(0x3<<12); // 1 bit de stop
USART->BRR=(Mantisse<<4)+Fract; // Baud Rate
USART->CR1=(USART->CR1)|USART_CR1_TE; // Transmit Enable

//Configuration r�ception
USART->CR1=(USART->CR1)|USART_CR1_RE; // Transmit Enable
USART->CR1=(USART->CR1)|USART_CR1_RXNEIE; // validation IT locale en r�ception


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
// Fonctions d'�mission USART
//=======================================================================================
// **************************************************************************************
//=======================================================================================


//======================================================================================
void Put_Char(USART_TypeDef *USART,char Car)
{

	USART->DR = Car;
	while (((USART->SR)& USART_SR_TXE)==0) // attendre que le buffer soit vide � la fin pour �tre certain d'envoyer last
	{
	}
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
			// �mission car en cours (putchar):
		Put_Char(USART,* pCar);
		i++;
		pCar++;
		}
		// fin de transmission : on verrouille la longueur du string re�u
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
			// �mission car en cours (putchar):
		Put_Char(USART,* pCar);
		i++;
		pCar++;
		}
		// attente dernier octet parti
		while (((USART->SR)& USART_SR_TC)==0)
		{
		}			
			
		// fin de transmission : on verrouille la longueur du string re�u
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
// Fonctions de R�ception USART
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
	// Reset variables r�ception
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

//void USART1_IRQHandler(void)
//{
//	if (((USART1->SR)&USART_SR_RXNE)==USART_SR_RXNE)
//	{
//			// Stockage octet, MAJ cpt
//			Buffer_USART1[USART1_ReceptionData.IndexStr]=USART1->DR;
//			USART1_ReceptionData.CurrentByte=USART1->DR;
//			USART1_ReceptionData.IndexStr++;
//		  // test overrun
//			if (USART1_ReceptionData.FlagReception ==1)
//			{
//				USART1_ReceptionData.FlagOverWrite =1;
//			}
//			USART1_ReceptionData.FlagReception=0;
//				
//#ifdef Dble_Term_Car_USART1 		
//			if (USART1_ReceptionData.IndexStr>1)
//			{
//#endif
//				// **********   TEST FIN RECEPTION  ****************
//#ifndef Dble_Term_Car_USART1 // un seul caract�re de fin
//				if (USART1_ReceptionData.CurrentByte==Term_Car_1_USART1)
//				{
//					Buffer_USART1[USART1_ReceptionData.IndexStr-1]=Null;
//					USART1_ReceptionData.FlagReception=1;
//					USART1_ReceptionData.StringLen =USART1_ReceptionData.IndexStr-1;
//				}
//#else // deux caract�res de fin
//				if (USART1_ReceptionData.CurrentByte==Term_Car_2_USART1)
//				{
//					if (Buffer_USART1[USART1_ReceptionData.IndexStr-2]==Term_Car_1_USART1)
//					{
//						Buffer_USART1[USART1_ReceptionData.IndexStr-1]=Null;
//						Buffer_USART1[USART1_ReceptionData.IndexStr-2]=Null;
//						USART1_ReceptionData.FlagReception =1;
//						USART1_ReceptionData.StringLen=USART1_ReceptionData.IndexStr-2;
//					}
//				}			
//#endif
//							
//				else if (USART1_ReceptionData.IndexStr==Buf_Len_StrRec_USART1+2) 
//				{
//					Buffer_USART1[USART1_ReceptionData.IndexStr-1]=Null;
//					Buffer_USART1[USART1_ReceptionData.IndexStr-2]=Null;
//					USART1_ReceptionData.FlagReception =1;
//					USART1_ReceptionData.StringLen=Buf_Len_StrRec_USART1;
//				}
//				
//				// gestion fin str
//				if (USART1_ReceptionData.FlagReception==1)
//				{
//					USART1_ReceptionData.IndexStr=0; // pr�parer prochain str
//					// lev�e callback si valid�e	
//					if 	(USART1_ReceptionData.EnableStrCallback==1) 
//					{
//						USART1_ReceptionData.StrCallback() ; // appel callback
//					}
//				}
//#ifdef Dble_Term_Car_USART1				
//			}
//#endif					
//			// Gestion octet
//			if 	(USART1_ReceptionData.EnableByteCallback ==1)
//					{
//						USART1_ReceptionData.ByteCallback() ;
//					}	
//	}
//}

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
#ifndef Dble_Term_Car_USART2 // un seul caract�re de fin
				if (USART2_ReceptionData.CurrentByte==Term_Car_1_USART2)
				{
					Buffer_USART2[USART2_ReceptionData.IndexStr-1]=Null;
					USART2_ReceptionData.FlagReception=1;
					USART2_ReceptionData.StringLen =USART2_ReceptionData.IndexStr-1;
				}
#else // deux caract�res de fin
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
					USART2_ReceptionData.IndexStr=0; // pr�parer prochain str
					// lev�e callback si valid�e	
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

// v.b : UART3 compl�tement revue (pour FSK)

/* PRINCIPE
- d�tection du header
- D�marrage Timer TimeOut en fonction du BdRate et du nbre de caract�re attendu
- r�cup�ration CRC
- stockage du Messge avec # en t�te
- mise � jour Status = OK / TimeOut / WrongCRC / NoReception


Dans le d�tail
Si on d�tecte HeaderLenMin fois le caract�re d'ent�te et si HeaderOK=0
	HeaderOK=1
	IndexMessg=0;
	Start TimeOut
Fin si

Si HeaderOK = 0
	Statue = NoReception
FinSi

Si HeaderOK 
	Echantillonnage des MessgLen Caract�res y compris CRC.
FinSi

Si tous les caract�res OK
	Si checkSum OK
		Status = OK
		Reset TimeOut
		HeaderOK=0
		HeaderCarOccur=0
	Sinon
		Status = WrongCRC
		Reset TimeOut
		HeaderOK=0
		HeaderCarOccur=0;
	Sinon
Fin Si

Si Timeout
	status = TimeOut
	Reset TimeOut
	HeaderOK=0
	HeaderCarOccur=0;
Fin Si


Fonction GetStatus : renvoir l'�tat
Fonction ResetStatus : remet le status � NoReception





Utilisation :
while (1)
si GetStatus != NoReception
	si OK, si Timeout etc...
  ...
  ...
  ResetStatus(); // pour lib�rer le prochain processus
fin si

*/




void ResetStatus(void)
{
			HeaderOK=0;
			IndexMessg=0;     // on pr�pare le tableau de r�ception Mssg
			HeaderCarOccur=0; // pour la prochaine fois
			TimeOut_BitRate=0; // reset timeout et lancement timer
	    Bloque_Timer(TIM4);
			for (i=0;i<MessgLen;i++)
			{
				Messg[i]=0;
			}
			Status=NoReception;
	    
}

StatusRecept GetStatus(void)
{
	return Status;
}

char * GetStringAddres (void)
{
	return (Messg+2);
}



void USART1_IRQHandler(void)
{
	if (((USART1->SR)&USART_SR_RXNE)==USART_SR_RXNE)
	{
		
//void USART3_IRQHandler(void)
//{
//	if (((USART3->SR)&USART_SR_RXNE)==USART_SR_RXNE)
//	{
	DataUART=USART1->DR;
		
  // ******************			
	//d�tection du header
  // ******************		
	if ((DataUART==HeaderCar) && (HeaderOK==0))
	{
		HeaderCarOccur++;
	  if (HeaderCarOccur==HeaderCarLen) 
		{
			HeaderOK=1;
			IndexMessg=0;     // on pr�pare le tableau de r�ception Mssg
			HeaderCarOccur=0; // pour la prochaine fois
			TimeOut_BitRate=0; // reset timeout et lancement timer
			/////////////////////////////////////////////////////////////Run_Timer(TIM4);
		}
	}
	else HeaderCarOccur=0; // si ##?## HeaderCarOccur=12012 et non 12234.
	
	


	if (HeaderOK == 0) Status = NoReception;
	
		
	else
	// ******************			
	// Header d�tect�
  // ******************		
	{
		// ******************			
	  // Stockage data
		// *****************
		Messg[IndexMessg]=DataUART;
		IndexMessg++;
		

		if ((IndexMessg==MessgLen) && (Status!=TimeOut)) // tous les caract�res on �t� lus (index hors table ici)
																										 // si un timeOut a �t� lev�, on ne modifie pas le status
																										 // il reste TimeOut
		// **********************************************			
	  // toute la ch�ine est acquise (aucun octet perdu)
		// ***********************************************	
		{
			

			// Reset TimeOut and stop timer
			Bloque_Timer(TIM4);
			TimeOut_BitRate=0;
			
			
			// ***********
	    // Check sum calcul
		  // ***********
			CRC_Val=Messg[2]+Messg[1]*256;
			Sum=0;
			for (i=3;i<MessgLen;i++) // on commence � i = 3 pour �viter 0 et 1 qui contiennent le CRC et 2 qui contient #
			{
				Sum=Sum+Messg[i];
			}
			
			//	Si checkSum OK
			if (CRC_Val==Sum)
			// ***********
	    // Check sum OK
		  // ***********
			{
				Status=OK;				
			}
			else
			// ***********
	    // Check sum KO
		  // ***********
			{
				Status = WrongCRC;
			}
		}

		if (IndexMessg==MessgLen) IndexMessg=0; // �viter � tout prix un effet de bord!
	}
	



	}
	

}
