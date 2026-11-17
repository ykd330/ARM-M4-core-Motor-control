#include "device_driver.h"
#define MOTORCHANGETIME 1500

void motor_Init(int PinA, int PinB)
{
	Macro_Write_Block(GPIOA->MODER, 0x3, 0x1, (PinA * 2));
	Macro_Write_Block(GPIOA->MODER, 0x3, 0x1, (PinB * 2));
	Macro_Write_Block(GPIOA->OTYPER, 0x1, 0x0, PinA);
	Macro_Write_Block(GPIOA->OTYPER, 0x1, 0x0, PinB);
	Macro_Write_Block(GPIOA->ODR, 0x1, 0x0, PinA);
	Macro_Write_Block(GPIOA->ODR, 0x1, 0x0, PinB);
}

void motor_rev()
{
	unsigned char dir_change = GPIOA->ODR;
	Macro_Write_Block(GPIOA->ODR, 0x3, 0x3, 0);
	Macro_Invert_Bit(GPIOA->ODR, 5);
	TIM2_Delay(MOTORCHANGETIME);
	Macro_Write_Block(GPIOA->ODR, 0x3, ((~dir_change) & 0x3), 0);
}

void motor_run_f() {
	Macro_Write_Block(GPIOA->ODR, 0x3, 0x1, 0);
}

void motor_run_r() {
	Macro_Write_Block(GPIOA->ODR, 0x3, 0x2, 0);
}

void motor_stop_en() {
	Macro_Write_Block(GPIOA->ODR, 0x3, 0x3, 0);
}

void motor_stop_de() {
	Macro_Write_Block(GPIOA->ODR, 0x3, 0x0, 0);
}