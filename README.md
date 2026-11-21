# ARM-M4-core-Motor-control

MOTOR CONTROL PROJECT

---

## 목차

1. [변수 설정](#1-변수-설정)
   - [MAIN](#main)
   - [motor.c](#motorc)
2. [함수 설정](#2-함수-설정)
   - [MAIN](#main-1)
   - [motor.c](#motorc-1)
3. [Timer 설정](#3-timer-설정)
4. [Interrupt 설정 - exception.c](#4-interrupt-설정---exceptionc)

---

## 1. 변수 설정

### MAIN

* **`state`**
  * **크기:** 1B
  * **역할:** motor의 on / off set | Event Handler
  * **사용 위치:** `main.c` (`volatile`), `exception.c` (`device_driver`: `extern volatile`)
  * **범위:** `Global`
  * **값:** `0` : off, `1` : on (`enum State` : `Motor_Off`, `Motor_On`)

* **`uart_data`**
  * **크기:** 1B (`unsigned char`)
  * **역할:** USART통신을 통해 받은 문자를 저장 | Event Handler
  * **사용 위치:** `main` (`volatile`), `exeption.c` (`extern volatile`)
  * **범위:** `Local`
  * **값:** 문자 및 숫자 / 유효값 : `'R'`, `'r'`, `'F'`, `'f'`, `'1'` ~ `'9'`

* **`duty_rate[9]`**
  * **크기:** 9B (`1B x 9`)
  * **역할:** UART의 PWM 설정 입력 값인 1~9를 50% ~ 100% 값에 미리 설정해두기 위한 배열 변수 | Event Handler
  * **사용 위치:** `main`
  * **범위:** `Local`
  * **값:** `2000` ~ `4000`

* **`Button`**
  * **크기:** 1B
  * **역할:** GPIOC - PC13 스위치 Interrupt flag | Event Handler
  * **사용 위치:** `main` (`volatile`), `exception.c` (`extern volatile`)
  * **범위:** `Global`
  * **값:** `0` : stable, `1` : flag

* **`motor_speed`**
  * **크기:** 4B
  * **역할:** CCR 값 저장용
  * **사용 위치:** `main`
  * **범위:** `Local`
  * **값:** `2000` ~ `4000`, '[2000, 2400, 2600, 2800, 3000, 3200, 3400, 3600, 4000]'

### `motor.c`

* **`motor_state`**
  * **크기:** 4B
  * **역할:** motor의 회전 방향 제어 | Interrupt, Oper Handler
  * **사용 위치:** `motor.c`
  * **범위:** `Global`
  * **값:** `0x01`, `0x10` (`enum Motor_State` : `Motor_run_F`, `Motor_run_R`)

* **`motor_start_interrupt`**
  * **크기:** 1B
  * **역할:** 모터의 방향 전환을 위한 start lock용 변수
  * **사용 위치:** `motor.c` (`volatile`), `exception.c` (`device_driver` : `extern volatile`)
  * **범위:** `Global`
  * **값:** `0` (lock), `1` (unlock)

---

## 2. 함수 설정

### MAIN

#### `main`

* **Setup**
  * `#define MOTOR_PIN_A 0` : GPIOA의 모터핀 (`PA0`)
  * `#define MOTOR_PIN_B 1` : GPIOA의 모터핀 (`PA1`)
  * `enum Motor_State { Motor_Run_F = 0x01, Motor_Run_R = 0x10 }`
  * `enum State { Motor_Off, Motor_On }`
  * **Local var:** `motor_speed[9] = 2000, 2400, 2600, 2800, 3000, 3200, 3400, 3600, 4000` (PWM 50% ~ 100%)

* **`main.c`의 동작 (`for(;;)` 기준)**
  * **Event**
    1. `uart_data`가 입력되면 이를 감지하여 유효 값인지 검사하고 유효 값일 경우 Handler(`state`)의 값을 조절하여 작동부에 전달한다.
    2. `Button` 입력 처리
       * **2-1.** Button의 입력이 감지되면 Interrupt(PC13 : 40)와 `Button = 1` 코드가 실행되고 약 2초의 Timer(TIM4)를 설정하여 시간을 측정한다.
       * **2-2.** Time Out이 발생하면 즉시 `Button` 변수에 `0`이 대입되고 `state`가 `0` or `1`로 원래 상태에서 반전된다.
       * **2-3.** Time Out이 발생하기 전 버튼에서 손을 때면 (`PC13 IDR == 1`) TIM4는 즉시 정지하고 `Button` 함수에 `0`을 대입한 후 `motor_rev()` 함수를 실행한다.
  * **Operator**
    1. `state` 변수의 값 (on, off)에 따라 LED와 motor의 작동 유무를 결정한다.

---

### `motor.c`

* **`motor_Init(int PinA, int PinB)`**
  * **역할:** 모터와 연결된 GPIO와 모터의 기본 값 설정
  * **사용 위치:** `main`
  * **반환 값:** `void`
  * **전달 인자:** MOTOR PIN (`Pin A`, `Pin B`)
  * **내부 구조:** 입력받은 Pin의 clock과 Timer, Compare 등 PWM을 위한 Register을 설정하여 Pin이 PWM을 출력할 수 있도록 준비한다.
    * 사용할 핀은 `GPIOA` -> `PA0`, `PA1` : 해당하는 핀의 Timer는 `TIM2` : `channel[1:0]`
    * `RCC -> APB1ENR` : `(0x1 << 0)`으로 설정
    * `GPIOA->AFR[0]` : `0001 0001`로 설정
    * `GPIOA -> MODER` : `(0x10 << 0) || (0x10 << 2)` [Alternate function mode]으로 설정
    * Timer는 Repeat 모드로 설정, ARR은 Pulse가 8MH 주파수를 갖도록 설정, CCR은 DutyRate가 100%가 되도록 설정
    * `TIM2->SR` : TIM2의 channel 1, 2는 PWM을 출력하기 위한 핀으로 Repeat mode에선 자동으로 TimeOut bit가 clear되므로 따로 설정할 필요가 없다.
    * `TIM2->EGR` : `(0x1 << 0)`로 egr 0bit 값 초기화
    * `TIM2->ARR` : `MOTOR_ARR(800)`로 설정 / 8000HZ 주파수 기준으로 모터에서 소음 발생
    * `TIM1->PSC` : `MOTOR_PSC(((SYSCLK / MOTOR_FREQ) + 0.5) - 1)`로 설정
    * `TIM2->CCMR2` : `0x6868`로 설정 (양 채널 비교기 preload, PWM, Output 설정)
    * `TIM2->CCER` : `(TIM2->CCER & ~((0x1 << 5) | (0x1 << 1))) | ((0x1 << 4) | (0x1 << 0))`로 설정
    * `TIM2->CR1` : `(ARPE(7) : 0)`, `(DIR(4) : 1(Down Counter 사용))`, `(OPM(3) : 0(Repeat mode))`, `(CEN(0) : 1(timer enable))` `[0x1 << 4, 0x1 << 0]`으로 설정

* **`motor_rev()`**
  * **역할:** 모터의 회전 방향을 바꾸기 위한 함수
  * **사용 위치:** `main` / Button의 눌린 시간을 검사하는 main logic 내부에서 실행
  * **반환 값:** `void`
  * **전달 인자:** `void`
  * **내부 구조:**
    * `GPIOA->AFR[0]`을 `0`으로 대입해 PWM 출력을 멈춘다.
    * `motor_state`의 0번 째와 4번 째 bit를 반전시킨다.
    * `motor_start_interrupt`에 `1`을 대입하여 모터의 작동을 금지한다.
    * `TIM3_Interrupt_Enable(1, 1000)` 함수를 실행한다.

* **`motor_set_dir()`**
  * **역할:** 모터의 회전방향을 설정하기 위한 함수
  * **사용 위치:** `main`
  * **반환 값:** `void`
  * **전달 인자:** `(unsigned char dir)`
  * **내부 구조:**
    * `motor_state`와 입력받은 값 `dir`을 비교한다. 같으면 return
    * `GPIOA->AFR[0]`에 `0`을 대입하여 모터를 일시 정지시킨다.
    * 입력 받은 `dir`을 `motor_state`에 대입한다.
    * `motor_start_interrupt`에 `1`을 대입하여 모터의 작동을 금지한다.
    * `TIM3_Interrupt_Enable(1, 1000)` 함수를 실행한다.

* **`motor_set_speed()`**
  * **역할:** 모터의 DutyRate을 바꾸어 속도를 조절하는 역할
  * **사용 위치:** `main`
  * **반환 값:** `void`
  * **전달 인자:** `(unsigned long motor_speed)` [`duty_rate[]`] (`2000` ~ `4000`)
  * **내부 구조:** `TIM2->CCR1`, `TIM2->CCR2` 값에 전달 받은 인자를 대입한다.

* **`get_motor_state()`**
  * **역할:** 모터의 현재 회전 방향을 반환하는 함수
  * **사용 위치:** `main`
  * **반환 값:** `unsigned char`
  * **전달 인자:** `void`
  * **내부 구조:**
    * 함수 호출 시 현재 `GPIOA->AFR[0]`의 값을 반환한다.

* **`motor.c`의 동작**
  * 파일 내부에 `motor_state` 변수를 선언하여 `GPIOA->ARF[0]`의 값과 구분한다.
  * `motor_start_interrupt` 변수를 선언하여 방향 전환과 관련된 함수가 실행 중일 때 모터의 동작을 금지한다.
  * **모터의 동작에 직접적으로 관여하는 함수:** `motor_start()` , `motor_stop()`
  * **모터의 방향에 관여하는 함수:** `motor_set_dir(unsigned char dir)` , `motor_rec()`
  * **모터의 속도에 관여하는 함수:** `motor_set_speed(unsigned long motor_speed)`
  * **모터의 상태를 확인하는 함수:** `get_motor_state()` [return : `unsigned char`]

---

## 3. Timer 설정

* **TIM2:** Ch.1, Ch.2 모터의 PWM 출력
* **TIM3:** 모터의 안전한 방향 전환을 위한 Timer
  * 목표 시간 : `1000ms`
* **TIM4:** 내부 스위치의 눌린 시간을 측정하는 Timer
  * 목표 시간 : `2000ms`

---

## 4. Interrupt 설정 - `exception.c`

* **`TIM3_IRQHandler()` (TIM3) (29)**
  * TIM3 TimeOut 발생 시 호출
  * 1. `motor_start_interrupt`에 `0`을 대입하여 모터의 작동 금지를 해제한다.

* **`TIM4_IRQHandler()` (TIM 4) (30)**
  * TIM4 TimeOut 발생 시 호출
  * 1. `!state` : 모터의 on off 상태를 반전시킨다.
  * 2. `Button`에 `0`을 대입한다.

* **`EXTI15_10_IRQHandler()` (PC13) (40)**
  * PC13번 (내부 스위치)의 Falling Edge 발생 시 호출
  * 1. TIM4 Timer를 실행 시킨다.
  * 2. `Button`(flag) = `1`로 바꾼다.

* **`USART2_IRQHandler()` (USART2) (38)**
  * 터미널(teraterm)을 통해 문자를 입력 받으면 호출
  * 1. 문자를 입력 받으면 `uart_data`에 문자 입력
  * 2. TX를 통해 입력 받은 문자를 다시 터미널에 출력
