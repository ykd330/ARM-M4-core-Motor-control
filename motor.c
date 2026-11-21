#include "device_driver.h"
#define MOTOR_FREQ 8000000.
#define MOTOR_PSC ((int)(((SYSCLK / MOTOR_FREQ) + 0.5)) - 1)
#define MOTOR_ARR 4000.
volatile unsigned char motor_state;
volatile unsigned char motor_start_interrupt = 0;

void motor_Init(int PinA, int PinB)
{
	RCC->APB1ENR |= 0x1 << 0;
	Macro_Write_Block(GPIOA->MODER, 0x3, 0x2, (PinA * 2));
	Macro_Write_Block(GPIOA->MODER, 0x3, 0x2, (PinB * 2));
	Macro_Write_Block(GPIOA->AFR[0], 0xff, 0x11, 0);
	TIM2->PSC = MOTOR_PSC;
	TIM2->ARR = MOTOR_ARR;
	TIM2->CCMR1 = 0x6868;
	TIM2->CCER |= ((0x1 << 4) | (0x1 << 0));
	TIM2->CCR1 = (MOTOR_ARR);
	TIM2->CCR2 = (MOTOR_ARR);
	TIM2->CCER = (TIM2->CCER & ~((0x1 << 5) | (0x1 << 1))) | ((0x1 << 4) | (0x1 << 0));
	TIM2->EGR |= 0x1 << 0;
	TIM2->SR &= ~(0x1 << 0);
	TIM2->CR1 |= (0x1<<4)|(0x1<<0);
}

void motor_rev()
{
	GPIOA->AFR[0] = 0;
	Macro_Invert_Bit(motor_state, 0);
	Macro_Invert_Bit(motor_state, 4);
	motor_start_interrupt = 1;
	TIM3_Interrupt_Enable(1, 1000);
}

void motor_start() {
	if (!motor_start_interrupt)
		GPIOA->AFR[0] = motor_state;
}

void motor_stop() {
	GPIOA->AFR[0] = 0x00;
}

void motor_set_dir(unsigned char dir) {
	if(motor_state == dir) return;
	GPIOA->AFR[0] = 0x00;
	
	motor_state = dir;
	motor_start_interrupt = 1;
	TIM3_Interrupt_Enable(1, 1000);
}

void motor_set_speed(unsigned long motor_speed) {
	TIM2->CCR1 = motor_speed;
	TIM2->CCR2 = motor_speed;
}

unsigned char get_motor_state() {
	return GPIOA->AFR[0];
}