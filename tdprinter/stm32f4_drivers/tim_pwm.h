#pragma once

#include "stm32f4xx.h"
#include "gpio.h"

typedef struct{
	int id;
	TIM_TypeDef *base;
	TIM_OCInitTypeDef conf;
	gpio_t port;
	uint8_t af;
	uint16_t topValue;
}pwm_t;

void pwm_init(pwm_t *p);
void pwm_set(pwm_t *p,float value);
 




