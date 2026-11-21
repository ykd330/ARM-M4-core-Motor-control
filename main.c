#include "device_driver.h"
#include <stdio.h>
#define MOTOR_PIN_A 0
#define MOTOR_PIN_B 1

enum Motor_State
{
	Motor_Run_F,
	Motor_Run_R
};

enum State
{
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

void Main(void)
{
	Sys_Init(115200);
	Key_ISR_Enable(1);
	motor_Init(MOTOR_PIN_A, MOTOR_PIN_B);
	unsigned char current_state;
	volatile unsigned long motor_speed[9] = {2000, 2400, 2600, 2800, 3000, 3200, 3400, 3600, 4000};
	for (;;)
	{

		if (state == Motor_Off)
		{
			LED_Off();
			motor_stop();
			current_state = get_motor_state();
		}
		else if (state == Motor_On)
		{
			LED_On();
			if (motor_state != current_state)
				motor_start(motor_state);
			if (Button && (Key_Check_Input() == 0))
			{
				TIM4_Repeat_Interrupt_Enable(0);
				Button = 0;
				motor_rev();
			}
		}
	}
}