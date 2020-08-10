#include "pid_control.h"
#include "ahrs.h"
#include "motor.h"
#include "uart.h"
#include <stdio.h>
float past_error[100] = {0};
// motor1 left
// motor2 right
float error_sum = 0;

void pid_control(float data){	
	float bias = -4.0;
	data -= bias;
	float ideal = 0;
	float magic_para = 1.0;
	float error = 0;
	float left_signal = 12000;
	float right_signal = 12000;
	error = ideal - data;
	left_signal =  12000 + left_Base - magic_para*1.1*(error* gain_P + error_sum*gain_I + (error-past_error[49]) *gain_D);
	right_signal = 12000 + right_Base + magic_para*1.0*( error* gain_P + error_sum*gain_I + (error-past_error[49]) *gain_D);	
	right_signal <(12000+left_Base)? right_signal = (12000+right_Base):1;
	left_signal <(12000+right_Base)? left_signal = (12000+left_Base):1;
	set_motor_pwm_pulse(MOTOR1, (int)left_signal);
	set_motor_pwm_pulse(MOTOR2, (int)right_signal);
	char str[1000] = {};
	sprintf(str, "[siganl]  l: %f, r: %f ,data: %f\n\r", left_signal, right_signal,data);
	 //uart3_puts(str);
	int i;
	for(i=0;i<99;i++){
		past_error[i] = past_error[i+1];
	}
	past_error[99] = error;
	error_sum = error_sum - past_error[0] + past_error[99];
}

