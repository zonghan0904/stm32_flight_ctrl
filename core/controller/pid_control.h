#ifndef __PIDCONTORL_H__
#define __PIDCONTORL_H__

#define gain_P 57.0f
#define gain_I 0.0f
#define gain_D 16.2f
#define left_Base 4000.f
#define right_Base 3500.f

void pid_control(float,float);

#endif
