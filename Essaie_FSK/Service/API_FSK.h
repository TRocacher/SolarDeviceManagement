/*----------------------------------------------------------------------------

 *---------------------------------------------------------------------------*/

#ifndef _API_FSK_H__
#define _API_FSK_H__

void FSK_Init(void);
/*Son rôle se résume à initialiser la bonne USART et les pins de 
commandes RxCmd et TxCmd en sortie pushpull. */

void FSK_Send_Str(char *Msg);
/*La fonction envoie une chaîne de caractère dont l'adresse 
du premier caractère sera passé dans le paramètre Msg (pointeur de caractère).*/ 

char * FSK_Get_Str(void);
/*La fonction utilise directement la fonction 
char * Get_String(USART_TypeDef *USART) de la lib USART_rev. 
Notez que par défaut, une chaîne de caractères est considérée 
complète losque le caractère <CR> est reçu. Ce caractère de fin 
de chaîne est modifiable dans la fichier USART_User_Conf_2018.h.*/

/******************************************************************************/

#endif
