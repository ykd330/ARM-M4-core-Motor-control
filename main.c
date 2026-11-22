#include "device_driver.h"
#include <stdio.h>
#define MOTOR_PIN_A 0
#define MOTOR_PIN_B 1
#define MOTOR_EN 4

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
volatile char uart_data = 0;
volatile char motor_PWM = 8;
volatile char PWM_refresh = 0;

static void Sys_Init(int baud)
{
	SCB->CPACR |= (0x3 << 10 * 2) | (0x3 << 11 * 2);
	Clock_Init();
	Uart2_Init(baud);
	setvbuf(stdout, NULL, _IONBF, 0);
	LED_Init();
	Key_ISR_Enable(1);
	motor_Init(MOTOR_PIN_A, MOTOR_PIN_B, MOTOR_EN);
	Uart2_RX_Interrupt_Enable(1);
	I2C1_SC16IS752_Init(400000);
	I2C1_SC16IS752_Config_GPIO(0xFF);
}

void Main(void)
{
	Sys_Init(115200);
	printf("start motor test\n");
	motor_stop();
	motor_set_dir(Motor_Run_F);
	I2C1_SC16IS752_Write_GPIO(0xFF);
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
				motor_PWM = uart_data - '1';
				PWM_refresh = 1;
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
			I2C1_SC16IS752_Write_GPIO(0xFF);
			break;

		case Motor_On:

			LED_On();
			motor_start();
			motor_set_speed(motor_speed[(int)motor_PWM]);
			if (PWM_refresh)
			{
				if ((uart_data - '1') == 0)
					I2C1_SC16IS752_Write_GPIO(0xFF);
				else
					I2C1_SC16IS752_Write_GPIO(~(0x1 << (motor_PWM - 1)));

				PWM_refresh = 0;
			}
			break;
		}
	}
}