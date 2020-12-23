#include "pid_control.h"
#include "ahrs.h"
#include "motor.h"
#include "uart.h"
#include <stdio.h>
#include "ahrs.h"

// motor1 left
// motor2 right
// KP 30.5f
// KI 0.105f
// KD 72.7f
// left_Base 4000.f
// right_Base 3450.f

float error_sum = 0;
float past_error[100] = {0};
int count_print = 0;
void pid_control(float data,float ang_vel){
	ang_vel = - ang_vel;		//angular velocity
	float bias = 0;				//bias 
	data -= bias;
	float ideal = 0;			//ideal angle
	float error = 0;			//error
	float left_signal = 12000;	
	float right_signal = 12000;

	// PID control
	error = ideal - data;


/** write your code here **/
	left_signal =  12000 + left_Base + (error * gain_P + error_sum * gain_I + ang_vel * gain_D);
	right_signal = 12000 + left_Base - (error * gain_P + error_sum * gain_I + ang_vel * gain_D);


/**                      **/


	left_signal < (12000) ? left_signal = (12000) : 1;
	right_signal < (12000) ? right_signal = (12000) : 1;
	left_signal > (24000) ? left_signal = (24000) : 1;
	right_signal > (24000) ? right_signal = (24000) : 1;
	set_motor_pwm_pulse(MOTOR1, (int)left_signal);
	set_motor_pwm_pulse(MOTOR2, (int)right_signal);
	count_print++;
	if(count_print>10){
	 char str[1000] = {};
	 //sprintf(str, "[siganl]  l: %f, r: %f, data: %f \n\r", left_signal, right_signal, ang_vel);
	 sprintf(str, "[siganl]  data: %f left: %f right: %f\n\r", data ,left_signal,right_signal);
//	 uart3_puts(str);
	count_print = 0;
	}
	int i;
	for(i=0;i<99;i++){
		past_error[i] = past_error[i+1];
	}
	past_error[99] = error;
	error_sum = error_sum  + past_error[99] - past_error[0] ;
}

