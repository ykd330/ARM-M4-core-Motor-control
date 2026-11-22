#include "device_driver.h"

void Key_Poll_Init(void)
{
	Macro_Set_Bit(RCC->AHB1ENR, 1); 
	Macro_Set_Bit(RCC->AHB1ENR, 2); 
	Macro_Write_Block(GPIOC->MODER, 0x3, 0x0, 26);
	Macro_Write_Block(GPIOB->MODER, 0x3, 0x0, 2);
	Macro_Write_Block(GPIOB->MODER, 0x3, 0x0, 4);
}

int Key_Get_Pressed(void)
{
	return Macro_Check_Bit_Clear(GPIOC->IDR, 13);	
}

void Key_ISR_Enable(int en)
{
	if(en)
	{
		Macro_Set_Bit(RCC->AHB1ENR, 1); 
		Macro_Set_Bit(RCC->AHB1ENR, 2); 
		Macro_Write_Block(GPIOC->MODER, 0x3, 0x0, 26);
		Macro_Write_Block(GPIOB->MODER, 0x3, 0x0, 2);
		Macro_Write_Block(GPIOB->MODER, 0x3, 0x0, 4);
		Macro_Write_Block(GPIOB->PUPDR, 0x3, 0x1, 2);
		Macro_Write_Block(GPIOB->PUPDR, 0x3, 0x1, 4);
		
		Macro_Set_Bit(RCC->APB2ENR, 14); 
		Macro_Write_Block(SYSCFG->EXTICR[3], 0xf, 0x2, 4);
		Macro_Write_Block(SYSCFG->EXTICR[0], 0xff, 0x11, 4);
	
		Macro_Set_Bit(EXTI->FTSR, 13);
		Macro_Set_Bit(EXTI->FTSR, 1);
		Macro_Set_Bit(EXTI->FTSR, 2);
		
		EXTI->PR = (0x1 << 13) | (0x1 << 1) | (0x1 << 2);
		
		NVIC_ClearPendingIRQ((IRQn_Type)7);
		NVIC_ClearPendingIRQ((IRQn_Type)8);
		NVIC_ClearPendingIRQ((IRQn_Type)40);
		Macro_Set_Bit(EXTI->IMR, 13);
		Macro_Set_Bit(EXTI->IMR, 1);
		Macro_Set_Bit(EXTI->IMR, 2);
		
		NVIC_EnableIRQ((IRQn_Type)40);
		NVIC_EnableIRQ((IRQn_Type)7);
		NVIC_EnableIRQ((IRQn_Type)8);
		
	}

	else
	{
		NVIC_DisableIRQ((IRQn_Type)40);
		NVIC_DisableIRQ((IRQn_Type)7);
		NVIC_DisableIRQ((IRQn_Type)8);
		
	}
}

