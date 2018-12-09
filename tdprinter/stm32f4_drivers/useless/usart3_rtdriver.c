#include <rtthread.h>
#include "stm32f4xx.h"

static struct rt_device uart3;
static rt_err_t rt_uart3_register(rt_device_t device, const char* name, rt_uint32_t flag,void *user_data);
static rt_err_t  init(rt_device_t dev);
static rt_size_t read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size);
static rt_size_t write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size);

void rt_uart3_init(void){
	rt_uart3_register(&uart3,"uart3",0,0);
}

rt_err_t rt_uart3_register(rt_device_t device, const char* name, rt_uint32_t flag,void *user_data)
{
	RT_ASSERT(device != RT_NULL);
	device->type 		= RT_Device_Class_Char;
	device->rx_indicate = RT_NULL;
	device->tx_complete = RT_NULL;
	device->init 		= init;
	device->open		= RT_NULL;
	device->close		= RT_NULL;
	device->read 		= read;
	device->write 		= write;
	device->control 	= RT_NULL;
	device->user_data	= user_data;
	/* register a character device */
	return rt_device_register(device, name, RT_DEVICE_FLAG_RDWR | flag);
}

void UART3_HAL_Init(void);
void UART3_HAL_Config(USART_InitTypeDef *init);
void UART3_DMA_Send(const char *buf,int size);
void UART3_Putc(uint8_t c);
uint8_t UART3_Getc(void);

static rt_err_t  init(rt_device_t dev){
	UART3_HAL_Init();
	UART3_HAL_Config(0);
	return RT_EOK;
}
static rt_size_t read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size){
	int i=0;
	uint8_t *lp=buffer;
	while(i<size){
		lp[i++]=UART3_Getc();
	}
	return RT_EOK;
}
static rt_size_t write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size){
	int i=0;
	const uint8_t *lp=buffer;
	while(i<size){
		UART3_Putc(lp[i++]);
	}
	return RT_EOK;
}



