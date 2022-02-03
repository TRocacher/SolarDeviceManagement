#include "stm32f10x.h"

#include "API_Gsm.h"
#include <string.h>
#include <stdio.h>
#include "FctDiverses.h" 
#include "USART_rev2018.h"



#define USART_GSM USART1




/* =============================================
				Fonctions et variables privées
 ==============================================*/

void Attendre_Reponse_Modem(char * Rep)
{
	/*
	La fonction attend une réponse précise.
	Elle attend en boucle des string.
	Si le string est trouvé, la fonction se termine
	*/

	char * Reponse;
	while(1)
	{		
		Reponse=Get_String(USART_GSM);
		if (strcmp(Reponse,Rep)==0) // cmp OK
		{
			break; // flag de sortie = 1
		}
	}
}



/* =============================================
				Fonctions d IT de réception
 ==============================================*/
int Nb_String_Rec;
char Flag_SMS_Rec;
void Fct_Rec_Str(void)
{
	Nb_String_Rec++;
	if (Nb_String_Rec==3) Flag_SMS_Rec=1;
}

/* =============================================
				Fonctions de configuration GSM
 ==============================================*/




void GSM_Init(char *pin)
// renvoie 1 si OK, 0 sinon
{
	char cmd[50] ;	
	
	// conf USART à 115200 bds
	Init_USART(USART_GSM,115200,0); 
	Init_IT_Serial_Receive_Str(USART_GSM,Fct_Rec_Str);
	Nb_String_Rec=0;
	Flag_SMS_Rec=0;
	
	// configuration carte SIM
	sprintf(cmd,"AT+CPIN=\"%s\"\r",pin); 
	Put_String(USART_GSM,cmd);
	Delay_x_ms(6000);
  	
	// SMS en mode txt
	sprintf(cmd,"AT+CMGF=1\r"); 
	Put_String(USART_GSM,cmd);
	Delay_x_ms(2000);                //Attendre_Reponse_Modem("OK");
 	
	// SMS lus depuis la mémoire du modem, SMS émis via la mémoire du modem, SMS reçus dans la mémoire du modem
	sprintf(cmd,"AT+CPMS=\"ME\",\"ME\",\"ME\"\r"); 
	Put_String(USART_GSM,cmd);
	Delay_x_ms(2000);                  //Attendre_Reponse_Modem("OK");
	
	// Réception directe des SMS (pas de notifications)
	sprintf(cmd,"AT+CNMI=2,2,0,0,0\r"); 
	Put_String(USART_GSM,cmd);
	Delay_x_ms(2000);                  //Attendre_Reponse_Modem("OK");
  //Attendre_Reponse_Modem("OK");
	
	// Initialisation variable de réception SMS
	Nb_String_Rec=0;
	Flag_SMS_Rec=0;
}


/* =============================================
				Fonctions d'utilisation GSM
 ==============================================*/


void GSM_Send_SMS(char *Dest,char *Msg)
{
	char cmd[20] ;	

	sprintf(cmd,"AT+CMGS=\"%s\"\r",Dest); 
	Put_String(USART_GSM,cmd);
	Delay_x_ms(500);                  //Attendre_Reponse_Modem("OK");
	Put_String(USART_GSM,Msg);
	Put_Char(USART_GSM,ESC);
	Attendre_Reponse_Modem("OK");
  
	// Initialisation variable de réception SMS
	Nb_String_Rec=0;
	Flag_SMS_Rec=0;
	
}



char * GSM_Get_SMS(void)
{
	while (Flag_SMS_Rec==0)
	{
	}
	Nb_String_Rec=0;
	Flag_SMS_Rec=0;
	return Read_Received_Current_String(USART_GSM);
}	
