#include "stm32f4xx.h"
#include "option.h"
#include "macro.h"
#include "malloc.h"

// Uart.c
extern void Uart2_Init(int baud);
extern void Uart2_RX_Interrupt_Enable(int en);

//SysTick.c
extern void SysTick_Run(unsigned int msec);
extern int SysTick_Check_Timeout(void);
extern unsigned int SysTick_Get_Time(void);
extern unsigned int SysTick_Get_Load_Time(void);
extern void SysTick_Stop(void);

//Led.c
extern void LED_Init(void);
extern void LED_On(void);
extern void LED_Off(void);
extern void LED_Display(int on);

//Clock.c

extern void Clock_Init(void);

//Key.c
extern void Key_Poll_Init(void);
extern int Key_Get_Pressed(void);
extern void Key_ISR_Enable(int en);

//Timer.c
extern void TIM4_Repeat_Interrupt_Enable(int en);
extern void TIM3_Interrupt_Enable(int en, int time);
extern void TIM4_Interrupt_Enable(int en, int time);

//Motor.c
extern void motor_Init(int PinA, int PinB, int PinEN);
extern void motor_rev();
extern void motor_set_dir(unsigned char dir);
extern void motor_stop();
extern void motor_start();
extern void motor_set_speed(unsigned long motor_speed);
extern unsigned char get_motor_state();

// i2c.c

#define SC16IS752_IODIR				0x0A
#define SC16IS752_IOSTATE			0x0B

extern void I2C1_SC16IS752_Init(unsigned int freq);
extern void I2C1_SC16IS752_Write_Reg(unsigned int addr, unsigned int data);
extern void I2C1_SC16IS752_Config_GPIO(unsigned int config);
extern void I2C1_SC16IS752_Write_GPIO(unsigned int data);

// main.c
extern volatile char motor_PWM;
extern volatile unsigned char motor_start_interrupt;
extern volatile int Button;
extern volatile unsigned long state;
extern volatile char uart_data;
extern volatile char PWM_refresh;