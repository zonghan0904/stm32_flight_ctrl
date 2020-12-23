#include "stm32f4xx_conf.h"
#include <string.h>
#include <stdio.h>
#include "uart.h"
#include "mpu6500.h"
#include "ahrs.h"
#include "pid_control.h"
ahrs_t ahrs;
float accel_lpf[3], gyro_raw[3];

/*
 * TIM2: Get IMU data
 * Frequency: 1000Hz
 */
void timer2_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* (42MHz -> interface clock) * 2 / (840 -> period * 100 -> prescaler) = 1000Hz */
	TIM_TimeBaseInitTypeDef TimeBaseInitStruct = {
		.TIM_Period = 840 - 1,
		.TIM_Prescaler = 100 - 1,
		.TIM_CounterMode = TIM_CounterMode_Up
	};
	TIM_TimeBaseInit(TIM2, &TimeBaseInitStruct);

	NVIC_InitTypeDef NVIC_InitStruct = {
		.NVIC_IRQChannel = TIM2_IRQn,
		.NVIC_IRQChannelPreemptionPriority = 0,
		.NVIC_IRQChannelCmd = ENABLE
	};
	NVIC_Init(&NVIC_InitStruct);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void){
    	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		mpu6500_int_handler();
		// debug_print_mpu6500_gyro();
    	}
}

/*
 * TIM3: Madgwick filter and PID controller
 * Frequency: 400Hz
 */
void timer3_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* (42MHz -> interface clock) * 2 / (8400 -> period * 25 -> prescaler) = 400Hz */
	TIM_TimeBaseInitTypeDef TimeBaseInitStruct = {
		.TIM_Period = 8400 - 1,
		.TIM_Prescaler = 25 - 1,
		.TIM_CounterMode = TIM_CounterMode_Up
	};
	TIM_TimeBaseInit(TIM3, &TimeBaseInitStruct);

	NVIC_InitTypeDef NVIC_InitStruct = {
		.NVIC_IRQChannel = TIM3_IRQn,
		.NVIC_IRQChannelPreemptionPriority = 1,
		.NVIC_IRQChannelCmd = ENABLE
	};
	NVIC_Init(&NVIC_InitStruct);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void){
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		mpu6500_get_filtered_accel(accel_lpf);
		mpu6500_get_raw_gyro(gyro_raw);
		ahrs_estimate(&ahrs, accel_lpf, gyro_raw);
		pid_control(ahrs.attitude.roll,gyro_raw[0]);
	}
}

/*
 * TIM5: Uart (debug purpose)
 * Frequency: 8Hz
 */
void timer5_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* (42MHz -> interface clock) * 2 / (84000 -> period * 125 -> prescaler) = 8Hz */
	TIM_TimeBaseInitTypeDef TimeBaseInitStruct = {
		.TIM_Period = 84000 - 1,
		.TIM_Prescaler = 125 - 1,
		.TIM_CounterMode = TIM_CounterMode_Up
	};
	TIM_TimeBaseInit(TIM5, &TimeBaseInitStruct);

	NVIC_InitTypeDef NVIC_InitStruct = {
		.NVIC_IRQChannel = TIM5_IRQn,
		.NVIC_IRQChannelPreemptionPriority = 2,
		.NVIC_IRQChannelCmd = ENABLE
	};
	NVIC_Init(&NVIC_InitStruct);

	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
}

void TIM5_IRQHandler(void){
	static int cnt = 0;
    	char str[1000] = {};
    	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
		sprintf(str, "[ahrs %d]  roll: %f, pitch: %f, yaw: %f\n\r", cnt++,ahrs.attitude.roll, ahrs.attitude.pitch, ahrs.attitude.yaw);
//		uart3_puts(str);
    	}
}

