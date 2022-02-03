#include "stm32f10x.h"

//=======================================================================================
// 	Auteur T.R.
//  Juin 2017 , 28/06
//  Processeur : STM32F103RB
//  Logiciel dev : µVision 5.23
//  Fichier de configuration de l'USART
//=======================================================================================


// caractères particuliers (ne pas modifier !!) : utilisable dans le main
#define Null 0
#define ClearScreen 0xC
#define LF 0xA
#define CR 0xD
#define ESC 0x1A




#ifndef _USART_USER_CONF_H__
#define _USART_USER_CONF_H__

//=======================================================================================

//_________ USER DEFINE _________________________________________________________________

// Taille du buffer d'émission (réglable pour chaque USART)
#define Max_Car_In_String_Emission_1 80  
#define Max_Car_In_String_Emission_2 80	
#define Max_Car_In_String_Emission_3 80
// Taille du buffer de réception (réglable pour chaque USART)
#define Buf_Len_StrRec_USART1 80
#define Buf_Len_StrRec_USART2 80	
#define Buf_Len_StrRec_USART3 80 

// Caractère de terminaison, par défaut \r = 0xD :
//#define Dble_Term_Car_USART1
//#define Dble_Term_Car_USART2
//#define Dble_Term_Car_USART3

#define Term_Car_1_USART1 CR
#define Term_Car_2_USART1 LF
#define Term_Car_1_USART2 CR
#define Term_Car_2_USART2 '_'
#define Term_Car_1_USART3 CR
#define Term_Car_2_USART3 '_'






#endif
