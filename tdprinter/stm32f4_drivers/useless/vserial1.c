#include "vserial1.h"
#include <rtthread.h>
#include "stm32f4xx.h"
#include "app/lib_appLayer.h"

#define Send(x,y) Send(CONSOLE,x,y)

struct rt_device RT_DEV_VUSART1={0};
VIRTUAL_SERIAL_T vserial1={0};
static uint8_t rxBuffer[VIRTUAL_SERIAL_RX_BUFFER_SIZE];	//RingBuffer

static void rt_serial_portHandler(uint8_t port,uint8_t *buffer,uint8_t len){
	rt_ringbuffer_t rb=&(((VIRTUAL_SERIAL_T*)RT_DEV_VUSART1.user_data)->rxBuffer);
	if(len!=0){
		rt_ringbuffer_put(rb,buffer,len);
		rt_hw_vserial_isr(&RT_DEV_VUSART1);
	}
}
				
/* RT-Thread Device Interface */
static rt_err_t rt_serial_init (rt_device_t dev)
{
	VIRTUAL_SERIAL_T* vs = (VIRTUAL_SERIAL_T*) dev->user_data;
	rt_ringbuffer_t rb=&(vs->rxBuffer);

	if (!(dev->flag & RT_DEVICE_FLAG_ACTIVATED))
	{
		dev->flag |= RT_DEVICE_FLAG_ACTIVATED;
		appLayer_register(CONSOLE,rt_serial_portHandler);
		vs->Send=appLayer_send;
		rt_ringbuffer_init(rb,rxBuffer,sizeof(rxBuffer));
	}

	return RT_EOK;
}

static rt_err_t rt_serial_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t rt_serial_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_size_t rt_serial_read (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	VIRTUAL_SERIAL_T* vs = (VIRTUAL_SERIAL_T*) dev->user_data;
	rt_ringbuffer_t rb=&(vs->rxBuffer);
	if(RT_RINGBUFFER_SIZE(rb)){
		rt_ringbuffer_getchar(rb,buffer);
		return 1;
	}else{
		return 0;
	}
}

static rt_size_t rt_serial_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
	VIRTUAL_SERIAL_T* vs = (VIRTUAL_SERIAL_T*) dev->user_data;
	if(vs->Send((uint8_t*)buffer,size))return size;
	//rt_hw_console_output(buffer);
	return size;
}

static rt_err_t rt_serial_control (rt_device_t dev, rt_uint8_t cmd, void *args)
{
	return RT_EOK;
}

/*
 * serial register for STM32
 * support STM32F103VB and STM32F103ZE
 */
rt_err_t rt_hw_vserial_register(rt_device_t device, const char* name, rt_uint32_t flag,VIRTUAL_SERIAL_T *serial)
{
	RT_ASSERT(device != RT_NULL);

	device->type 		= RT_Device_Class_Char;
	device->rx_indicate = RT_NULL;
	device->tx_complete = RT_NULL;
	device->init 		= rt_serial_init;
	device->open		= rt_serial_open;
	device->close		= rt_serial_close;
	device->read 		= rt_serial_read;
	device->write 		= rt_serial_write;
	device->control 	= rt_serial_control;
	device->user_data	= serial;

	/* register a character device */
	return rt_device_register(device, name, RT_DEVICE_FLAG_RDWR | flag);
}

void rt_hw_vserial_isr(rt_device_t device)
{
	VIRTUAL_SERIAL_T* vs = (VIRTUAL_SERIAL_T*) device->user_data;
	rt_ringbuffer_t rb=&(vs->rxBuffer);
	
	if (device->rx_indicate != RT_NULL)
	{
		device->rx_indicate(device,RT_RINGBUFFER_SIZE(rb));
	}
}


