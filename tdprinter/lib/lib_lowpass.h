#pragma once

//lowpass
typedef struct{
	float f,fPI,a;
	float result;
}lowpass_t;

#define FACTOR2HZ(factor,dt) (1.0f/(dt*6.28f))

void lowpass_init(lowpass_t *f,float hz);
void lowpass_input(lowpass_t *f,float value,float dt);
float lowpass_output(lowpass_t *f,float *out);
void lowpass_setFL(lowpass_t *f,float hz);
void lowpass_reset(lowpass_t *f,float value);




