#ifndef __PIDCONTORL_H__
#define __PIDCONTORL_H__

#define gain_P 30.5f
#define gain_I 0.105f
#define gain_D 72.7f
#define left_Base 4000.f
#define right_Base 3450.f

void pid_control(float);

#endif
