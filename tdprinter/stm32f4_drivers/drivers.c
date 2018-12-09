#include "drivers.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uart_t uart1={
	USART1,
	RCC_APB2PeriphClockCmd,
	RCC_APB2Periph_USART1,
	{RTE_USART1_TX_PORT,1<<RTE_USART1_TX_BIT,1,},
	{RTE_USART1_RX_PORT,1<<RTE_USART1_RX_BIT,1,},
	GPIO_AF_USART1,
	{
		USART1_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
};
const uart_t uart2={
	USART2,
	RCC_APB1PeriphClockCmd,
	RCC_APB1Periph_USART2,
	{RTE_USART2_TX_PORT,1<<RTE_USART2_TX_BIT,1,},
	{RTE_USART2_RX_PORT,1<<RTE_USART2_RX_BIT,1,},
	GPIO_AF_USART2,
	{
		USART2_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
};
const uart_t uart3={
	USART3,
	RCC_APB1PeriphClockCmd,
	RCC_APB1Periph_USART3,
	{RTE_USART3_TX_PORT,1<<RTE_USART3_TX_BIT,1,},
	{RTE_USART3_RX_PORT,1<<RTE_USART3_RX_BIT,1,},
	GPIO_AF_USART3,
	{
		USART3_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
};
const uart_t uart4={
	UART4,
	RCC_APB1PeriphClockCmd,
	RCC_APB1Periph_UART4,
	{RTE_UART4_TX_PORT,1<<RTE_UART4_TX_BIT,1,},
	{RTE_UART4_RX_PORT,1<<RTE_UART4_RX_BIT,1,},
	GPIO_AF_UART4,
	{
		UART4_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
};

const uart_t uart5={
	UART5,
	RCC_APB1PeriphClockCmd,
	RCC_APB1Periph_UART5,
	{RTE_UART5_TX_PORT,1<<RTE_UART5_TX_BIT,1,},
	{RTE_UART5_RX_PORT,1<<RTE_UART5_RX_BIT,1,},
	GPIO_AF_UART5,
	{
		UART5_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
};

const uart_t uart6={
	USART6,
	RCC_APB2PeriphClockCmd,
	RCC_APB2Periph_USART6,
	{RTE_USART6_TX_PORT,1<<RTE_USART6_TX_BIT,1,},
	{RTE_USART6_RX_PORT,1<<RTE_USART6_RX_BIT,1,},
	GPIO_AF_USART6,
	{
		USART6_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
};

uart_dma_t uart1_dma={
	(uart_t*)&uart1,
	{	//NVIC DMA TX
		DMA2_Stream7_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	{	//NVIC DMA RX
		DMA2_Stream2_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	DMA2_Stream7,	//DMA TX STREAM
	DMA2_Stream2,	//DMA RX STREAM
	{	//DMA TX CONFIG
		DMA_Channel_4,
		USART1_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_MemoryToPeripheral,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,		//fix bug
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	{	//DMA RX CONFIG
		DMA_Channel_4,
		USART1_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_PeripheralToMemory,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	DMA_FLAG_TEIF7 | DMA_FLAG_TCIF7,
	DMA_FLAG_TEIF2 | DMA_FLAG_TCIF2,
};

uart_dma_t uart2_dma={
	(uart_t*)&uart2,
	{	//NVIC DMA TX
		DMA1_Stream6_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	{	//NVIC DMA RX
		DMA1_Stream5_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	DMA1_Stream6,	//DMA TX STREAM
	DMA1_Stream5,	//DMA RX STREAM
	{	//DMA TX CONFIG
		DMA_Channel_4,
		USART2_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_MemoryToPeripheral,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,		//fix bug
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	{	//DMA RX CONFIG
		DMA_Channel_4,
		USART2_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_PeripheralToMemory,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	DMA_FLAG_TEIF6 | DMA_FLAG_TCIF6,
	DMA_FLAG_TEIF5 | DMA_FLAG_TCIF5,
};

uart_dma_t uart3_dma={
	(uart_t*)&uart3,
	{	//NVIC DMA TX
		DMA1_Stream3_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	{	//NVIC DMA RX
		DMA1_Stream1_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	DMA1_Stream3,	//DMA TX STREAM
	DMA1_Stream1,	//DMA RX STREAM
	{	//DMA TX CONFIG
		DMA_Channel_4,
		USART3_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_MemoryToPeripheral,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,		//fix bug
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	{	//DMA RX CONFIG
		DMA_Channel_4,
		USART3_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_PeripheralToMemory,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	DMA_FLAG_TCIF3,
	DMA_FLAG_TEIF1 | DMA_FLAG_TCIF1,
};

uart_dma_t uart4_dma={
	(uart_t*)&uart4,
	{	//NVIC DMA TX
		DMA1_Stream4_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	{	//NVIC DMA RX
		DMA1_Stream2_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	DMA1_Stream4,	//DMA TX STREAM
	DMA1_Stream2,	//DMA RX STREAM
	{	//DMA TX CONFIG
		DMA_Channel_4,
		UART4_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_MemoryToPeripheral,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,		//fix bug
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	{	//DMA RX CONFIG
		DMA_Channel_4,
		UART4_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_PeripheralToMemory,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	DMA_FLAG_TEIF4 | DMA_FLAG_TCIF4,
	DMA_FLAG_TEIF2 | DMA_FLAG_TCIF2,
};


uart_dma_t uart5_dma={
	(uart_t*)&uart5,
	{	//NVIC DMA TX
		DMA1_Stream7_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	{	//NVIC DMA RX
		DMA1_Stream0_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	DMA1_Stream7,	//DMA TX STREAM
	DMA1_Stream0,	//DMA RX STREAM
	{	//DMA TX CONFIG
		DMA_Channel_4,
		UART5_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_MemoryToPeripheral,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,		//fix bug
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	{	//DMA RX CONFIG
		DMA_Channel_4,
		UART5_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_PeripheralToMemory,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	DMA_FLAG_TEIF7 | DMA_FLAG_TCIF7,
	DMA_FLAG_TEIF0 | DMA_FLAG_TCIF0,
};


uart_dma_t uart6_dma={
	(uart_t*)&uart6,
	{	//NVIC DMA TX
		DMA2_Stream6_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	{	//NVIC DMA RX
		DMA2_Stream1_IRQn,
		1,		//PreemptionPriority
		1,		//SubPriority
		ENABLE,
	},
	DMA2_Stream6,	//DMA TX STREAM
	DMA2_Stream1,	//DMA RX STREAM
	{	//DMA TX CONFIG
		DMA_Channel_5,
		USART6_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_MemoryToPeripheral,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,		//fix bug
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	{	//DMA RX CONFIG
		DMA_Channel_5,
		USART6_BASE+0x04,
		0,	//Memory0 src
		DMA_DIR_PeripheralToMemory,
		0,	//buffer size
		DMA_PeripheralInc_Disable,
		DMA_MemoryInc_Enable,
		DMA_PeripheralDataSize_Byte,
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		DMA_Priority_High,
		DMA_FIFOMode_Disable,
		DMA_FIFOThreshold_Full,
		DMA_MemoryBurst_Single,
		DMA_PeripheralBurst_Single,
	},
	DMA_FLAG_TEIF6 | DMA_FLAG_TCIF6,
	DMA_FLAG_TEIF1 | DMA_FLAG_TCIF1,
};
const USART_InitTypeDef uart_default_config=
{
		115200,
		USART_WordLength_8b,
		USART_StopBits_1,
		USART_Parity_No,
		USART_Mode_Rx | USART_Mode_Tx,
		USART_HardwareFlowControl_None,
};
spi_t spi1={
	1,
	RCC_APB2PeriphClockCmd,
	RCC_APB2Periph_SPI1,
	SPI1,
	{RTE_SPI1_SCL_PORT, 1<<RTE_SPI1_SCL_BIT,1,},	//sck
	{RTE_SPI1_MISO_PORT,1<<RTE_SPI1_MISO_BIT,1,},	//miso
	{RTE_SPI1_MOSI_PORT,1<<RTE_SPI1_MOSI_BIT,1,},	//mosi
	GPIO_AF_SPI1,
};
spi_t spi2={
	2,
	RCC_APB1PeriphClockCmd,
	RCC_APB1Periph_SPI2,
	SPI2,
	{RTE_SPI2_SCL_PORT, 1<<RTE_SPI2_SCL_BIT,1,},	//sck
	{RTE_SPI2_MISO_PORT,1<<RTE_SPI2_MISO_BIT,1,},	//miso
	{RTE_SPI2_MOSI_PORT,1<<RTE_SPI2_MOSI_BIT,1,},	//mosi
	GPIO_AF_SPI2,
};
spi_t spi3={
	3,
	RCC_APB1PeriphClockCmd,
	RCC_APB1Periph_SPI3,
	SPI3,
	{RTE_SPI3_SCL_PORT, 1<<RTE_SPI3_SCL_BIT,1,},	//sck
	{RTE_SPI3_MISO_PORT,1<<RTE_SPI3_MISO_BIT,1,},	//miso
	{RTE_SPI3_MOSI_PORT,1<<RTE_SPI3_MOSI_BIT,1,},	//mosi
	GPIO_AF_SPI3,
};


void core_init(void){
	SystemCoreClockUpdate();
	gpio_rcc_init();
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	#if RTE_USART1==1
	uart_init((uart_t*)&uart1);
	uart_configure((uart_t*)&uart1,(USART_InitTypeDef*)&uart_default_config);
	uart_dma_init(&uart1_dma,1,0);
	#endif
	#if RTE_USART2==1
	uart_init((uart_t*)&uart2);
	uart_configure((uart_t*)&uart2,(USART_InitTypeDef*)&uart_default_config);
	uart_dma_init(&uart2_dma,1,0);
	#endif
	#if RTE_USART3==1
	uart_init((uart_t*)&uart3);
	uart_configure((uart_t*)&uart3,(USART_InitTypeDef*)&uart_default_config);
	uart_dma_init(&uart3_dma,1,0);
	#endif
	#if RTE_UART4 == 1
	uart_init((uart_t*)&uart4);
	uart_configure((uart_t*)&uart4,(USART_InitTypeDef*)&uart_default_config);
	uart_dma_init(&uart4_dma,1,0);
	#endif
	#if RTE_UART5 == 1
	uart_init((uart_t*)&uart5);
	uart_configure((uart_t*)&uart5,(USART_InitTypeDef*)&uart_default_config);
	uart_dma_init(&uart1_dma,1,0);
	#endif
	#if RTE_USART6==1
	uart_init((uart_t*)&uart6);
	uart_configure((uart_t*)&uart6,(USART_InitTypeDef*)&uart_default_config);
	uart_dma_init(&uart6_dma,1,0);
	#endif
	
	#ifdef RTE_SYSTICK_ENABLE
	systick_init(RTE_SYSTICK_PRE_SECOND);
	#endif
	
	#if RTE_SPI1==1
	spi_init(&spi1);
	#endif
	#if RTE_SPI2==1
	spi_init(&spi2);
	#endif
	#if RTE_SPI3==1
	spi_init(&spi3);
	#endif
	
	/*void testloop(void);
	testloop();
	*/
	
	
}

/*
uint8_t buffer[20],waiting=1;
uint32_t cnt=0;
volatile uint8_t dma_started=0;
void testloop(void){
	while(1){
		sprintf(buffer,"%d\r\n",cnt++);
		while(dma_started==1){__ASM("nop");}
		USART_ClearFlag(USART3,USART_FLAG_TC);
		//USART_ITConfig(USART3, USART_IT_TC | USART_IT_RXNE, ENABLE);
		USART3->CR1 |= USART_CR1_TCIE;
		while(waiting){__ASM("nop");}
		waiting=1;
		dma_started=1;
		uart_dma_transmit(&uart3_dma,buffer,strlen(buffer));
	}
}

void USART3_IRQHandler(void)
{
	if(USART3->SR & USART_SR_TC){
		USART_ClearFlag(USART3,USART_FLAG_TC);
		//USART3->CR1 &= ~USART_CR1_TCIE;
		dma_started=0;
	}
}

*/


