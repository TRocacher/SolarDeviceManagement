/*
 * MyIT.c
 *
 *  Created on: Jan 9, 2022
 *      Author: trocache
 */

#include "MyIT.h"

int Tab_Ech[12];
int IndiceTab;
int NbEch;

void MyIT_Init(void)
{

for (IndiceTab=0;IndiceTab<12;IndiceTab++)
	{
	Tab_Ech[IndiceTab]=0;
	}
IndiceTab=0;
NbEch=0;

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
			if (FrontUp==FrontMax)
			{
				Start=0;
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

	IndiceTab=NbEch/32;

	// insertion d'un 0 dans le 32bits courant
	Tab_Ech[ IndiceTab]=Tab_Ech[ IndiceTab]<<1;

	// insertion d'un 1 si l'input vaut 1
	 if (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_10)==GPIO_PIN_SET)
	 {
		 Tab_Ech[ IndiceTab]=Tab_Ech[ IndiceTab]|1;
	 }


	 if (NbEch<383)
	 {
		 NbEch++;
	 }
	 else
	 {
		 NbEch=NbEch;
	 }

	// fin pulse
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
}
