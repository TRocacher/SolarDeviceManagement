#include "API_ADC.h"
#include "stm32f10x.h"
#include "ADC_DMA.h"
#include "GPIO.h"



/* =============================================
				L'ADC
 ============================================= */

void ADC_Global_Conf(int Pin)
{
	switch(Pin)
	{
		case PA0:Port_IO_Init(GPIOA, 0, INPUT, ANALOG);break; 
		case PA1:Port_IO_Init(GPIOA, 1, INPUT, ANALOG);break;
		case PA2:Port_IO_Init(GPIOA, 2, INPUT, ANALOG);break;
		case PA3:Port_IO_Init(GPIOA, 3, INPUT, ANALOG);break;
		case PA4:Port_IO_Init(GPIOA, 4, INPUT, ANALOG);break;
		case PA5:Port_IO_Init(GPIOA, 5, INPUT, ANALOG);break;
		case PA6:Port_IO_Init(GPIOA, 6, INPUT, ANALOG);break;
		case PA7:Port_IO_Init(GPIOA, 7, INPUT, ANALOG);break;
		case PB0:Port_IO_Init(GPIOB, 0, INPUT, ANALOG);break;
		case PB1:Port_IO_Init(GPIOB, 1, INPUT, ANALOG);break;
		case PC0:Port_IO_Init(GPIOC, 0, INPUT, ANALOG);break;
		case PC1:Port_IO_Init(GPIOC, 1, INPUT, ANALOG);break;
		case PC2:Port_IO_Init(GPIOC, 2, INPUT, ANALOG);break;
		case PC3:Port_IO_Init(GPIOC, 3, INPUT, ANALOG);break;
		case PC4:Port_IO_Init(GPIOC, 4, INPUT, ANALOG);break;
		case PC5:Port_IO_Init(GPIOC, 5, INPUT, ANALOG);break;
	}

	Init_TimingADC_ActiveADC(ADC1, 10.0 );
}

int ADC_Get_Value(int Pin)
{
Single_Channel_ADC(ADC1, Pin);
Start_ADC_Conversion(ADC1);	
Wait_On_EOC_ADC (ADC1);
return Read_ADC(ADC1);
}

