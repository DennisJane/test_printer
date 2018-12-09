#include "uart_dma.h"

void uart_dma_init(uart_dma_t *dma,int txEnable,int rxEnable){
	if(txEnable){
		NVIC_Init(&(dma->nvic_txConfig));
		USART_DMACmd(dma->uart->uart_base, USART_DMAReq_Tx , ENABLE);
		DMA_DeInit(dma->dma_txBase);
	}
	if(rxEnable){
		NVIC_Init(&(dma->nvic_rxConfig));
		USART_DMACmd(dma->uart->uart_base, USART_DMAReq_Rx , ENABLE);
		DMA_DeInit(dma->dma_rxBase);
		DMA_Init(dma->dma_rxBase,&dma->dma_rxConfig);
	}
}

void uart_dma_tx_setIT(uart_dma_t *dma,int tc){
	if(tc){
		DMA_ITConfig(dma->dma_txBase, DMA_IT_TC|DMA_IT_TE, ENABLE);
	}
}

void uart_dma_rx_setIT(uart_dma_t *dma,int tc){
	if(tc){
		DMA_ITConfig(dma->dma_rxBase, DMA_IT_TC|DMA_IT_TE, ENABLE);
	}
}

void uart_dma_transmit(uart_dma_t *dma,uint8_t *buffer,uint32_t size){
	if(size==0)assert_param(0);
	DMA_ClearFlag(dma->dma_txBase,dma->clearTxFlag);
	dma->dma_txConfig.DMA_BufferSize=size;
	dma->dma_txConfig.DMA_Memory0BaseAddr=(uint32_t)buffer;	
	
	DMA_Init(dma->dma_txBase, &dma->dma_txConfig);
	DMA_Cmd(dma->dma_txBase,ENABLE);
}
void uart_dma_clearTxFlag(uart_dma_t *dma){
	DMA_ClearFlag(dma->dma_txBase,dma->clearTxFlag);
}


