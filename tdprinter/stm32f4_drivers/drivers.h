#pragma once

#include "lib/libConfig.h"

#ifdef TDPRINTER_BRAIN
#include "drivers_config_brain.h"
#else
#include "drivers_config_tdp.h"
#endif

#include "gpio.h"
#include "uart.h"
#include "uart_dma.h"
extern const uart_t uart1;
extern const uart_t uart2;
extern const uart_t uart3;
extern const uart_t uart4;
extern const uart_t uart5;
extern const uart_t uart6;
extern uart_dma_t uart1_dma;
extern uart_dma_t uart2_dma;
extern uart_dma_t uart3_dma;
extern uart_dma_t uart4_dma;
extern uart_dma_t uart5_dma;
extern uart_dma_t uart6_dma;
#include "spi.h"
extern spi_t spi1;
extern spi_t spi2;
extern spi_t spi3;
extern spi_t spi4;
extern spi_t spi5;
extern spi_t spi6;

#include "tim_base.h"
#include "tim_pwm.h"

extern pwm_t heater;

#include "systick.h"

void core_init(void);

