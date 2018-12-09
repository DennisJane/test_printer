#include "rtuart.h"

static rt_err_t configure(struct rt_serial_device *serial, struct serial_configure *cfg){
	uart_dma_t *dma=serial->parent.user_data;
	uart_t *uart=dma->uart;
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate = cfg->baud_rate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	if(cfg->stop_bits==STOP_BITS_2){
		USART_InitStructure.USART_StopBits = USART_StopBits_2;
	}else{
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
	}
	if(cfg->parity==PARITY_ODD){
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
	}else if(cfg->parity==PARITY_EVEN){
		USART_InitStructure.USART_Parity = USART_Parity_Even;
	}else{
		USART_InitStructure.USART_Parity = USART_Parity_No;
	}
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	uart_configure(uart,&USART_InitStructure);
	if(serial->parent.flag & RT_DEVICE_FLAG_DMA_TX){
		uart_clearFlag(uart,USART_FLAG_TC);
		uart_setISR(uart,USART_IT_TC,ENABLE);
	}
	return RT_EOK;
}
static rt_err_t control(struct rt_serial_device *serial, int cmd, void *arg){
	uart_dma_t *dma=serial->parent.user_data;
	uart_t *uart=dma->uart;
	if(cmd==RT_DEVICE_CTRL_SET_INT){
		int intArg=(int)(arg);
		if(intArg&RT_SERIAL_RX_INT){
			uart_setISR(uart,USART_IT_RXNE,ENABLE);
		}
		if(intArg&RT_SERIAL_TX_INT){
			uart_setISR(uart,USART_IT_TXE,ENABLE);
		}
	}else if(cmd==RT_DEVICE_CTRL_CLR_INT){
		int intArg=(int)(arg);
		if(intArg&RT_SERIAL_RX_INT){
			uart_setISR(uart,USART_IT_RXNE,DISABLE);
		}
		if(intArg&RT_SERIAL_TX_INT){
			uart_setISR(uart,USART_IT_TXE,DISABLE);
		}
	}
	return RT_EOK;
}
static int putc(struct rt_serial_device *serial, char c){
	uart_dma_t *dma=serial->parent.user_data;
	uart_t *uart=dma->uart;
	uart_putc(uart,c);
	return RT_EOK;
}
static int getc(struct rt_serial_device *serial){
	uart_dma_t *dma=serial->parent.user_data;
	uart_t *uart=dma->uart;
	return uart_trygetc(uart);
}
static rt_size_t dma_transmit(struct rt_serial_device *serial, const char *buf, rt_size_t size){
	uart_dma_t *dma=serial->parent.user_data;
	uart_t *uart=dma->uart;
	uart_clearFlag(uart,USART_FLAG_TC);
	uart_dma_transmit(dma,(uint8_t*)buf,size);
	return 0;
}
static struct rt_uart_ops ops={configure,control,putc,getc,dma_transmit}; 

void rt_uart_init(rt_serial_t *rt_uart,char *name,uart_dma_t *uart_dma)
{
	rt_uart->ops=&ops;
	rt_hw_serial_register(rt_uart, name,
		RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_INT_TX,uart_dma);
	rt_device_open(&rt_uart->parent,RT_DEVICE_FLAG_RDWR);
}


void stm32_rtuart_handler(rt_serial_t *rtuart){
	rt_interrupt_enter();
	uart_dma_t *dma=(uart_dma_t*)rtuart->parent.user_data;
	uart_t *uart=dma->uart;
	if(uart->uart_base->SR & USART_SR_TC){
		if(rtuart->parent.flag & RT_DEVICE_FLAG_DMA_TX){		
			USART_ClearFlag(uart->uart_base,USART_FLAG_TC);
			rt_hw_serial_dma_tx_isr(rtuart);
		}
	}
	if(uart->uart_base->SR & USART_SR_TXE){
		if(rtuart->parent.flag & RT_DEVICE_FLAG_INT_TX){
			rt_hw_serial_int_tx_isr(rtuart);
		}
	}
	if(uart->uart_base->SR & USART_SR_RXNE){
		if(rtuart->parent.flag & RT_DEVICE_FLAG_INT_RX){
			rt_hw_serial_int_rx_isr(rtuart);
		}
	}
	rt_interrupt_leave();
}

