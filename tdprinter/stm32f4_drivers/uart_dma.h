#pragma once

#include "uart.h"

typedef struct{
	uart_t *uart;
	NVIC_InitTypeDef nvic_txConfig;
	NVIC_InitTypeDef nvic_rxConfig;
	DMA_Stream_TypeDef *dma_txBase;
	DMA_Stream_TypeDef *dma_rxBase;
	DMA_InitTypeDef dma_txConfig;
	DMA_InitTypeDef dma_rxConfig;
	uint32_t clearTxFlag;
	uint32_t clearRxFlag;
}uart_dma_t;

void uart_dma_init(uart_dma_t *dma,int txEnable,int rxEnable);
void uart_dma_tx_setIT(uart_dma_t *dma,int tc);
void uart_dma_rx_setIT(uart_dma_t *dma,int tc);
void uart_dma_transmit(uart_dma_t *dma,uint8_t *buffer,uint32_t size);
void uart_dma_clearTxFlag(uart_dma_t *dma);









