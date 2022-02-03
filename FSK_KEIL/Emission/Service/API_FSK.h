/*----------------------------------------------------------------------------

 *---------------------------------------------------------------------------*/

#ifndef _API_FSK_H__
#define _API_FSK_H__

void FSK_Init(void);
/*Son r�le se r�sume � initialiser la bonne USART et les pins de 
commandes RxCmd et TxCmd en sortie pushpull. */

void FSK_Send_Str(char *Msg);
/*La fonction envoie une cha�ne de caract�re dont l'adresse 
du premier caract�re sera pass� dans le param�tre Msg (pointeur de caract�re).*/ 

char * FSK_Get_Str(void);
/*La fonction utilise directement la fonction 
char * Get_String(USART_TypeDef *USART) de la lib USART_rev. 
Notez que par d�faut, une cha�ne de caract�res est consid�r�e 
compl�te losque le caract�re <CR> est re�u. Ce caract�re de fin 
de cha�ne est modifiable dans la fichier USART_User_Conf_2018.h.*/

/******************************************************************************/

#endif
