
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
#define HeaderCarLenMax 5  // !!! impair !!!!!
#define HeaderCarLenMin 3

int LongRep;
int HeaderCar_Count; // nbre de HeaderCar trouvé
int Indice_HeaderCar[HeaderCarLenMax];  // indice dans la réponse où se trouve le dernier HeaderCar trouvé
int ResultatParsing;


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
	int Moy,MoyArith;
	int iloop;
	int ReturnVal;
	char* Rep;
	
	// parsing réponse
	// forme de la châine : "xzeczcvAAAAMsg" : il faut détecter les AAAAA puis les supprimer et mettre à jour
	// le pointeur de châine initialement en première position.
	// Si AAAA n'est pas trouvé, abandon, retour "ùùùùErreur réception try again"
	HeaderCar_Count=0;
	Rep=Reponse;
	for (iloop=0;iloop<HeaderCarLenMax;iloop++)	Indice_HeaderCar[iloop]=0;
	Sum=0.0;
	ReturnVal=-1;
	
	for (iloop=0;iloop<LongRep;iloop++)
	{
		if (*Rep==HeaderCar)
		{
			Indice_HeaderCar[HeaderCar_Count]=iloop; 
			HeaderCar_Count++;
			if (HeaderCar_Count==HeaderCarLenMax) break;
					
		}
		Rep++; // incrémentation pointeur
	}
	if (HeaderCar_Count>=HeaderCarLenMin) // au moins 3 HeaderCar ont été trouvés
	{
		// calcul de la moyenne pour voir si les indices se suivent (principe du rami sur les suites)
		
		// 3 indices qui se suivent : 2,3,4 => moy = 3 (élts milieu)
		// 4											 : 2,3,4,5 => moy = 3.5 (moyenne des elts du centre, fract)
		// 5											 : 2,3,4,5,6 => moy = 4 (élts milieu)
		
		// Pour rester en int, on va doubler les indices :
		// 3 indices qui se suivent : 4,6,8 => moy = 6 (élts milieu)
		// 4 												: 4,6,8,10 => moy = 7 (moyenne des elts du centre, fract)
		// 5											 : 4,6,8,10,12 => moy = 8 (élts milieu)
		
		if (HeaderCar_Count==3)
		{
			pipo='3';
		}
		if (HeaderCar_Count==4)
		{
			pipo='4';
		}
		if (HeaderCar_Count==5)
		{
			pipo='5';
		}
		
		
		// doublage coeff
		for (iloop=0;iloop<HeaderCar_Count;iloop++)	Indice_HeaderCar[iloop]=Indice_HeaderCar[iloop]<<1;
		// calcul moyenne trivial
		for (iloop=0;iloop<HeaderCar_Count;iloop++) Sum=Sum+Indice_HeaderCar[iloop];
		Moy=Sum/(float)HeaderCar_Count;
		
		// calcul par suite arith
		MoyArith=(Indice_HeaderCar[0]+Indice_HeaderCar[HeaderCar_Count-1])/2;
		
    // égalité moyenne triviale et moyenne arithmétique ?
		if	(Moy==MoyArith)
		{
			ReturnVal=Indice_HeaderCar[HeaderCar_Count-1]/2 +1;
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
