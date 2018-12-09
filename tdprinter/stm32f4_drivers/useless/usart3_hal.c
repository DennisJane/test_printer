#include <rtthread.h>
#include "stm32f4xx.h"

#define UART				USART3
#define UART_RCC()			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE)
#define AFIO_RCC()			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE)
#define GPIO				GPIOD
#define GPIO_TX				GPIO_Pin_8
#define GPIO_TX_PIN 		GPIO_PinSource8
#define GPIO_RX				GPIO_Pin_9
#define GPIO_RX_PIN 		GPIO_PinSource9
#define GPIO_AF				GPIO_AF_USART3

#define DMA					DMA1
#define DMA_RCC()			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
#define DMA_TX 				DMA1_Stream3
#define DMA_RX				0
#define DMA_TX_CH			DMA_Channel_4

#define UART_IRQ 			USART3_IRQn
#define DMA_IRQ				DMA1_Stream3_IRQn

static void RCC_Configuration(void)
{
	UART_RCC();
	AFIO_RCC();
	DMA_RCC();
}
static void  DMA_Configuration(void){
	DMA_InitTypeDef DMA_InitStructure;

	DMA_InitStructure.DMA_Channel = DMA_TX_CH;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = (uint32_t)1;			//初始化为0会出错
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;			//stm32f4xx的MSIZE和PSIZE有限制,SHIT
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_DeInit(DMA_TX);
	DMA_Init(DMA_TX, &DMA_InitStructure);
	DMA_TX->NDTR=0;
}
static void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_TX | GPIO_RX;
	GPIO_Init(GPIO, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIO, GPIO_TX_PIN, GPIO_AF);
    GPIO_PinAFConfig(GPIO, GPIO_RX_PIN, GPIO_AF);
}

static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = UART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void UART3_HAL_Init()
{
	RCC_Configuration();
	GPIO_Configuration();
	NVIC_Configuration();
	DMA_Configuration();
}

void UART3_HAL_Config(USART_InitTypeDef *init){
	USART_Cmd(UART, DISABLE);
	if(init){
		USART_Init(UART,init);
	}else{
		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(UART,&USART_InitStructure);
	}
	USART_Cmd(UART, ENABLE);
}

void UART3_Putc(uint8_t c){
	while (!(UART->SR & USART_SR_TXE));
	UART->DR = c;
}

uint8_t UART3_Getc(void){
	while(!(UART->SR & USART_SR_RXNE));
	return UART->DR;
}

void rt_hw_console_output(const char *str){
	while(*str){
		if(*str=='\n'){
			UART3_Putc('\r');
		}
		UART3_Putc(*str);
		str++;
	}
}

//使用条件
//buf非易失
void UART3_DMA_Commit(const char *buf,int size){
	while(DMA_GetCurrDataCounter(DMA_TX)){;}
	USART_DMACmd(UART,USART_DMAReq_Tx,DISABLE);
	DMA_Cmd(DMA_TX, DISABLE);
		
	DMA_TX->M0AR = (uint32_t)buf;
	DMA_TX->NDTR = size;

	//DMA_ITConfig(DMA_TX, DMA_IT_TC, ENABLE);
	USART_DMACmd(UART,USART_DMAReq_Tx,ENABLE);
	DMA_Cmd(DMA_TX, ENABLE);
}




