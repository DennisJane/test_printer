#include "stm32f4xx.h"
#include "libStepMotor.h"

/*
void stepMotor_construct(stepMotor_t *s){
	for(int i=0;i<STEPPER_NUM;i++){
		a->relative[i]=false;
		a->motor[i]=defaultMotor[i];
	}
}*/
void stepMotor_init(stepMotor_t *s){
	gpio_rcc_init();
	gpio_init_spd(&s->en,GPIO_Mode_OUT,GPIO_Low_Speed);
	gpio_init_spd(&s->dir,GPIO_Mode_OUT,GPIO_Low_Speed);
	gpio_init_spd(&s->clk,GPIO_Mode_OUT,GPIO_Low_Speed);
}

