

//=======================================================================================
// 	Auteur T.R.
//  Juin 2018 , 
//  Processeur : STM32F103RB à 72MHz
//  Logiciel dev : µVision 5.23
//=======================================================================================


#ifndef _FctDiverses_H__
#define _FctDiverses_H__

/* =============================================
				DIVERS
 ============================================= */
/*
Fonction permettant d'attendre environ x ms
Procède par boucle while , donc peu précis.
Testé pour CPU à 72MHz
*/
void Delay_x_ms(int x);

#endif
