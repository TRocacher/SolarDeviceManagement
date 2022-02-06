
#include "clock.h"
#include "API_ADC.h"
#include "Timer_1234.h"
#include "USART_rev2021.h"
#include "FctDiverses.h"
#include "API_FSK.h"
#include "stdio.h"
#include "GPIO.h"
#include "MyLCD.h"
#include "NVIC_IT_Ext.h"

char* Reponse;  						// déclaration d'un pointeur de caractère 
 // réponse contient l'adresse du buffer qui contient
														// la chaîne de caractères
char pipo;
#define HeaderCar '#'
#define HeaderCarLen 5  // !!! impair !!!!!


int LongRep;
int HeaderCar_Count; // nbre de HeaderCar trouvé
int Indice_HeaderCar[HeaderCarLen];  // indice dans la réponse où se trouve le dernier HeaderCar trouvé
int ResultatParsing;

#define HeaderCar '#'
#define HeaderCarLen 5  // !!! impair !!!!!

int ReponseParsing(void);
void IT_CarUSART3(void);
void IT_CarrierDetect(void);

int main (void)
{
  CLOCK_Configure();
	Init_USART(USART1,38400, 0); // uart pour check xctu
	
	MyLCD_Init();
	MyLCD_Clear();
	MyLCD_Set_cursor(0, 0);
	MyLCD_Print("Recepteur...");
	MyLCD_Set_cursor(0, 1);	
  FSK_Init();
	
	// IT externe pour Carrier detect
	NVIC_Ext_IT (GPIOB, 7, FALLING_RISING_EDGE, INPUT_FLOATING,1,IT_CarrierDetect);
	
	
	
	Init_IT_Serial_Receive_Byte(USART3, IT_CarUSART3);

	// RxCmde = 1
	Port_IO_Set(GPIOB,8);

	
while(1)
	{
//  if (GPIO_Read(GPIOB,7)==0)
//		USART3->CR1=(USART3->CR1)|USART_CR1_RXNEIE; // validation IT locale en réception
//	else
//		USART3->CR1=(USART3->CR1)&~USART_CR1_RXNEIE; // blocage IT locale en réception
	
	if (Is_Str_Received(USART3)==1)
		{
			Reponse=Get_String(USART3); // jamais bloquant puisque un string est reçu !
			LongRep=Read_Received_StrLen(USART3);
			
			ResultatParsing=ReponseParsing();
			
			if (ResultatParsing!=-1)
			{
			Reponse=Reponse+ResultatParsing;
			MyLCD_ClearLineDown();
			MyLCD_Set_cursor(0,1);
			MyLCD_Print(Reponse);			
		  
			}
      Flush(USART3);
		}
	}	
}



int ReponseParsing(void)
{
	int Sum;
	int Moy;
	int iloop;
	int ReturnVal;
	char* Rep;
	
	// parsing réponse
	// forme de la châine : "xzeczcvAAAAMsg" : il faut détecter les AAAAA puis les supprimer et mettre à jour
	// le pointeur de châine initialement en première position.
	// Si AAAA n'est pas trouvé, abandon, retour "ùùùùErreur réception try again"
	HeaderCar_Count=0;
	Rep=Reponse;
	for (iloop=0;iloop<HeaderCarLen;iloop++)	Indice_HeaderCar[iloop]=0;
	Sum=0;
	ReturnVal=-1;
	
	for (iloop=0;iloop<LongRep;iloop++)
	{
		if (*Rep==HeaderCar)
		{
			Indice_HeaderCar[HeaderCar_Count]=iloop; 
			HeaderCar_Count++;
			if (HeaderCar_Count==HeaderCarLen) break;
					
		}
		Rep++; // incrémentation pointeur
	}
	if (HeaderCar_Count==HeaderCarLen) // 5 HeaderCar ont été trouvé
	{
		// calcul de la moyenne pour voir si les indices se suivent (principe du rami sur les suites)
		for (iloop=0;iloop<HeaderCarLen;iloop++)	Sum=Sum+Indice_HeaderCar[iloop];
		Moy=Sum/HeaderCarLen;
		if (Moy==Indice_HeaderCar[(HeaderCarLen-1)/2]) // la moyenne vaut l'élément du milieu
			// exemple : HeaderCarLen=5, et Indice_HeaderCar = {10 11 12 13 14}, Moy = 12 à comparer avec le 12 rang 3
		{
			ReturnVal=Indice_HeaderCar[HeaderCarLen-1] +1;
		}
		
		
	}		
	
	return ReturnVal;
}


void IT_CarUSART3(void)
{
	char lettre;

	
	lettre=Read_Received_Current_Byte(USART3);
	Put_Char(USART1,lettre); 
	
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
