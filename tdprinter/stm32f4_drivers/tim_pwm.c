#include "tim_pwm.h"
#include <rtthread.h>

void pwm_init(pwm_t *p){
	switch(p->id){
		case 1:
			TIM_OC1Init(p->base,&p->conf);
			TIM_OC1PreloadConfig(p->base, TIM_OCPreload_Enable);
			break;
		case 2:
			TIM_OC2Init(p->base,&p->conf);
			TIM_OC2PreloadConfig(p->base, TIM_OCPreload_Enable);
			break;
		case 3:
			TIM_OC3Init(p->base,&p->conf);
			TIM_OC3PreloadConfig(p->base, TIM_OCPreload_Enable);
			break;
		case 4:
			TIM_OC4Init(p->base,&p->conf);
			TIM_OC4PreloadConfig(p->base, TIM_OCPreload_Enable);
			break;
		default:
			break;
	}
	gpio_init(&p->port,GPIO_Mode_AF);
	uint8_t ppwm=0;
	for(int i=0;i<16;i++){
		if(p->port.pin==(1<<i)){
			ppwm=i;
		}
	}
	GPIO_PinAFConfig(p->port.port,ppwm,p->af);
	p->topValue=p->base->ARR;
}

void pwm_set(pwm_t *p,float value){
	if(value>1.0f)value=1.0f;
	else if(value<0.0f)value=0.0f;
	uint16_t output=(uint16_t)(value*(float)(p->topValue));
	switch(p->id){
		case 1:
			TIM_SetCompare1(p->base,output);
			break;
		case 2:
			TIM_SetCompare2(p->base,output);
			break;
		case 3:
			TIM_SetCompare3(p->base,output);
			break;
		case 4:
			TIM_SetCompare4(p->base,output);
			break;
		default:
			break;
	}
}
