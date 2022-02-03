/*!
 * \brief 	Xbee Commandes
 * \details Bibliotheque pour piloter les XBEE en mode AT
 * \author 	T.R. /S. DI MERCURIO
 * \date   	2018
 * \version 1.0
 * \warning Necessite le package USART_rev2018.h
 */

//=======================================================================================
//  Dépendance logicielle : 
//	- USART_rev2018.h (USART_rev2018.c) 
//	- FctDiverses.h (FctDiverses.c) pour les délais soft
//	Ressources matérielle utilisées :
//	- USART2
//=======================================================================================



#ifndef __XBEE_CMD_H__
#define __XBEE_CMD_H__
 
	 
/**
 * Liste des codes retour possibles pour les fonctions
 * NB : tous les items ne sont pas utilisés
 */
 
enum XBEE_STATUS {
	XBEE_SUCCESS=		  	0, /*!< L'operation s'est déroulée sans probleme */
	XBEE_INVALID_CHANNEL= 	1, /*!< La valeur de canal ne fait pas partie des valeurs autorisées (0x0B - 0x1C) */
	XBEE_INVALID_PANID= 	2, /*!< La valeur de pan id ne fait pas partie des valeurs autorisées (0x0000 - 0xFFFF) */
	XBEE_INVALID_ADDRESS=	3, /*!< La valeur d'adresse source ne fait pas partie des valeurs autorisées (0x0000 - 0xFFFF) */
	XBEE_TIMEOUT_EXPIRED= 	4, /*!< L'operation a pris plus de temps que la durée maximum (timeout) \see XBEE_TIMEOUT \see xbee_set_timeout */
	XBEE_INVALID_FRAME=  	5, /*!< Les données recues sont invalides */
	XBEE_INIT_FAILURE=		6, /*!< L'initalisation du xbee a echouée \see xbee_init */
	XBEE_BUSY=				7, /*!< Le xbee est déjà occupée */
	XBEE_SEND_FAILED=		8  /*!< L'envoi des données a échouée */
};








/* =============================================
				Initialisation Xbee
 ============================================= */
/**
 * Initialise le module XBEE (en mode AT)
 * @param channel Le canal d'emission (0x0B-0x1D).
 * @param pan_id Le pan identifier du reseau (0x0000 - 0xFFFF).
 * @param address L'adresse propre (source) du module (0x0000 - 0xFFFE).
 * @return Resultat de l'init.
 * @see XBEE_STATUS
 * Exemple : Xbee_Init(0xD, 0x4444, 0xB);
 */ 
int Xbee_Init(int channel, int pan_id, int address);


/**
 * Fixe l'adresse destination pour le prochain envoi de donnée (en mode AT)
 * NB : la fonction dure une seconde
 * @param addr_destination (0x0000 - 0xFFFF).
 * @see XBEE_STATUS
 * Exemple : Xbee_Fix_DestAdress(0xA);
 */  
 
int Xbee_Fix_DestAdress(int addr_destination);



/* =============================================
				Emission / réception Xbee
 ============================================= */

/**
 * Envoie une chaine de caracteres a un module xbee (mode transparent)
 * @param str Pointeur sur une chaine de caractere.
 * Exemple : char * Ptr_Mssg;
 * 					 Xbee_Send_Str(Ptr_Mssg); 
 */ 
void Xbee_Send_Str( char *str);



/**
 * Attend qu'un string soit reçu depuis le Xbee puis 
 * recupere la chaine de caractere envoyée par le module xbee
 * Attention : La fonction est bloquante
 * @return Pointeur sur le buffer de réception de l'USART associée
 */ 
char * Xbee_Get_Str(void);


/**
 * Renvoie 1 si un String est reçu depuis le Xbee.
 * la fonction est non bloquante et permet d'éviter l'attente de la fonction Xbee_Get_Str
 * @return Pointeur sur le buffer de réception de l'USART associée
 * Exemple : 
 		if (Xbee_Is_Str_Received()==1)
		{
			Reponse=Xbee_Get_Str(); // jamais bloquant puisque un string est reçu !
		}
 */ 
char Xbee_Is_Str_Received(void);
 
#endif /* __XBEE_CMD_H__ */
