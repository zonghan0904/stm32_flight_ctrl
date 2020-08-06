#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "pwm.h"

void pwm_init()
{
	/* RCC initialization */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
        GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);

	/* GPIO initialization */
	GPIO_InitTypeDef GPIO_InitStruct = {
		.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_Speed = GPIO_Speed_100MHz,
		.GPIO_OType =GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* Timer initialization */
	// interface clock(42MHz) * 2 / ( Period(30000) * Prescaler(7) ) = 400Hz
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {
		.TIM_Period = 30000- 1,
		.TIM_Prescaler = 7 - 1,
		.TIM_CounterMode = TIM_CounterMode_Up
	};
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

	/* PWM output initialization */
	TIM_OCInitTypeDef TIM_OCInitStruct = {
		.TIM_OutputState = TIM_OutputState_Enable,
		.TIM_OCMode = TIM_OCMode_PWM1,
		.TIM_Pulse = 0
	};

	TIM_OC1Init(TIM4, &TIM_OCInitStruct);
        TIM_OC2Init(TIM4, &TIM_OCInitStruct);

	TIM_Cmd(TIM4, ENABLE);
}
