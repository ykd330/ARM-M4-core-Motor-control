#include "device_driver.h"
#include <stdio.h>

static void Sys_Init(int baud)
{
	SCB->CPACR |= (0x3 << 10 * 2) | (0x3 << 11 * 2);
	Clock_Init();
	Uart2_Init(baud);
	setvbuf(stdout, NULL, _IONBF, 0);
	LED_Init();
}

/* Key.c의 옵션을 변경하면서 동작을 비교 실험 진행 */

void Main(void)
{
	Sys_Init(115200);
	printf("\nChattering 제거 방식 Key_Get_Pressed() 동작 시험\n");
	Macro_Write_Block(GPIOA->MODER, 0x3, 0x1, 0);
	Macro_Write_Block(GPIOA->MODER, 0x3, 0x1, 2);
	Macro_Write_Block(GPIOA->OTYPER, 0x1, 0x0, 0);
	Macro_Write_Block(GPIOA->OTYPER, 0x1, 0x0, 1);
	Macro_Write_Block(GPIOA->ODR, 0x1, 0x0, 0);
	Macro_Write_Block(GPIOA->ODR, 0x1, 0x1, 1);

	char lock = 0, check = 0, active = 1;
	volatile unsigned int time = 0;
	unsigned int state = 0;
	for (;;)
	{
		if (Key_Get_Pressed()) {
			Key_Wait_Key_Released();
			active = !active;
		}
		if (active == 0)
		{
			Macro_Write_Block(GPIOA->ODR, 0x3, 0x3, 0);
			check = 0;
		}
		else if (active == 1)
		{
			if(check == 0){
				check = 1;
				Macro_Write_Block(GPIOA->ODR, 0x3, 0x1, 0);
			}
			if ((lock == 0) && Macro_Check_Bit_Clear(GPIOC->IDR, 13))
			{
				state = GPIOA->ODR;
				Macro_Write_Block(GPIOA->ODR, 0x3, 0x3, 0);
				Macro_Invert_Bit(GPIOA->ODR, 5);
				for (time = 0; time < 0x1000000; time++);
				Macro_Write_Block(GPIOA->ODR, 0x3, ((~state) & 0x3), 0);
			}
			else if ((lock == 1) && Macro_Check_Bit_Set(GPIOC->IDR, 13))
			{
				lock = 0;
			}
		}
	}
}
