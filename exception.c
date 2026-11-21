#include "device_driver.h"
#include <stdio.h>

void _Invalid_ISR(void)
{
	unsigned int r = Macro_Extract_Area(SCB->ICSR, 0x1ff, 0);
	printf("\nInvalid_Exception: %d!\n", r);
	printf("Invalid_ISR: %d!\n", r - 16);
	for(;;);
}

void EXTI15_10_IRQHandler(void)
{
	Button = 1;
	TIM4_Repeat_Interrupt_Enable(1);
	EXTI->PR = 0x1 << 13;
	NVIC_ClearPendingIRQ(40);
}

void USART2_IRQHandler(void)
{
    uart_data = (unsigned char)USART2->DR;
    USART2->DR = uart_data;

    NVIC_ClearPendingIRQ(38);
}

void TIM3_IRQHandler(void) {
	TIM3_Interrupt_Enable(0, 0);
	motor_start_interrupt = 0;
}

void TIM4_IRQHandler(void)
{
	TIM4_Interrupt_Enable(0, 0);
	Button = 0;
	state = !state;
}
