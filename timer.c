#include "device_driver.h"
#define TIM4_TICK	  		(20) 					// usec
#define TIM4_FREQ 	  		(1000000./TIM4_TICK) 	// Hz
#define TIME4_PLS_OF_1ms  	(1000./TIM4_TICK)

#define TIM_TICK	  	(100) 					// usec
#define TIM_FREQ 	  	(1000000./TIM_TICK) 	// Hz
#define TIM_PLS_OF_1ms  (1000./TIM_TICK)

void TIM4_Interrupt_Enable(int en, int time)
{
	if(en)
	{
		Macro_Set_Bit(RCC->APB1ENR, 2);

		TIM4->CR1 = (1<<4)|(1<<3);
		TIM4->PSC = (unsigned int)(SYSCLK/TIM4_FREQ + 0.5)-1;
		TIM4->ARR = TIME4_PLS_OF_1ms * time;
		Macro_Set_Bit(TIM4->EGR,0);

		Macro_Clear_Bit(TIM4->SR, 0);
		NVIC_ClearPendingIRQ(30);

		Macro_Set_Bit(TIM4->DIER, 0);
		NVIC_EnableIRQ(30);

		Macro_Set_Bit(TIM4->CR1, 0);
	}

	else
	{
		NVIC_DisableIRQ(30);
		Macro_Clear_Bit(TIM4->CR1, 0);
		Macro_Clear_Bit(TIM4->DIER, 0);
	}
}

void TIM4_Repeat_Interrupt_Enable(int en)
{
	if(en)
	{
		Macro_Set_Bit(RCC->APB1ENR, 2);

		TIM4->CR1 = (1<<4)|(1<<3);
		TIM4->PSC = (unsigned int)(SYSCLK/(double)TIM_FREQ + 0.5)-1;
		TIM4->ARR = TIM_PLS_OF_1ms * 2000;
		Macro_Set_Bit(TIM4->EGR,0);

		Macro_Clear_Bit(TIM4->SR, 0);
		NVIC_ClearPendingIRQ(30);

		Macro_Set_Bit(TIM4->DIER, 0);
		NVIC_EnableIRQ(30);

		Macro_Set_Bit(TIM4->CR1, 0);
	}
	else
	{
		Macro_Clear_Bit(TIM4->CR1, 0);
		Macro_Clear_Bit(TIM4->SR, 0);
		NVIC_DisableIRQ(30);
		Macro_Clear_Bit(TIM4->DIER, 0);
	}
}

#define TIM3_FREQ 	  			(8000000.) 	      		// Hz
#define TIM3_TICK	  			(20)	// usec
#define TIME3_PLS_OF_1ms  		(1000./TIM3_TICK)

void TIM3_Interrupt_Enable(int en, int time)
{
	if(en)
	{
		Macro_Set_Bit(RCC->APB1ENR, 1);

		TIM3->CR1 = (1<<4)|(1<<3);
		TIM3->PSC = (unsigned int)(SYSCLK/TIM4_FREQ + 0.5)-1;
		TIM3->ARR = TIME3_PLS_OF_1ms * time;
		Macro_Set_Bit(TIM3->EGR,0);

		Macro_Clear_Bit(TIM3->SR, 0);
		NVIC_ClearPendingIRQ(29);

		Macro_Set_Bit(TIM3->DIER, 0);
		NVIC_EnableIRQ(29);

		Macro_Set_Bit(TIM3->CR1, 0);
	}

	else
	{
		NVIC_DisableIRQ(29);
		Macro_Clear_Bit(TIM3->CR1, 0);
		Macro_Clear_Bit(TIM3->DIER, 0);
	}
}

