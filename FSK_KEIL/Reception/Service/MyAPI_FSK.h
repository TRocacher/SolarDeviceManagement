/*----------------------------------------------------------------------------

 *---------------------------------------------------------------------------*/

#ifndef _API_FSK_H__
#define _API_FSK_H__

#include "stm32f10x.h"

// New
void FSK_Init(int Baud_Rate_bits_par_Sec, USART_TypeDef * USART_FSK_, TIM_TypeDef * FSK_Timer_);
/*Son r�le se r�sume � initialiser la bonne USART et les pins de 
commandes RxCmd et TxCmd en sortie pushpull. */

void FSK_Send_Str(char *Msg);
/*La fonction envoie une cha�ne de caract�re dont l'adresse 
du premier caract�re sera pass� dans le param�tre Msg (pointeur de caract�re).*/ 


//New
void SM_Recept_FSK(void);


// New
// getter setter
void ResetFlag_CodeReadyForRead (void);
char IsCodeReadyForRead(void);
char * GetCodeAdress(void);


typedef enum {
	TimeOut,
	WrongCRC,
	OK
}Status;

Status GetCodeStatus(void);



/******************************************************************************/

#endif
