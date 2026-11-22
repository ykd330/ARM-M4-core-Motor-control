# ARM-M4-core-Motor-control

## 온디바이스AISW개발2기 ARM core M4 Board / STM32 
- 팀장 : `유경도`
- 팀원 : `전민수 조혜원 한성현` 
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
5. [기존 드라이버/예제 함수 목록](#5-기존-드라이버예제-함수-목록)

---

## 구현 목표

### 기본 구현
1. 내부 스위치(PC13)를 약 2초간 누르면 모터가 On / Off 전환됨
2. 내부 스위치를 2초 이내로 누르고 떼면 모터의 회전 방향이 바뀜
3. 시리얼 터미널을 통해 `F`/`f`, `R`/`r`, `1` ~ `9`, `S`/`s` 입력 시 방향 전환, 속도 조절, On/Off 제어 수행

### 추가 구현
1. 외부 스위치(PB1, PB2) 인터럽트로 모터 속도를 단계별로 조절
2. `BUS_EXT` 보드와 I2C1 통신(`SC16IS752`)을 통해 보드의 8개 LED로 현재 모터 속도 표시

---

## 1. 변수 설정

### MAIN

- **`state`**
  - **크기:** 4B (`unsigned long`, `enum State`)
  - **역할:** 모터 On / Off 상태 저장 (`Event Handler`)
  - **사용 위치:** `main.c` (`volatile`), `exception.c` (`device_driver.h`: `extern volatile`)
  - **범위:** `Global`
  - **값:** `0` (`Motor_Off`), `1` (`Motor_On`)

- **`uart_data`**
  - **크기:** 1B (`unsigned char`)
  - **역할:** USART2 수신 문자를 저장 (`Event Handler`)
  - **사용 위치:** `main.c`, `exception.c` (`extern volatile`)
  - **범위:** `Global`
  - **값:** 수신 문자 / 유효값: `'R'`, `'r'`, `'F'`, `'f'`, `'1'` ~ `'9'`

- **`motor_speed[9]`**
  - **크기:** 36B (`4B x 9`, `const unsigned long`)
  - **역할:** `motor_PWM` 인덱스(`0 ~ 8`)를 실제 PWM `CCR` 값으로 매핑 (50% ~ 100% 수준)
  - **사용 위치:** `main.c`
  - **범위:** `Global` (`const`)
  - **값:** `{2000, 2400, 2600, 2800, 3000, 3200, 3400, 3600, 4000}`

- **`Button`**
  - **크기:** 4B (`int`)
  - **역할:** `GPIOC` `PC13` 스위치 인터럽트 플래그 (`Event Handler`)
  - **사용 위치:** `main.c`, `exception.c` (`extern volatile`)
  - **범위:** `Global`
  - **값:** `0` (정상/대기), `1` (눌림 감지 플래그)

- **`motor_PWM`**
  - **크기:** 1B (`char` / `unsigned char`)
  - **역할:** `motor_speed` 배열 인덱스 저장
  - **사용 위치:** `main.c`, `exception.c` (`extern volatile`)
  - **범위:** `Global`
  - **값:** `0 ~ 8` (초기값 `8`)

- **`PWM_refresh`**
  - **크기:** 4B (`int` 등)
  - **역할:** `BUS_EXT` 보드의 LED(`motor_PWM`) 갱신 요청 플래그 (`Event Handler`)
  - **사용 위치:** `main.c`, `exception.c` (`extern volatile`)
  - **범위:** `Global`
  - **값:** `0`, `1`

### `motor.c`

- **`A`**
  - **크기:** 4B (`int`)
  - **역할:** 모터 제어 `Pin A` 번호 저장 및 내부 공유
  - **사용 위치:** `motor.c`
  - **범위:** `Global` (`static`)
  - **값:** `MOTOR_PIN_A` (`PA0` 기준 `0`)

- **`B`**
  - **크기:** 4B (`int`)
  - **역할:** 모터 제어 `Pin B` 번호 저장 및 내부 공유
  - **사용 위치:** `motor.c`
  - **범위:** `Global` (`static`)
  - **값:** `MOTOR_PIN_B` (`PA1` 기준 `1`)

- **`EN`**
  - **크기:** 4B (`int`)
  - **역할:** 모터 드라이버 Enable 핀 번호 저장 및 내부 공유
  - **사용 위치:** `motor.c`
  - **범위:** `Global` (`static`)
  - **값:** `MOTOR_EN` (`PA4` 기준 `4`)

- **`motor_start_interrupt`**
  - **크기:** 1B (`volatile` 등)
  - **역할:** 방향 전환 중 모터 구동을 일시 잠금(Lock)하는 플래그
  - **사용 위치:** `motor.c`, `exception.c` (`extern volatile`)
  - **범위:** `Global`
  - **값:** `0` (해제/허용), `1` (잠금/금지)

---

## 2. 함수 설정

### MAIN

#### `Sys_Init(int baud)` & `Main(void)`

- **초기화 (`Sys_Init`)**
  - `SCB->CPACR`: FPU(CP10, CP11) 접근 활성화
  - `Clock_Init()`: 시스템 클럭 및 주변장치 클럭 설정
  - `Uart2_Init(baud)`: USART2 통신 레지스터 설정 (`PA2`/`PA3`, AF7)
  - `LED_Init()`: `PA5` 내부 LED 핀 출력 설정
  - `Key_ISR_Enable(1)`: `PC13`, `PB1`, `PB2` 외부 인터럽트(`EXTI`) 설정
  - `motor_Init(MOTOR_PIN_A, MOTOR_PIN_B, MOTOR_EN)`: 모터 제어 핀 및 `TIM2` PWM 설정
  - `Uart2_RX_Interrupt_Enable(1)`: USART2 수신(`RXNE`) 인터럽트 활성화
  - `I2C1_SC16IS752_Init(400000)` 및 `I2C1_SC16IS752_Config_GPIO(...)`: I2C1 및 `SC16IS752` GPIO 초기화

- **상수 및 열거형 (`Setup`)**
  - `#define MOTOR_PIN_A 0` (`PA0`)
  - `#define MOTOR_PIN_B 1` (`PA1`)
  - `#define MOTOR_EN 4` (`PA4`)
  - `enum Motor_State { Motor_Run_F = 0x01, Motor_Run_R = 0x10 }`
  - `enum State { Motor_Off, Motor_On }`

- **메인 루프 (`for (;;)` 동작)**
  1. **`uart_data` 입력 처리**
     - `'R'` / `'r'`: `motor_set_dir(Motor_Run_R)` 호출
     - `'F'` / `'f'`: `motor_set_dir(Motor_Run_F)` 호출
     - `'1'` ~ `'9'`: `motor_PWM = uart_data - '1'`로 설정하고 `PWM_refresh = 1` 설정 (또는 I2C GPIO 출력 갱신)
     - `'S'` / `'s'`: `state = !state`로 On/Off 토글
     - 처리 후 `uart_data = 0`으로 초기화
  2. **`Button` 입력 처리**
     - `EXTI15_10_IRQHandler`에서 `Button = 1` 및 `TIM4` 시작
     - 타임아웃(약 2초) 전 버튼에서 손을 떼면(`Button && !Key_Get_Pressed()`) `TIM4`를 정지하고 `Button = 0` 후 `motor_rev()` 호출
     - 타임아웃 발생 시 자동으로 `Button = 0`과 `state`가 반전됨
  3. **상태 출력 (`switch (state)`)**
     - `Motor_Off`: `LED_Off()`, `motor_stop()` 호출
     - `Motor_On`: `LED_On()`, `motor_start()`, `motor_set_speed(motor_speed[motor_PWM])` 호출 및 속도 변경 시 `I2C1_SC16IS752_Write_GPIO(...)`로 `BUS_EXT` LED 표시 갱신

-  **UART**
  1. **`UART` 동작**
     - `Terminel`에 데이터 입력 시 `USART2_IRQHandler`가 호출됨
     - `USART2->DR`의 데이터가 `uart_data`에 저장됨
     - `USART2->SR`을 통해 `TX Buffer`가 빌 때 까지 대기
     - `TX Buffer`에 `uart_data`를 넣어 `Terminel`로 재전송

### `motor.c`
- **`define`**
  - `MOTOR_FREQ` : `8000000`
  - `MOTOR_PSC` : `(int)(((SYSCLK / MOTOR_FREQ) + 0.5)) - 1` : `PSC` = `8`
  - `MOTOR_ARR` : `4000` : 테스트 결과 `ARR = 8000` 부근에서 모터에 소음이 발생함

- **`motor_Init(int PinA, int PinB, int PinEN)`**
  - `PA0`, `PA1` Alternate Function(AF1, `TIM2` Ch.1/Ch.2) 및 `PA4` 출력 설정
  - `TIM2` Repeat 모드, `ARR`, `PSC`, `CCMR`, `CCER`, `CR1` 설정 및 `A`, `B`, `EN` 저장

- **`motor_rev(void)`**
  - 드라이버 비활성화 후 회전 방향 핀/AF 설정 반전
  - `motor_start_interrupt = 1` 설정 후 `TIM3_Interrupt_Enable(1, 1000)` 호출

- **`motor_set_dir(unsigned char dir)`**
  - 현재 방향과 다를 경우 드라이버 일시 정지 및 방향 설정 반영
  - `motor_start_interrupt = 1` 설정 후 `TIM3_Interrupt_Enable(1, 1000)` 호출

- **`motor_set_speed(unsigned long motor_speed)`**
  - 전달받은 듀티/비교 값으로 `TIM2` 채널의 `CCR` 레지스터 갱신

- **`get_motor_state(void)`**
  - 현재 방향 설정 상태(`unsigned char`) 반환

- **`motor_stop(void)`**
  - `GPIOA->ODR`의 `EN` 비트를 `0`으로 설정해 모터 출력 비활성화

- **`motor_start(void)`**
  - `motor_start_interrupt == 1`이면 즉시 반환하고, 아니면 `GPIOA->ODR`의 `EN` 비트를 `1`로 설정해 모터 출력 활성화

---

## 3. Timer 설정
- **`define`**
  - `TIM4_TICK` : `20us`
  - `TIM4_FREQ` : `1000000` / `TIM4_TICK` = `50000Hz`
  - `TIME4_PLS_OF_1ms` : `1000` / `TIM4_TICK` = `50EA`
  - `TIM_TICK` : `100us`
  - `TIM_FREQ` : `1000000` / `TIM_TICK` = `10000Hz`
  - `TIM_PLS_OF_1ms` : `1000` / `TIM_TICK` = `10EA`

- **TIM2:** Ch.1, Ch.2 모터 PWM 출력
- **TIM3:** 모터 방향 전환 안전 대기 타이머 (`1000ms`)
- **TIM4:** 내부 스위치(PC13) 누름 시간 측정 타이머 (`2000ms`)

---

## 4. Interrupt 설정 - `exception.c`

- **`TIM3_IRQHandler()` (IRQ 29):** 타임아웃 시 `motor_start_interrupt = 0` 및 `TIM3` 비활성화
- **`TIM4_IRQHandler()` (IRQ 30):** 타임아웃 시 `TIM4` 정지, `Button = 0`, `state = !state` 반전
- **`EXTI15_10_IRQHandler()` (PC13, IRQ 40):** Falling Edge 시 `Button = 1` 및 `TIM4` 시작
- **`USART2_IRQHandler()` (USART2, IRQ 38):** 문자 수신 시 `uart_data` 저장 후 `TXE` 플래그 확인 및 에코 송신
- **`EXTI1_IRQHandler()` (PB1, IRQ 7):** Falling Edge 시 `motor_PWM < 8`이면 `1` 증가, `PWM_refresh = 1` 설정
- **`EXTI2_IRQHandler()` (PB2, IRQ 8):** Falling Edge 시 `motor_PWM > 0`이면 `1` 감소, `PWM_refresh = 1` 설정

---

## 5. 기존 드라이버/예제 함수 목록

- **`led.c`:** `LED_Init(void)`, `LED_On(void)`, `LED_Off(void)`
- **`key.c`:** `Key_Poll_Init(void)`, `Key_Get_Pressed(void)`, `Key_ISR_Enable(int en)`
- **`timer.c`:** `TIM3_Interrupt_Enable(...)`, `TIM4_Repeat_Interrupt_Enable(int en)`
- **`uart.c`:** `Uart2_Init(int baud)`, `Uart2_RX_Interrupt_Enable(int en)`
- **`i2c_2.c`:** `I2C1_SC16IS752_Init(unsigned int freq)`, `I2C1_SC16IS752_Config_GPIO(unsigned int config)`, `I2C1_SC16IS752_Write_GPIO(unsigned int data)`
