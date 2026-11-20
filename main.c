#include "device_driver.h"
#include <stdio.h>
#define MOTOR_PIN_A 0
#define MOTOR_PIN_B 1
#define BUTTON_DELAY 2000
#define MOTTOR_STOP 750



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


void Main(void)
{
	Sys_Init(115200);
	printf("\nChattering 제거 방식 Key_Get_Pressed() 동작 시험\n");
	motor_Init(MOTOR_PIN_A, MOTOR_PIN_B);
	volatile int j;
	volatile long i;
	volatile unsigned long motor_speed[5] = {2000, 2500, 3000, 3500, 4000};
	motor_start(0x01);
	for (;;)
	{
		for (j = 0 ; j < 5; j++) {

			for (i = 0; i < 0x8000000; i++);
			motor_rev();
			for (i = 0; i < 0x8000000; i++);
			motor_set_speed(motor_speed[j]);
		}
	}
}