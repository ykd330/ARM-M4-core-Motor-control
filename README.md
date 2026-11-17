# ARM-M4-core-Motor-control

MOTOR CONTROL PROJECT

1. 변수 설정
MAIN 
변수 이름 : state
역할 : motor의 on / off set
사용 위치 : main
범위 : local
값 : 0 : off, 1 : on

변수 이름 : motor_state
역할 : motor dir control
사용 위치 : main
범위 : local
값 : 0 : F, 1 : R

변수 이름 : lock
역할 : 버튼을 이용한 state 변경 시 연속적인 입력을 막으며 다른 실행 요소에 영향을 주지 않게 하기 위한 상태 표시 변수
사용 위치 : main
범위 : local
값 : 0 : unlock, 1 : lock

변수 이름 : dir_change
역할 : motor_state 변경에 따른 motor_reverse()의 동작 유무를 결정하기 위해 현재 motor_state의 상태를 저장하기 위한 변수
사용 위치 : main
범위 : local
값 : =motor_state, 10 : motor_state값 저장 설정 값

motor.h
변수 이름 : current
역할 : motor_rev()에서 현재 모터 핀의 GPIO->ODR값을 저장하기 위한 변수
모터의 완전 정지 이후 모터를 반대로 작동시키기 위해 ~연산으로 값 반전
사용 위치 : motor_rev()
범위 : local
값 : 0x01 or 0x10 (current >> pinx) & 0x1로 값 추출


2. 함수 설정
MAIN

motor.h
함수 : void motor_Init(int PinA, int PinB)
역할 : 모터와 연결된 GPIO와 모터의 기본 값 설정
사용 위치 : main
void motor_rev()
void motor_runr()
void motor_runf()


key.h

led.h

clock.h

usart.h

sys.h


