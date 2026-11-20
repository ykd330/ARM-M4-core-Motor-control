# ARM-M4-core-Motor-control

MOTOR CONTROL PROJECT

1. 변수 설정
MAIN 
변수 이름 : state
크기 : 1B
역할 : motor의 on / off set | Event Handler
사용 위치 : main
범위 : Global
값 : 0 : off, 1 : on (enum State : Motor_Off, Motor_On)

변수 이름 : motor_state
크기 : 1B
역할 : motor의 회전 방향 제어 | Event Handler
사용 위치 : main
범위 : Global
값 : 0 : F, 1 : R (enum MotorState : Motor_Run_F, Motor_Run_R)

변수 이름 : uart_data
크기 : 1B (unsigned char)
역할 : USART통신을 통해 받은 문자를 저장 | Event Handler
사용 위치 : main(volatile), exeption.c(extern volatile)
범위 : Global
값 : 문자 및 숫자 / 유효값 : 'R', 'r', 'F', 'f', '1' ~ '9'

변수 이름 : dir_change
크기 : 1B
역할 : motor_state 변경에 따른 motor_reverse()의 동작 유무를 결정하기 위해 현재 motor_state의 상태를 저장하기 위한 변수
사용 위치 : main
범위 : local
값 : = motor_state, 10 : motor_state값 저장 설정 값

변수 이름 : duty_rate[9]
크기 : 9B ( 1B x 9 ) 
역할 : UART의 PWM 설정 입력 값인 1~9를 50% ~ 100% 값에 미리 설정해두기 위한 배열 변수 | Event Handler
사용 위치 : main
범위 : local
값 : 50, 60, 65, 70, 75, 80, 85, 90, 100

변수 이름 : button
크기 : 1B
역할 : GPIOC - PC13 스위치 Interrupt flag | Event Handler
사용 위치 : main(volatile), exception.c(extern volatile)
범위 : Global
값 : 0 : stable, 1 : flag

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
GPIOA -> MODER : (0x10 << 0) || (0x10 << 2)[Alternate function mode]으로 설정
Timer는 Repeat모드로 설정, ARR은 Pulse가 8MH주파수를 갖도록 설정, CCR은 DutyRate가 100%가 되도록 설정
TIM2->CR1 : (ARPE(7) : 0), (DIR(4) : 1(Down Counter 사용)), (OPM(3) : 0(Repeat mode)), (CEN(0) : 0(timer disable)) [0x1 << 4, 0x0 << 3]으로 설정
TIM2->SR : TIM2의 channel 1, 2는 PWM을 출력하기 위한 핀으로 Repeat mode에선 자동으로 TimeOut bit가 clear되므로 따로 설정할 필요가 없다.
TIM2->CCMR2 : 0x6868로 설정 (양 채널 비교기 preload, PWM, Output 설정)
TIM2->CCER : (0x01 << 4) || (0x01 << 0)로 설정


함수 : motor_rev()
역할 : 모터의 회전 방향을 바꾸기 위한 함수
사용 위치 : main
반환 값 : void
전달 인자 : motor_state
내부 구조 : 모터의 전달되고 있는 PWM의 채널을 변경하여 원래 핀에 전달되던 PWM pulse를 0으로 만들고 다른 핀에 PWM을 인가하여 모터의 방향을 전환한다.
전환 시 500ms의 딜레이를 주어 모터의 탈조 현상을 방지한다.
PWM Pin channel의 (A)CCxE값을 0(disable)시켜 출력을 0으로 바꿈 - Delay(500) - (B)CCxE값을 1(enable)시켜 PWM을 출력함
  

함수 : motor_run_r()
역할 : 모터의 역방향 회전을 설정하기 위한 함수
사용 위치 : main
반환 값 : void
전달 인자 : motor_state
내부 구조 : PinB에 PWM pulse, PinA에 Logic : 0 값을 인가하여 모터를 역방향으로 회전시킨다.
CC1E : 0, CC2E : 1

함수 : void motor_run_f()
역할 : 모터의 정방향 회전을 설정하기 위한 함수
사용 위치 : main
반환 값 : void
전달 인자 : motor_state
내부 구조 : PinA에 PWM pulse, PinB에 Logic : 0 값을 인가하여 모터를 정방향으로 회전시킨다.
CC1E : 1, CC2E : 0

Interrupt 설정