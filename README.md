# ARM-M4-core-Motor-control

MOTOR CONTROL PROJECT

LED : On / Off 확인 용


1. 변수 설정
MAIN 
변수 이름 : state
크기 : 1B
역할 : motor의 on / off set | Event Handler
사용 위치 : main
범위 : Global
값 : 0 : off, 1 : on (enum State : Motor_Off, Motor_On)

변수 이름 : motor_state
크기 : 4B
역할 : motor의 회전 방향 제어 | Interrupt, Oper Handler
사용 위치 : main
범위 : Local
값 :  GPIOA->AFR[0]

변수 이름 : change
크기 : 1B
역할 : motor의 회전 방향 제어 | Event Handler
사용 위치 : main
범위 : Local
값 : 0 : F, 1 : R (enum MotorState : Motor_Run_F, Motor_Run_R)

변수 이름 : uart_data
크기 : 1B (unsigned char)
역할 : USART통신을 통해 받은 문자를 저장 | Event Handler
사용 위치 : main(volatile), exeption.c(extern volatile)
범위 : Local
값 : 문자 및 숫자 / 유효값 : 'R', 'r', 'F', 'f', '1' ~ '9'

변수 이름 : current_state
크기 : 1B
역할 : motor_state 변경에 따른 motor_reverse()의 동작 유무를 결정하기 위해 현재 motor_state의 상태를 저장하기 위한 변수
사용 위치 : main
범위 : Local
값 : = motor_state, 10 : motor_state값 저장 설정 값

변수 이름 : duty_rate[9]
크기 : 9B ( 1B x 9 ) 
역할 : UART의 PWM 설정 입력 값인 1~9를 50% ~ 100% 값에 미리 설정해두기 위한 배열 변수 | Event Handler
사용 위치 : main
범위 : Local
값 : 2000 ~ 4000

변수 이름 : Button
크기 : 1B
역할 : GPIOC - PC13 스위치 Interrupt flag | Event Handler
사용 위치 : main(volatile), exception.c(extern volatile)
범위 : Global
값 : 0 : stable, 1 : flag

변수 이름 : motor_speed
크기 : 4B
역할 : CCR 값 저장용
사용 위치 : main
범위 : Local
값 : 2000 ~ 4000


2. 함수 설정
MAIN

main



motor.c
함수 : motor_Init(int PinA, int PinB)
역할 : 모터와 연결된 GPIO와 모터의 기본 값 설정
사용 위치 : main
반환 값 : void
전달 인자 : MOTOR PIN(Pin A, Pin B)
내부 구조 : 입력받은 Pin의 clock과 Timer, Compare등 PWM을 위한 Register을 설정하여 Pin이 PWM을 출력할 수 있도록 준비한다.
사용할 핀은 GPIOA -> PA0, PA1 : 해당하는 핀의 Timer는 TIM2 : channel[1:0]
RCC -> APB1ENR : (0x1 << 0)으로 설정
GPIOA->AFR[0] : 0001 0001로 설정
GPIOA -> MODER : (0x10 << 0) || (0x10 << 2)[Alternate function mode]으로 설정
Timer는 Repeat모드로 설정, ARR은 Pulse가 8MH주파수를 갖도록 설정, CCR은 DutyRate가 100%가 되도록 설정
TIM2->SR : TIM2의 channel 1, 2는 PWM을 출력하기 위한 핀으로 Repeat mode에선 자동으로 TimeOut bit가 clear되므로 따로 설정할 필요가 없다.
TIM2->EGR : (0x1 << 0)로 egr 0bit 값 초기화
TIM2->ARR : MOTOR_ARR(800)로 설정 / 8000HZ 주파수 기준으로 모터에서 소음 발생
TIM1->PSC : MOTOR_PSC(((SYSCLK / MOTOR_FREQ) + 0.5) - 1)로 설정
TIM2->CCMR2 : 0x6868로 설정 (양 채널 비교기 preload, PWM, Output 설정)
TIM2->CCER : (TIM2->CCER & ~((0x1 << 5) | (0x1 << 1))) | ((0x1 << 4) | (0x1 << 0))로 설정
TIM2->CR1 : (ARPE(7) : 0), (DIR(4) : 1(Down Counter 사용)), (OPM(3) : 0(Repeat mode)), (CEN(0) : 1(timer enable)) [0x1 << 4, 0x1 << 0]으로 설정

함수 : motor_rev()
역할 : 모터의 회전 방향을 바꾸기 위한 함수
사용 위치 : main / Button의 눌린 시간을 검사하는 main logic 내부에서 실행
반환 값 : void
전달 인자 : void
내부 구조 : 
motor_state에  GPIOA->AFR[0]값을 저장하고 GPIOA->AFR[0]에 0을 대입한다.
TIM3를 초기화하고 실행한다.


함수 : motor_run_r()
역할 : 모터의 역방향 회전을 설정하기 위한 함수
사용 위치 : main
반환 값 : void
전달 인자 : motor_state
내부 구조 : GPIOA->AFR[0] 값을 0x10으로 대입하여 회전 방향을 역 방향으로 설정한다.

함수 : void motor_run_f()
역할 : 모터의 정방향 회전을 설정하기 위한 함수
사용 위치 : main
반환 값 : void
전달 인자 : void
내부 구조 : GPIOA->AFR[0] 값을 0x01로 대입하여 회전 방향을 정 방향으로 설정한다.

함수 : motor_set_speed(unsigned long speed)
역할 : 모터의 DutyRate을 바꾸어 속도를 조절하는 역할
사용 위치 : main
반환 값 : void
전달 인자 : duty_rate[] (2000 ~ 4000)
내부 구조 : TIM2->CCR1, TIM2->CCR2 값에 전달 받은 인자를 대입한다. 

Timer 설정
TIM2 : Ch.1, Ch.2 모터의 PWM출력
TIM3 : 내부 스위치의 눌린 시간이 2초 이내일 경우 모터의 반전을 수행하기 위해 모터의 방향 전환 전 모터의 정지를 위한 시간을 측정하는 Timer
-> 목표 시간 : 750ms
TIM4 : 내부 스위치의 눌린 시간을 측정하는 Timer
-> 목표 시간 : 2000ms

Interrupt 설정

TIM3_IRQHandler() (TIM3) (29)
 : TIM3 TimeOut 발생 시 호출
 1. GPIOA->AFR[0] = (Macro_Inverse_bit(motor_state, 0) | Macro_Inverse_bit(motor_state, 4));

TIM4_IRQHandler() (TIM 4) (30) 
 : TIM4 TimeOut 발생 시 호출
 1. !state : 모터의 on off 상태를 반전시킨다.
 2. if(state) - GPIOA->AFR[0] = motor_state;
 3. Button(flag) = 0으로 바꾼다.

EXTI15_10_IRQHandler() (PC13) (40)
 : PC13번 (내부 스위치)의 Falling Edge 발생 시 호출
 1. TIM4 Timer를 실행 시킨다.
 2. Button(flag) = 1로 바꾼다.

 USART2_IRQHandler() (USART2) ()
 : 터미널(teraterm)을 통해 문자를 입력 받으면 호출
 1. 문자를 입력 받으면 uart_data에 문자 입력
 2. TX를 통해 입력 받은 문자를 다시 터미널에 출력
 
