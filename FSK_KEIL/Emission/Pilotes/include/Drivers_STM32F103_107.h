

//=======================================================================================
// 	Auteur T.R.
//  
//=======================================================================================

#include "stm32f10x.h"

#ifndef _DRIVERS_STM32F103_107_H__
#define _DRIVERS_STM32F103_107_H__


/* =============================================
				GPIO
 ============================================= */
//______________________________________________________________________________
	   
char GPIO_Configure(GPIO_TypeDef * Port, int Broche, int Sens, int Techno);
//______________________________________________________________________________
/*
La fonction initialise n'importe quelle broche de port (entrée, sortie, techno....)
Exemple :
Port_IO_Init(GPIOB, 8, OUTPUT, OUTPUT_PPULL);
Place le bit 8 du port B en sortie Push-pull
Renvoie 0 si tout est OK,  et 1 s'il y a un problème (plage d'entrée non repsectée)
*/

// Sens
#define INPUT 				'i'
#define OUTPUT 				'o'

// Techno pour pin en entrée (INPUT)
#define ANALOG 				0
#define INPUT_FLOATING 		1
#define INPUT_PULL_DOWN_UP 	2

// Techno pour pin en sortie (OUTPUT)
#define OUTPUT_PPULL 		0
#define OUTPUT_OPDRAIN 		1
#define ALT_PPULL 			2
#define ALT_OPDRAIN 		3
/*
Fonction d'utilisation d'une PIN
Ex : GPIO_Pin_Set(Port_B,4);
La fonction GPIO_Pin_Read renvoie 0 si la PIN est au niveau logique 0
Sinon, c'est que l'entrée est au niveau logique 1
*/

//______________________________________________________________________________
// Macro
//______________________________________________________________________________
#define GPIO_Set(GPIO,Broche) GPIO->BSRR=(0x01<<Broche)
// Spécifier GPIO (GPIOA..GPIOC)
// Spécifier le numéro de broche (0 à 15)
// exemple : Port_IO_Set(GPIOB,8);

#define GPIO_Clear(GPIO,Broche) GPIO->BRR=(0x01<<Broche)
// Spécifier Port (GPIOA..GPIOC)
// Spécifier le numéro de broche (0 à 15)
// exemple : GPIO_Clear(GPIOB,8);

#define GPIO_Read(GPIO,Broche) ((GPIO->IDR)&(0x01<<Broche))
// Spécifier Port (GPIOA..GPIOC)
// Spécifier le numéro de broche (0 à 15) 
// Port_IO_Lire (GPIO,Broche) est un entier 16 bits	 qui vaut
// 0x0000 si la broche est à 0,  0x01<<Broche sinon
// Cette macro est utilisée lors d'un test de mise à 1 ou à 0
// d'une broche, évidemment configurée en entrée.







/* =============================================
				LES TIMERS
 ============================================= */

//______________________________________________________________________________

float Timer_1234_Init(TIM_TypeDef *Timer, float Duree_us );
//______________________________________________________________________________

/*
Timer : de TIM1 à TIM4
Duree_us : c'est l'intervalle de temps exprimé en µs entre deux débordements successifs
Renvoie la durée véritable, au plus proche de la valeur souhaitée
Le prescaler est calculé le plus petit possible pour une précision la plus 
fine possible). Le Timer est lancé en mode UP et non down (modif 2015).
NB: Lancer avant la fonction d'initialisétion du système d'hrologe. 
Elle permet de régler toutes les horloges par défaut.

Exemple :
CLOCK_Configure(); // lance l'initialisétion du système d'horloge de la puce
Timer_1234_Init(TIM2, 10.0 );   // Lance le timer avec une périodicité de 10.0 uS. 
*/
//______________________________________________________________________________




//______________________________________________________________________________

void Active_IT_Debordement_Timer( TIM_TypeDef *Timer, char Prio, void (*IT_function) (void));
//______________________________________________________________________________

/*
La fonction initialise le périphérique et le NVIC de manière à générer une interruption 
à chaque débordement du timer précisé.
La fonction passée en paramètre, forcément de type void Toto(void) est lancée sous interruption
Prio fixe la priorité (0 à 15, 0 étant le plus prioritaire)

Exemple :
Active_IT_Debordement_Timer( TIM2, 1, IT_Timer2); // La fonction active une interruption 
lors du débordement du Timer 2. La fonction appelée est IT_Timer2. La priorité associée est	1 
*/




#endif
