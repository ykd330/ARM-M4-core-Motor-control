#include "device_driver.h"
#include <stdio.h>
#define MOTOR_PIN_A 0
#define MOTOR_PIN_B 1

enum Motor_State {
	Motor_Run_F,
	Motor_Run_R
};

enum State {
	Motor_Off,
	Motor_On
};

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
	motor_Init(MOTOR_PIN_A, MOTOR_PIN_B);

	unsigned char lock = 0, state = 0, motor_state = 0, dir_change = 0, x = 0, on = 0;
	for (;;)
	{
		x = Uart2_Get_Pressed(); 
		if (x != 0) {
			USART2->DR = x;
			if((x == 's') || (x == 'S')) {
				state = Motor_Off;
			}
			else {
				if ((x == 'r') || (x == 'R')) {
					state = Motor_On;
					motor_state = Motor_Run_R;
				}
				else if ((x == 'f') || (x == 'F')) {
					state = Motor_On;
					motor_state = Motor_Run_F;
				}
				else if ((1 <= x) && (9 >= x)) {
				}
			}
		}
		x = 0;

		if ((lock == 0) && Macro_Check_Bit_Set(GPIOC->IDR, 13)) {
			lock = 1;
			state = Motor_On;
			if (Macro_Check_Bit_Set(GPIOA->ODR, 0) == Macro_Check_Bit_Set(GPIOA->ODR, 1))
				Macro_Invert_Bit(GPIOA->ODR, 0);
			else
				motor_state = !motor_state;
			LED_Display(on ^= 0x1);
		}
		else if ((lock == 1) && Macro_Check_Bit_Clear(GPIOC->IDR, 13)) lock = 0;



		if (state == Motor_Off)
		{
			motor_stop_de();
			dir_change = 10;
		}
		else if (state == Motor_On)
		{
			if(dir_change == 10){
				if(motor_state == Motor_Run_F) {
					dir_change = motor_state;
					motor_run_f();
				}
				else if(motor_state == Motor_Run_R) {
					dir_change = motor_state;
					motor_run_r();
				}
			}
			if(dir_change != motor_state) {
				motor_rev();
				dir_change = motor_state;
			}
		}
	}
}
