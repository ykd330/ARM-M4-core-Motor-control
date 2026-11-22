#include "device_driver.h"
#include <stdio.h>

void _Invalid_ISR(void)
{
	unsigned int r = Macro_Extract_Area(SCB->ICSR, 0x1ff, 0);
	printf("\nInvalid_Exception: %d!\n", r);
	printf("Invalid_ISR: %d!\n", r - 16);
	for (;;)
		;
}

void EXTI15_10_IRQHandler(void)
{
	EXTI->PR = 0x1 << 13;
	Button = 1;
	TIM4_Repeat_Interrupt_Enable(1);
	NVIC_ClearPendingIRQ(40);
}

void EXTI1_IRQHandler(void)
{
	EXTI->PR = 0x1 << 1;
	NVIC_ClearPendingIRQ(7);
	if (!((GPIOB->IDR >> 1) & 0x1))
	{
		if (motor_PWM < 8)
			motor_PWM++;
	}
	PWM_refresh = 1;
}

void EXTI2_IRQHandler(void)
{
	EXTI->PR = 0x1 << 2;
	NVIC_ClearPendingIRQ(8);
	if (!((GPIOB->IDR >> 2) & 0x1))
	{
		if (motor_PWM > 0)
			motor_PWM--;
	}
	PWM_refresh = 1;
}

void USART2_IRQHandler(void)
{
	uart_data = (unsigned char)USART2->DR;
	while (!((USART2->SR >> 7) & 0x1));
	USART2->DR = uart_data;
	NVIC_ClearPendingIRQ(38);
}

void TIM3_IRQHandler(void)
{
	motor_start_interrupt = 0;
	TIM3_Interrupt_Enable(0, 0);
}

void TIM4_IRQHandler(void)
{
	TIM4_Repeat_Interrupt_Enable(0);
	Button = 0;
	state = !state;
}
