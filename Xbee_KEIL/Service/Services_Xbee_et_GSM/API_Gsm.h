/*!
 * \file
 * \brief 	Bibliotheque de fonctions pour initialiser, envoyer et recevoir de données vers/de un module GSM.
 * \author 	G.A. / T.R.
 * \date   	octobre 2017, revu 2018
 * \version 1
 */

//=======================================================================================
//  Dépendance logicielle : 
//	- USART_rev2018.h (USART_rev2018.c) 
//	- FctDiverses.h (FctDiverses.c) pour les délais soft
//	Ressources matérielle utilisées :
//	- USART1
//=======================================================================================




#ifndef  __API_GSM_2018_H__
#define __API_GSM_2018_H__



/* =============================================
				Fonctions de configuration Xbee
 ==============================================*/
 
 /**
 * Règle l'USART 1 à 115200 bds
 * Donne la priorité 0 (la plus grande) à l'USART
 * Valide la carte SIM. Nécessite 6 secondes environ. Si une erreur s'est produite,
 * le programme plante dans une boucle sans fin. Si cela se produit, vérifier bien
 * le code PIN (3 éssais max !).
 * L'exécution prends en tout 12 secondes (le temps que le MODEM réponde au cmde AT
 *
 * Réglage du MODEM GSM dans le détail :
 * Valide la carte SIM (AT+CPIN) 																: 6 sec
 * SMS en mode texte	(AT+CMGF=1)																: 2 sec
 * SMS transmis/reçu en mémoire Modem (AT+CPMS=ME,ME,ME)				: 2 sec
 * Réception SMS directe sans notification (AT+CNMI=2,2,0,0,0)	: 2 sec
 * @param pin : code PIN de la carte SIM
 * 
 * ex : GSM_Init("1234");
 */ 
void GSM_Init(char *pin);



/* =============================================
				Fonctions d'émission / réception SMS
 ==============================================*/
 
/**
 * Envoi une chaine de caractere a un module GSM qui le
 * transmet sous forme de SMS
 * @param *Dest : Pointeur sur une chaine de caractere correspondant au num de tel à joindre
 * @param *Msg  : Pointeur sur une chaine de caractere qui constitue le SMS
 * Exemple : GSM_Send_SMS("06xxxxxxxx","Bonjour ");
 */ 
void GSM_Send_SMS(char *Dest,char *Msg);


/**
 * Attend la réception d'un SMS, fonction bloquante
 * @return Pointeur contenant l'adresse du premier caractère de la châine reçue 
 * Exemple : 
 *			char * Reponse;
 * 			Reponse=GSM_Get_SMS();
 */

char * GSM_Get_SMS(void);

#endif
