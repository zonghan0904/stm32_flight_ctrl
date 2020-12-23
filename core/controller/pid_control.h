#ifndef __PIDCONTORL_H__
#define __PIDCONTORL_H__

#define gain_P 50.0f
#define gain_I 0.0f
#define gain_D 0.0f
#define left_Base 4000
#define right_Base 4000

void pid_control(float,float);

#endif
