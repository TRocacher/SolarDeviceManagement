#include "stm32f10x.h"
#include "USART_User_Conf_2021.h"


// 13/10/2021 : ajout de la fonction de lecture du flag 
// Is_StringReceived(USART_TypeDef *USART)
// pour permettre une lecture non bloquante

// 08/2018 : Documentation_USART_Driver.pdf pour une information précise



//=======================================================================================
// 	Auteur T.R.
//  Juin 2017 , 28/06
//  Processeur : STM32F103RB
//  Logiciel dev : µVision 5.23
// 
//=======================================================================================


#ifndef _USART_rev3_H__
#define _USART_rev3_H__


/* 
 ======================================================================================
|
|								GENERALITE
|   Compléter le fichier USART_User_Conf.h pour paramétrer la COM
|   taille des buffer, caractères spéciaux...
|
 ======================================================================================
 
La lib fournit les fonctionnalités de bas niveau de l'UART mais aussi des fonctions
de plus haut niveau qui permettent la manipulation de chaîne de caractère.

* les 3 USART sont programmables
* les IO sont configurées dans la fonction d'initialisation de l'USART

=> USART_1_TX = TIM1_CH2 = PA9
=> USART_1_RX = TIM1_CH3 = PA10
=> USART_2_TX = TIM2_CH3 = PA2
=> USART_2_RX = TIM2_CH4 = PA3
=> USART_3_TX = PB10
=> USART_3_RX = PB11
 
Le débit de chacune des transmission est paramétrable.
Par contre les autres configurations sont figées :
- pas de parité
- 8 bits
- pas de contrôle de flux matériel
- 1 bit de stop
 
 
*/


//=======================================================================================
//                      Configuration  
//=======================================================================================

void Init_USART(USART_TypeDef *USART,int Baud_Rate_bits_par_Sec, char Prio_USART); // OK
/*___________________________________________________________________________________________

PARAM:
USART : adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3
Baud_Rate_bits_par_Sec: Débit de symboles en bits / s (normalement en Bauds)
Prio_USART: Priorité de l'interruption USART lors de la réception. On conseille de choisir 0 pour avoir la priorité maximale.

DESCRIPTION :
Initialise l'USART voulue au débit souhaité.

EXEMPLE : 
Init_USART(USART2,115200,0);	
___________________________________________________________________________________________*/


void Init_IT_Serial_Receive_Byte(USART_TypeDef *USART, void (*IT_function) (void));
void Init_IT_Serial_Receive_Str(USART_TypeDef *USART, void (*IT_function) (void));
/*___________________________________________________________________________________________
PARAM:
USART : adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3
IT_function: Adresse (nom) de la fonction callback à lancer

DESCRIPTION :
Mémorise le nom de la fonction callback à lancer (à la réception d'un octet ou s'un string). Initialise 
le mécanisme d'appel à la fonction callback.

EXEMPLE : 
Init_IT_Serial_Receive_Str(USART2, MaFonction);
___________________________________________________________________________________________*/




//=======================================================================================
//                      Réception
//=======================================================================================


char * Get_String(USART_TypeDef *USART); 
/*___________________________________________________________________________________________
PARAM:
USART : adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3

DESCRIPTION :
Attend la réception complète d'un string sur l'USART considérée

EXEMPLE : 
char* Reponse ;  						// déclaration d'un pointeur de caractère 
Reponse=Get_String(USART2); // réponse contient l'adresse du buffer qui contient
														// la chaîne de caractères
___________________________________________________________________________________________*/


char Is_Str_Received(USART_TypeDef *USART);
/*___________________________________________________________________________________________
PARAM:
USART : adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3

DESCRIPTION :
Renvoie l'état (0 ou 1) du flag qui signale qu'un String est arrivé.

EXEMPLE : 
		if (Is_Str_Received(USART1)==1)
		{
			Reponse=Get_String(USART1); // jamais bloquant puisque un string est reçu !
		}
___________________________________________________________________________________________*/


void Flush(USART_TypeDef *USART);
/*___________________________________________________________________________________________
PARAM:
USART : adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3

DESCRIPTION :
vide le buffer (écrit Null sur toute la longueur du buffer)

EXEMPLE : 
Flush(USART3) ;
___________________________________________________________________________________________*/





//=======================================================================================
//                      Emission
//=======================================================================================


void Put_Char(USART_TypeDef *USART,char Car); 
/*___________________________________________________________________________________________
PARAM:
USART : adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3
Car : le caractère à transmette

DESCRIPTION :
Emet juste un caractère sur l'USART donnée

EXEMPLE : 
Put_Char(USART1,'c');
___________________________________________________________________________________________*/


int Put_String(USART_TypeDef *USART,char * pCar);
/*___________________________________________________________________________________________
PARAM:
USART : adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3
* pCar : adresse du premier caractère de la chaîne à transmettre

DESCRIPTION :
La fonction envoie toute une chaîne de caractères dont l'adresse de début est donnée.
S'arrête quand le caractère Null est trouvé, ou si la longueur maximale est atteinte.
(cf "USART_User_Conf_2018.h")

EXEMPLE : 
char* Reponse ; 
sprintf(Reponse,  « coucou »);
Put_String(USART2,Reponse);
//ou directement :
Put_String(USART2,  « coucou » );
___________________________________________________________________________________________*/


int Put_String_FixedLen(USART_TypeDef *USART,char * pCar,int Len);

/*___________________________________________________________________________________________

PARAM:
USART : adresse de l'USART en question. Choisir parmi USART1, USART2 ou USART3
* pCar : adresse du premier caractère de la chaîne à transmettre
Len : longueur de la chaîne à passer

DESCRIPTION :
Cette fonction permet d'envoyer une suite d'octets quelconque, y compris le caractère Null.
Elle ne peut donc pas fonctionner sur le principe de la détection de Null.
C'est pourquoi la longueur est passée en paramètre. La fonction va envoyer un nombre de
caractère égal à Len, à partir de l'adresse donnée dans pcar.

EXEMPLE : 
char* Reponse ; 
Put_String(USART2,Reponse,15); // émet 15 caractères à partir de l'adresse contenue dans Reponse
__________________________________________________________________________________________*/








//=======================================================================================
//                      Getter et Setters
//=======================================================================================

int Read_Received_StrLen(USART_TypeDef *USART);
char Read_Received_Current_Byte(USART_TypeDef *USART);
char *  Read_Received_Current_String(USART_TypeDef *USART);
void Write_EndCar1(USART_TypeDef *USART, char EndCar); // permet de modifier par pgm les endCar
void Write_EndCar2(USART_TypeDef *USART, char EndCar);
int Read_Transmitted_StrLen(USART_TypeDef *USART);
/*___________________________________________________________________________________________
Fonction de lecture / écriture  à la volée des données structurée
@ USART_TypeDef *USART = USART, USART2 ou USART3
Les fonctions qui suivent permettent des accés en lecture et écriture de certains champs 
des variables structurées définies dans le .c. 
Ces fonctions sont à utiliser avec précaution.
___________________________________________________________________________________________*/









#endif
