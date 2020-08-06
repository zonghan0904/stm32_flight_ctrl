#include "stm32f4xx.h"
#include "uart.h"
#include "mpu6500.h"
#include "imu.h"
#include "spi.h"
#include <stdio.h>
#include <string.h>
#include "timer.h"
#include "ahrs.h"
#include "madgwick_ahrs.h"
#include "stm32f4xx_conf.h"
#include "pwm.h"
#include "motor.h"

imu_t imu;

/*
 * To test the frequency of timer
 */
void init_GPIO()
{
	GPIO_InitTypeDef GPIO_InitStruct = {
		.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,
		.GPIO_Mode = GPIO_Mode_OUT,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_OType =GPIO_OType_PP,
		.GPIO_PuPd = GPIO_PuPd_DOWN
	};

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_Init(GPIOD, &GPIO_InitStruct);
}

int main()
{
	init_GPIO();
	pwm_init();
	motor_init();
	uart3_init();
	spi1_init();          // imu communication
	mpu6500_init(&imu);   // mpu6500
	timer2_init();	      // imu raw data
	ahrs_init();          // estimator
	timer3_init();	      // madgwick
	timer5_init();	      // debug purpose
	
	int motor_speed = 12000;
	while(1){
		set_motor_pwm_pulse(MOTOR1,motor_speed);
		set_motor_pwm_pulse(MOTOR2,motor_speed);
		motor_speed+=1000;
		blocked_delay_ms(1000);
		if(motor_speed>24000){
			motor_speed = 12000;
			set_motor_pwm_pulse(MOTOR1,motor_speed);
			set_motor_pwm_pulse(MOTOR2,motor_speed);
		}
	}
}

