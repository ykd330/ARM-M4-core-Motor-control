#include "device_driver.h"
#include <stdio.h>
#define MOTOR_PIN_A 0
#define MOTOR_PIN_B 1

enum Motor_State
{
	Motor_Run_F = 0x01,
	Motor_Run_R = 0x10
};

enum State
{
	Motor_Off,
	Motor_On
};

const unsigned long motor_speed[9] = {2000, 2400, 2600, 2800, 3000, 3200, 3400, 3600, 4000};
volatile int Button = 0;
volatile unsigned long state = 0;
volatile unsigned char uart_data = 0;

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
	Uart2_RX_Interrupt_Enable(1);
	printf("start motor test");
	motor_set_dir(Motor_Run_F);
	for (;;)
	{
		
		if (uart_data != 0)
		{
			if ((uart_data == 'R') || (uart_data == 'r'))
			{
				motor_set_dir(Motor_Run_R);
			}
			else if ((uart_data == 'F') || (uart_data == 'f'))
			{
				motor_set_dir(Motor_Run_F);
			}
			else if ((uart_data >= '1') && (uart_data <= '9'))
			{
				motor_set_speed(motor_speed[uart_data - '1']);
			}
			uart_data = 0;
		}

		if ((Button && !Key_Get_Pressed()))
		{
			TIM4_Repeat_Interrupt_Enable(0);
			Button = 0;
			motor_rev();
		}

		switch (state)
		{
		case Motor_Off:

			LED_Off();
			motor_stop();

			break;
		
		case Motor_On:

			LED_On();
			motor_start();

			break;
		}
	}
}