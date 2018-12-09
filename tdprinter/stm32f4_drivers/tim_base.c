#include "tim_base.h"

void tim_init(tim_base_t *p){
	(*p->rcc_init)(p->rcc,ENABLE);
	TIM_TimeBaseInit(p->base, &p->conf);
	TIM_ARRPreloadConfig(p->base, ENABLE);
	NVIC_Init(&p->nvic);
    //TIM_Cmd(p->base, ENABLE);
}
void tim_setEnable(tim_base_t *p,FunctionalState enable){
	TIM_Cmd(p->base,enable);
}
void tim_setITUpdate(tim_base_t *p,FunctionalState enable){
	TIM_ITConfig(p->base, TIM_IT_Update, enable);
}

void tim_setARRPreload(tim_base_t *p,bool enable){
	if(enable){
		TIM_ARRPreloadConfig(p->base, ENABLE);
	}else{
		TIM_ARRPreloadConfig(p->base, DISABLE);
	}
}
