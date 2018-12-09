#pragma once

#include "lib_lowpass.h"

typedef struct{
	float kp,ki,kd,imax,omax;
	float result,sum,derivative,error0;
	lowpass_t dfilter;
}pid_t;

void pid_init(pid_t *pid,float kp,float ki,float kd,float imax,float omax,float hz);
void pid_input(pid_t *pid,float error,float dt);
float pid_output(pid_t *pid,float *out);
void pid_reset(pid_t *pid,float error);




