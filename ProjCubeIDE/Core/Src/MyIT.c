/*
 * MyIT.c
 *
 *  Created on: Jan 9, 2022
 *      Author: trocache
 */

#include "MyIT.h"
#include "main.h"

int IndiceTab;
int NbEch;

void MyIT_Init(void)
{

for (IndiceTab=0;IndiceTab<Nb_Octets;IndiceTab++)
	{
	Tab_Ech[IndiceTab]=0;
	}
IndiceTab=0;
NbEch=0;
finish=0;

}






void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) // front montant ou descendant GPIOC.10
{

NVIC->ICPR[1]|=(0x1<<8); // mise à 0 du pendig bit par mise à 1 !

//if (GPIO_Pin==GPIO_PIN_10) // front montant ou descendant input
	//{

		if (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_10)==GPIO_PIN_SET) // front up
		{
			if ((Start==0)&&(FrontUp==0))
			{
				Start=1;
			}

			else FrontUp++;

		}
		TIM2->CNT=0;
//	}
}

void TIM2_IRQHandler(void) // Match de CCR, instant d'échantillonnage
{
	// clear flag
	TIM2->SR&=~TIM_SR_CC1IF;

	// début pulse éch
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);

	IndiceTab=NbEch/8;

	// insertion d'un 0 dans le 32bits courant
	Tab_Ech[ IndiceTab]=Tab_Ech[ IndiceTab]<<1;

	// insertion d'un 1 si l'input vaut 1
	 if (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_10)==GPIO_PIN_SET)
	 {
		 Tab_Ech[ IndiceTab]=Tab_Ech[ IndiceTab]|1;
	 }


	 if (NbEch<NbBitsMax)
	 {
		 NbEch++;
	 }
	 else
	 {
		 NbEch=NbEch;
		 Start=0; // arrêt du système, tous les points sont échantillonnés
		 	 	  // (384 = 12 mots de 32bits
		 finish=1;
	 }

	// fin pulse
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
}
