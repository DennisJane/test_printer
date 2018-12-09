#ifndef _H_TIM1_TIMER_
#define _H_TIM1_TIMER_

#include <stdbool.h>
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C"{
#endif

__STATIC_INLINE void TIM1_setEnable(bool enable){
	if(enable)	TIM1->CR1 |= TIM_CR1_CEN;
	else		TIM1->CR1 &= (uint16_t)~TIM_CR1_CEN;
}
__STATIC_INLINE void TIM1_setOneShoot(bool enable){
	if(enable)	TIM1->CR1 |= TIM_CR1_OPM;
	else		TIM1->CR1 &= (uint16_t)~TIM_CR1_OPM;
}
__STATIC_INLINE void TIM1_setPreload(bool enable){
	if(enable)	TIM1->CR1 |= TIM_CR1_ARPE;
	else		TIM1->CR1 &= (uint16_t)~TIM_CR1_ARPE;
}

//设置clk分频
//下一周期生效
__STATIC_INLINE void TIM1_setPrescale(uint16_t prescale){
	TIM1->PSC=prescale;
}
//设置计数器触发值
//Preload=下一周期生效,否则立刻生效
__STATIC_INLINE void TIM1_setReload(uint16_t period){
	TIM1->ARR=period;
}

//24M时钟,16bit定时器,16bit分频
//Preload使能,所有设置下周期生效
void TIM1_TIMER_INIT(void);


#ifdef __cplusplus
}
#endif
#endif
