#pragma once

#include "stm32f4_drivers/tim1_timer.h"

//定时器时钟频率24M
#define TIMER_CLK 24000000.0f
#define TIMER_CLK_DIV_50 480000.0f

#define TIMER_PRESCALE_GAIN 7

#define stepTimer_setEnable		TIM1_setEnable

static float dt;

__STATIC_INLINE void stepTimer_setRate(float rate){	//hz
	uint16_t period,prescale;
	if(rate>20000.0f){
		prescale=1*TIMER_PRESCALE_GAIN-1;
		//period=1200-1;
		period=(uint16_t)(TIMER_CLK/rate)-1;
	}else if(rate>500.0f){
		prescale=1*TIMER_PRESCALE_GAIN-1;
		period=(uint16_t)(TIMER_CLK/rate)-1;
	}else if(rate>10.0f){
		prescale=50*TIMER_PRESCALE_GAIN-1;
		period=(uint16_t)(TIMER_CLK_DIV_50/rate)-1;
	}else{
		prescale=50*TIMER_PRESCALE_GAIN-1;
		period=48000-1;
	}
	TIM1_setPrescale(prescale);
	TIM1_setReload(period);
	dt=1.0f/rate;
}
__STATIC_INLINE float stepTimer_getDT(void){
	return dt;
}

