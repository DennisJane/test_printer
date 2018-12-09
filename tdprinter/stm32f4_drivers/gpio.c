#include "gpio.h"

void gpio_rcc_init(void){
	RCC_AHB1PeriphClockCmd(
	RCC_AHB1Periph_GPIOA|
	RCC_AHB1Periph_GPIOB|
	RCC_AHB1Periph_GPIOC|
	RCC_AHB1Periph_GPIOD|
	RCC_AHB1Periph_GPIOE|
	RCC_AHB1Periph_GPIOF|
	RCC_AHB1Periph_GPIOG|
	RCC_AHB1Periph_GPIOH|
	RCC_AHB1Periph_GPIOI|
	RCC_AHB1Periph_GPIOJ|
	RCC_AHB1Periph_GPIOK,
	ENABLE);
}

void gpio_init(gpio_t *port,GPIOMode_TypeDef mode){
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = port->pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = mode;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(port->port, &GPIO_InitStructure);
	if(port->value){
		port->port->BSRRL|=port->pin;
	}else{
		port->port->BSRRH|=port->pin;
	}
}

void gpio_init_spd(gpio_t *port,GPIOMode_TypeDef mode,GPIOSpeed_TypeDef spd){
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = port->pin;
	GPIO_InitStructure.GPIO_Speed = spd;
	GPIO_InitStructure.GPIO_Mode  = mode;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(port->port, &GPIO_InitStructure);
	if(port->value){
		port->port->BSRRL|=port->pin;
	}else{
		port->port->BSRRH|=port->pin;
	}
}

void gpio_init_custom(gpio_t *port,GPIO_InitTypeDef attribute){
	attribute.GPIO_Pin=port->pin;
	GPIO_Init(port->port,&attribute);
	if(port->value){
		port->port->BSRRL|=port->pin;
	}else{
		port->port->BSRRH|=port->pin;
	}
}



