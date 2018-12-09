#include "lib_pid.h"

void pid_init(pid_t *pid,float kp,float ki,float kd,float imax,float omax,float hz){
	pid->kp=kp;
	pid->ki=ki;
	pid->kd=kd;
	pid->imax=imax;
	pid->omax=omax;
	pid->result=0;
	pid->sum=0;
	pid->derivative=0;
	pid->error0=0;
	lowpass_init(&pid->dfilter,hz);
}

void pid_reset(pid_t *pid,float error){
	lowpass_reset(&pid->dfilter,error);
}

/*
void pid_input(pid_t *pid,float error,float dt){
	pid->sum+=error * pid->ki * dt;
		 if(pid->sum >  pid->imax)pid->sum = pid->imax;
	else if(pid->sum < -pid->imax)pid->sum = -pid->imax;
	lowpass_input(&pid->dfilter,error-pid->error0,dt);
	pid->error0=error;
	pid->derivative=lowpass_output(&pid->dfilter,0)*pid->kd;
	pid->result=pid->kp*error+pid->sum+pid->derivative;
	if(pid->result>pid->omax)pid->result=pid->omax;
	if(pid->result<-pid->omax)pid->result=-pid->omax;
}*/

void pid_input(pid_t *pid,float error,float dt){
	lowpass_input(&pid->dfilter,error-pid->error0,dt);
	pid->error0=error;
	pid->derivative=lowpass_output(&pid->dfilter,0)*pid->kd;
	//TODO:单边最大值控制特化PID,D项只对增大起阻尼作用
	if(pid->derivative<0){
		pid->result=pid->kp*error + pid->derivative;
	}else{
		pid->result=pid->kp*error;
	}
	pid->sum+=pid->result * pid->ki * dt;
	if(pid->sum >  pid->imax)pid->sum = pid->imax;
	else if(pid->sum < -pid->imax)pid->sum = -pid->imax;
	pid->result=pid->result + pid->sum;
	if(pid->result>pid->omax)pid->result=pid->omax;
	if(pid->result<-pid->omax)pid->result=-pid->omax;
}

float pid_output(pid_t *pid,float *out){
	if(out){
		*out=pid->result;
	}
	return pid->result;
}







