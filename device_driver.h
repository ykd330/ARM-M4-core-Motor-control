#include "stm32f4xx.h"
#include "option.h"
#include "macro.h"
#include "malloc.h"

// Uart.c

extern void Uart2_Init(int baud);
extern void Uart1_Init(int baud);
extern void Uart1_Send_Byte(char data);
extern char Uart1_Get_Char(void);
extern char Uart1_Get_Pressed(void);
extern void Uart2_RX_Interrupt_Enable(int en);

// SysTick.c

extern void SysTick_Run(unsigned int msec);
extern int SysTick_Check_Timeout(void);
extern unsigned int SysTick_Get_Time(void);
extern unsigned int SysTick_Get_Load_Time(void);
extern void SysTick_Stop(void);

// Led.c

extern void LED_Init(void);
extern void LED_On(void);
extern void LED_Off(void);
extern void LED_Display(int on);

// Clock.c

extern void Clock_Init(void);

// Key.c

extern void Key_Poll_Init(void);
extern int Key_Get_Pressed(void);
extern void Key_Wait_Key_Released(void);
extern void Key_Wait_Key_Pressed(void);
extern void Key_ISR_Enable(int en);

// Timer.c

extern void TIM4_Repeat(int time);
extern int TIM4_Check_Timeout(void);
extern void TIM4_Stop(void);
extern void TIM4_Change_Value(int time);
extern void TIM3_Out_Init(void);
extern void TIM3_Out_Freq_Generation(unsigned short freq);
extern void TIM3_Out_Stop(void);
extern void TIM3_Interrupt_Enable(int en, int time);
extern void TIM4_Interrupt_Enable(int en, int time);

// Motor.c
extern void motor_Init(int PinA, int PinB);
extern void motor_rev();
extern void motor_run_r();
extern void motor_run_f();
extern void motor_stop();
extern void motor_start(char dir);
extern void motor_set_speed(unsigned long motor_speed);
extern unsigned char get_motor_state();

extern volatile unsigned char motor_state;
extern volatile int Button;
extern volatile unsigned long state;
extern volatile unsigned char uart_data;