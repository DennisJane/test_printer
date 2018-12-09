#include "lib_lowpass.h"

//LOWPASS
void lowpass_init(lowpass_t *f,float hz){
	f->f=hz;
	f->fPI=f->f*6.18f;
	f->a=0;
	f->result=0;
}

void lowpass_input(lowpass_t *f,float value,float dt){
	f->a = f->fPI * dt;	//a=fL*2*PI*dt; <= fL=a/(2*PI*dt)
	if(f->a > 1.0f)f->a=1.0f;
	f->result = f->result + f->a * (value - f->result);
}
void lowpass_reset(lowpass_t *f,float value){
	f->result=value;
}

float lowpass_output(lowpass_t *f,float *out){
	if(out)*out=f->result;
	return f->result;
}

void lowpass_setFL(lowpass_t *f,float hz){
	f->f=hz;
	f->fPI=f->f*6.28f;
}

