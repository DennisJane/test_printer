#pragma once

#include "stm32f4xx.h"
#include <stdbool.h>


#ifdef __cplusplus
extern "C"{
#endif

//使用Pin而不是Bit的原因是gpio_set时效率高

typedef struct{
	GPIO_TypeDef *port;//GPIOx
	uint16_t pin;					 //GPIO_Pin_x
	uint16_t value;				 //default value
}gpio_t;

void gpio_rcc_init(void);
void gpio_init(gpio_t *port,GPIOMode_TypeDef mode);
void gpio_init_spd(gpio_t *port,GPIOMode_TypeDef mode,GPIOSpeed_TypeDef spd);
void gpio_init_custom(gpio_t *port,GPIO_InitTypeDef attribute);

__STATIC_INLINE void gpio_set(gpio_t *port,bool value){
	if(value){
		port->port->BSRRL|=port->pin;
	}else{
		port->port->BSRRH|=port->pin;
	}
}


__STATIC_INLINE bool gpio_get(gpio_t *port){
	return ((port->port->IDR)&(port->pin))?true:false;
}

#ifdef __cplusplus
}
#endif

