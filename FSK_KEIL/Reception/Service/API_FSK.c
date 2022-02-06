#include "GPIO.h"
#include "FctDiverses.h"
#include "USART_rev2021.h"

#define USART_FSK USART3

/*Son rôle se résume à initialiser la bonne USART et les pins de 
commandes RxCmd et TxCmd en sortie pushpull. */
void FSK_Init(void)
{
	Init_USART(USART_FSK,38400, 0); 
	GPIO_Configure(GPIOB, 8, OUTPUT, OUTPUT_PPULL ); // RxCmde
	GPIO_Configure(GPIOB, 9, OUTPUT, OUTPUT_PPULL ); // TxCmde
}


void FSK_Send_Str(char *Msg)
/*La fonction envoie une chaîne de caractère dont l'adresse 
du premier caractère sera passé dans le paramètre Msg (pointeur de caractère).*/ 
{
	// TxCmde = 1
	Port_IO_Set(GPIOB,9);
	Delay_x_ms(10);
	Put_String(USART_FSK,Msg);
	Delay_x_ms(3);
	GPIO_Clear(GPIOB,9);
}


char * FSK_Get_Str(void);

/*La fonction utilise directement la fonction 
char * Get_String(USART_TypeDef *USART) de la lib USART_rev. 
Notez que par défaut, une chaîne de caractères est considérée 
complète losque le caractère <CR> est reçu. Ce caractère de fin 
de chaîne est modifiable dans la fichier USART_User_Conf_2018.h.*/

