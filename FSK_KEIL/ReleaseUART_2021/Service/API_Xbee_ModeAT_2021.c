/*!
 * \brief 	Xbee Commandes
 * \details Bibliotheque pour piloter les XBEE en mode AT
 * \author 	T.R. /S. DI MERCURIO
 * \date   	2021
 * \version 1.0
 * \warning Necessite le package USART_rev2018.h
 */

#include "stm32f10x.h"
#include "USART_rev2021.h"
#include "API_Xbee_ModeAT_2021.h"
#include <string.h>
#include <stdio.h>
#include "FctDiverses.h"

#define USART_Xbee USART2

// chaîne de caractère pour élaborer les cmde AT via sprintf
#define XBEE_STRING_LENGTH 30
char xbee_string[XBEE_STRING_LENGTH];

// Defines utiles 
#define XBEE_ENTER_AT "+++"
#define XBEE_AT_OK "OK"



/* =============================================
				Fonctions de configuration Xbee
 ==============================================*/
 
int Xbee_Init(int channel, int pan_id, int address)
{
	/* Verification des parametres d'entrée */
	if ((channel <0x0B) || (channel >0x1A)) return XBEE_INVALID_CHANNEL;
	if ((pan_id <0) || (pan_id >0xFFFF)) return XBEE_INVALID_PANID;
	if ((address <0) || (address >0xFFFF)) return XBEE_INVALID_ADDRESS;
	
	/* Initialise l'USART2 */
	Init_USART(USART_Xbee,9600, 0);
	
	/* Initialisation du XBEE en mode AT, avec le channel, le pan_id et l'adresse de la carte passés en parametre*/
	/* Entre en mode commande AT */
	Put_String(USART_Xbee,XBEE_ENTER_AT);
	/* Attend la reception du XBEE (OK)*/
	if (strcmp(Get_String(USART_Xbee),XBEE_AT_OK)!=0)
	{
		return XBEE_INIT_FAILURE;
	}
		
	/* Regle le canal d'emission (ATCH) */
	sprintf(xbee_string, "ATCH=%02X\r",channel);
	Put_String(USART_Xbee,xbee_string);
	/* Attend la reception du XBEE (OK)*/
	if (strcmp(Get_String(USART_Xbee),XBEE_AT_OK)!=0)
	{
		return XBEE_INIT_FAILURE;
	}
	
	/* Regle le PAN ID (ATID) */
	sprintf(xbee_string, "ATID=%04X\r",pan_id);
	Put_String(USART_Xbee,xbee_string);
	/* Attend la reception du XBEE (OK)*/
	if (strcmp(Get_String(USART_Xbee),XBEE_AT_OK)!=0)
	{
		return XBEE_INIT_FAILURE;
	}
	

	
	/* Regle l'adresse locale (ATMY=Adresse) */
	sprintf(xbee_string, "ATMY=%04X\r",address);
	Put_String(USART_Xbee,xbee_string);
	/* Attend la reception du XBEE (OK)*/
	if (strcmp(Get_String(USART_Xbee),XBEE_AT_OK)!=0)
	{
		return XBEE_INIT_FAILURE;
	}
	
	/* Regle l'adresse de destination poids fort à 0 (ATDH=0000) */
	Put_String(USART_Xbee,"ATDH=0000\r");
	/* Attend la reception du XBEE (OK)*/
	if (strcmp(Get_String(USART_Xbee),XBEE_AT_OK)!=0)
	{
		return XBEE_INIT_FAILURE;
	}

	/* Sortie du mode AT, passage en mode transparent */
	sprintf(xbee_string, "ATCN\r");
	Put_String(USART_Xbee,xbee_string);
	/* Attend la reception du XBEE (OK)*/
	if (strcmp(Get_String(USART_Xbee),XBEE_AT_OK)!=0)
	{
		return XBEE_INIT_FAILURE;
	}
	
	// le module Xbee est maintenant en mode transparent
	return XBEE_SUCCESS;
}



/* =============================================
				Fonctions d'utilisation Xbee
 ==============================================*/


//======================================================================================
int Xbee_Fix_DestAdress(int addr_destination)
{

	/* Verification des parametres d'entrée */
	if ((addr_destination <0) || (addr_destination >0xFFFF)) return XBEE_INVALID_ADDRESS;
	
	Delay_x_ms(1000); // attendre au cas avant de relancer une cmde AT
										// sinon ça plante
	
	/* Entre en mode commande AT */
	Put_String(USART_Xbee,XBEE_ENTER_AT);
	/* Attend la reception du XBEE (OK)*/
	if (strcmp(Get_String(USART_Xbee),XBEE_AT_OK)!=0)
	{
		return XBEE_INIT_FAILURE;
	}
	

	
	/* Regle l'adresse destinataire (ATDL=Adresse) */
	sprintf(xbee_string, "ATDL=%04X\r",addr_destination);
	Put_String(USART_Xbee,xbee_string);
	/* Attend la reception du XBEE (OK)*/
	if (strcmp(Get_String(USART_Xbee),XBEE_AT_OK)!=0)
	{
		return XBEE_INIT_FAILURE;
	}
	

	
	/* Sortie du mode AT, passage en mode transparent */
	sprintf(xbee_string, "ATCN\r");
	Put_String(USART_Xbee,xbee_string);
	/* Attend la reception du XBEE (OK)*/
	if (strcmp(Get_String(USART_Xbee),XBEE_AT_OK)!=0)
	{
		return XBEE_INIT_FAILURE;
	}
	

	
	// le module Xbee est maintenant en mode transparent
	return XBEE_SUCCESS;
}




//======================================================================================
void Xbee_Send_Str( char *str)
{	
Put_String(USART_Xbee,str);
}


//======================================================================================
char * Xbee_Get_Str(void)
{	
return Get_String(USART_Xbee);
}

//======================================================================================
char Xbee_Is_Str_Received(void)
{
return Is_Str_Received(USART_Xbee);	
}
