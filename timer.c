#include "device_driver.h"



void TIM2_Stopwatch_Start(void)
{
	Macro_Set_Bit(RCC->APB1ENR, 0);

	// TIM2 CR1 설정: down count, one pulse
	TIM2->CR1 = (0x1 << 4) | (0x1 << 3);

	// PSC 초기값 설정 => 20usec tick이 되도록 설계 (50KHz)
	TIM2->PSC = TIM2_PSC - 1;
	// ARR 초기값 설정 => 최대값 0xFFFFFFFF 설정
	TIM2->ARR = TIM2_ARR_MAX;
	// UG 이벤트 발생
	TIM2->SR |= 0x1 << 0;
	// TIM2 start
	TIM2->CR1 |= 0x1 << 0;
}

unsigned int TIM2_Stopwatch_Stop(void)
{
	TIM2->CR1 &= ~(0x1 << 0);
	return (TIM2->ARR - TIM2->CNT) * TIM2_PPS;
}

void TIM2_Delay(int time)
{
	Macro_Set_Bit(RCC->APB1ENR, 0);

	TIM2->CR1 = (0x0 << 7) | (0x1 << 4) | (0x1 << 3);
	TIM2->PSC = ((int)((CPU_CLOCK /TIM2_FREQ) + 0.5) - 1);
	TIM2->ARR = time * TIM2_PPS;
	TIM2->EGR |= 0x1 << 0;

	// UIF(Update Interrupt Pending) Clear
	TIM2->SR &= ~(0x1 << 0);
	
	// TIM2 start
	TIM2->CR1 |= 0x1 << 0;

	// Wait timeout
	while(!Macro_Check_Bit_Set(TIM2->SR, 0));

	// TIM2 Stop
	Macro_Clear_Bit(TIM2->CR1, 0);
}

void TIM4_Repeat(int time)
{
	Macro_Set_Bit(RCC->APB1ENR, 2);

	TIM4->CR1 = (0x0 << 7) | (0x1 << 4) | (0x0 << 3);
	TIM4->PSC = ((int)((CPU_CLOCK / TIM2_FREQ) + 0.5) - 1);
	TIM4->ARR = time * TIM2_PPS;
	TIM4->EGR |= 0x1 << 0;
	TIM4->SR &= ~(0x1 << 0);
	TIM4->CR1 |= (0x1 << 0);
}

int TIM4_Check_Timeout(void)
{
	// 타이머가 timeout 이면 1 리턴, 아니면 0 리턴
	int i = ((TIM4->SR >> 0) & 0x1) ? 1 : 0;
	TIM4->SR &= ~(0x1 << 0);
	return i;
}

void TIM4_Stop(void)
{
	Macro_Clear_Bit(TIM4->CR1, 0);
}

void TIM4_Stop(void)
{
	Macro_Clear_Bit(TIM4->CR1, 0);
}

void TIM4_Change_Value(int time)
{
	TIM4->ARR = 50 * time;
}
