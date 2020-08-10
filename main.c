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

int main()
{
	pwm_init();           // control motor
	motor_init();
	uart3_init();
	spi1_init();          // imu communication
	mpu6500_init(&imu);   // mpu6500
	timer2_init();	      // imu raw data
	ahrs_init();          // estimator
	timer3_init();	      // madgwick
	timer5_init();	      // debug purpose

	int motor_speed = MOTOR_PULSE_MIN;

	while(1){
	}
	return 0;
}

