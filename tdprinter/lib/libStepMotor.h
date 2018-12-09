#pragma once

#include "stm32f4_drivers/drivers.h"
#include "lib/libConfig.h"

typedef struct{
	gpio_t en;
	gpio_t clk;
	gpio_t dir;
	bool inv_en;
	bool inv_clk;
	bool inv_dir;
}stepMotor_t;



//void stepMotor_construct(stepMotor_t *s);
void stepMotor_init(stepMotor_t *s);

//设定步进电机方向电平
__STATIC_INLINE void stepMotor_setDir(stepMotor_t *s,bool dir){
	gpio_set(&s->dir,dir^s->inv_dir);
	__ASM("nop");
}

//步进电机换向电平
__STATIC_INLINE void stepMotor_oneStep(stepMotor_t *s){
	gpio_set(&s->clk,true^s->inv_clk);
	__ASM("nop");
	__ASM("nop");
	gpio_set(&s->clk,false^s->inv_clk);
	__ASM("nop");
	__ASM("nop");
}

//步进电机使能电平
__STATIC_INLINE void stepMotor_setEnable(stepMotor_t *s,bool enable){
	gpio_set(&s->en,enable^s->inv_en);
	__ASM("nop");
}



