#pragma once

#include "stm32f4xx.h"
#include "gpio.h"

typedef struct{
	int id;
	void (*rcc_init)(uint32_t rcc,FunctionalState en);
	uint32_t rcc;
	SPI_TypeDef *base;
	gpio_t sck;
	gpio_t miso;
	gpio_t mosi;
	uint8_t af;
}spi_t;

void spi_init(spi_t *s);
void spi_configure(spi_t *s,SPI_InitTypeDef *conf);
void spi_setBaudRate(spi_t *s,uint32_t max_hz);
void spi_setBaudPrescaler(spi_t *s,uint16_t SPI_BaudRatePrescaler);

uint16_t spi_swap(spi_t *s,uint16_t byte);
void spi_swaps8(spi_t *s,uint8_t *tx,uint8_t *rx,uint16_t size);
void spi_swaps16(spi_t *s,uint16_t *tx,uint16_t *rx,uint16_t size);



