/*
 * SendToClim.c
 *
 *  Created on: Feb 13, 2022
 *      Author: trocache
 */



#include <ClimTeleco.h>
#include "stm32f1xx_hal.h"
#include "MyLCD.h"

char Tab_CommonCode_Temp[19]={0xFF, 0x08, 0x8A, 0xA2, 0xA2, 0x28, 0xA8, 0x8A, 0x22, 0xA2, 0xA2, 0xAA, 0xAA, 0xAA, 0xAA, 0x8A, 0x8A, 0x8A, 0xAA};
char TabCode[6][18]={{0xA2, 0x88, 0xAA, 0xAA, 0x28, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA8, 0x88, 0x8A, 0x22, 0x80},
		             {0xAA, 0x22, 0xAA, 0xA8, 0xA2, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA8, 0x88, 0xA2, 0x28, 0x00},
					 {0xA2, 0x28, 0xAA, 0xAA, 0x28, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA8, 0xA2, 0x28, 0x8A, 0x00},
					 {0xA8, 0xA2, 0xAA, 0xA8, 0xA2, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x22, 0x88, 0xA0, 0x00},
					 {0xA2, 0xA2, 0xAA, 0xA8, 0xA2, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA2, 0x28, 0xA2, 0x28, 0x00},
					 {0xAA, 0x8A, 0xAA, 0xA2, 0x8A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA2, 0x8A, 0x22, 0x80, 0x00}
};

char TabOFF[36]={0xFF, 0x08, 0x8A, 0xA2, 0xA2, 0x28, 0xA8, 0x8A, 0x22, 0xA2, 0xA2, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x2A, 0x22, 0xAA, 0xA8, 0x8A, 0xAA, 0x28, 0x8A, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xA8, 0x8A, 0x2A, 0x28};


// indices des lignes
#define Chaud_18_VanBas_FanAuto 0
#define Chaud_19_VanBas_FanAuto 1
#define Chaud_20_VanBas_FanAuto 2
#define Chaud_21_VanBas_FanAuto 3
#define Chaud_22_VanBas_FanAuto 4
#define Chaud_23_VanBas_FanAuto 5
#define Stop 6

#define CodeMax Stop

#define LenCommonCode 19
#define LenTabCode 18
#define LenStopCode 36



typedef enum {
	WaitForUserBp,
	SetNewCodeToSend,
	SendCode,
}ClimTelecoState;

ClimTelecoState State;


char  UserBpPushed;
int CodeToSend;
char SendCodeEnable;
int IndexOctet;
char IndexBit;
char SendCodeEnd;
char TempPremabuleTransmission;

void ClimTeleco_Init(void)
{
	State=WaitForUserBp;
	CodeToSend=0;
	UserBpPushed=0;
	MyLCD_Init();
	MyLCD_Clear();
	MyLCD_ClearLineUp();
	MyLCD_Set_cursor(0,0);
	MyLCD_Print("Teleco Mitsu");
	TIM2->CCER&=~TIM_CCER_CC1E;
}

void ClimTeleco_SM(void)
{
	switch(State)
	{
	case WaitForUserBp:
		{
		if (UserBpPushed==1)
			{
			UserBpPushed=0;
			State=SetNewCodeToSend;
			SendCodeEnable=0;
			SendCodeEnd=0;
			IndexOctet=0;
			IndexBit=128; // rang 7 / poids 128

			}
		break;
		}
	case SetNewCodeToSend:
		{
			CodeToSend++;
			if (CodeToSend==CodeMax+1) CodeToSend=0;

			// AFFICHAGE LCD
			MyLCD_ClearLineDown();
			MyLCD_Set_cursor(0,1);
			if (CodeToSend==Stop) MyLCD_Print("Stop ...           ");
			else if (CodeToSend==Chaud_18_VanBas_FanAuto) MyLCD_Print("Chaud 18 ...         ");
			else if (CodeToSend==Chaud_19_VanBas_FanAuto) MyLCD_Print("Chaud 19 ...         ");
			else if (CodeToSend==Chaud_20_VanBas_FanAuto) MyLCD_Print("Chaud 20 ...         ");
			else if (CodeToSend==Chaud_21_VanBas_FanAuto) MyLCD_Print("Chaud 21 ...         ");
			else if (CodeToSend==Chaud_22_VanBas_FanAuto) MyLCD_Print("Chaud 22 ...         ");
			else   MyLCD_Print("Chaud 23 ...         ");
			IndexOctet=0;
			IndexBit=128; // rang 7 / poids 128
			SendCodeEnable=1;
			TempPremabuleTransmission=1;
			State=SendCode;
			break;
		}
	case SendCode:
		{
		if (SendCodeEnd==1)
			{
			SendCodeEnd=0;
			State=WaitForUserBp;
			}
		break;
		}
	default : State=WaitForUserBp;
	}
}


// INTERRUPTION Routines
char CurrentByte;
void TIM3_IRQHandler(void)
{
	// clear flag
	TIM3->SR&=~TIM_SR_UIF;
	if (SendCodeEnable==1)
	{
		if (CodeToSend==Stop)
		{
			CurrentByte=TabOFF[IndexOctet];
			if ((CurrentByte&IndexBit)==IndexBit)
			{
				HAL_GPIO_WritePin (GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
				// valider Ch1 TIM2
				TIM2->CCER|=TIM_CCER_CC1E;
			}
			else
			{
				HAL_GPIO_WritePin (GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
				TIM2->CCER&=~TIM_CCER_CC1E;
			}
			IndexBit=IndexBit>>1;

			if (IndexBit==0) // débordement
			{
				IndexBit=128;
				IndexOctet++;
				if (IndexOctet==LenStopCode)
				{
					SendCodeEnable=0;
					SendCodeEnd=1;
				}
			}
		}
		else
		{
			if (TempPremabuleTransmission==1)
			{
				// émission préambule commun
				CurrentByte=Tab_CommonCode_Temp[IndexOctet];
				if ((CurrentByte&IndexBit)==IndexBit)
				{
					// PC11 = 1
					HAL_GPIO_WritePin (GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
					TIM2->CCER|=TIM_CCER_CC1E;
				}
				else
				{
					HAL_GPIO_WritePin (GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
					TIM2->CCER&=~TIM_CCER_CC1E;
				}
				IndexBit=IndexBit>>1;

				if (IndexBit==0) // débordement
				{
					IndexBit=128;
					IndexOctet++;
					if (IndexOctet==LenCommonCode)
					{
						// passage au code température prochaine it
						TempPremabuleTransmission=0;
						IndexOctet=0;
						IndexBit=128;
					}
				}
			}
			else //  TempPremabuleTransmission =0
			{
				// émission particulière temp
				CurrentByte=TabCode[CodeToSend][IndexOctet];
				if ((CurrentByte&IndexBit)==IndexBit)
				{
					// PC11 = 1
					HAL_GPIO_WritePin (GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
					TIM2->CCER|=TIM_CCER_CC1E;
				}
				else
				{
					HAL_GPIO_WritePin (GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
					TIM2->CCER&=~TIM_CCER_CC1E;
				}
				IndexBit=IndexBit>>1;

				if (IndexBit==0) // débordement
				{
					IndexBit=128;
					IndexOctet++;
					if (IndexOctet==LenTabCode)
					{
						SendCodeEnable=0;
						SendCodeEnd=1;
						TempPremabuleTransmission=1;
					}
				}
			}
		}


	}
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin==GPIO_PIN_13) UserBpPushed=1;
}
