#ifndef __VIRTUAL_SERIAL1_H__
#define __VIRTUAL_SERIAL1_H__

#include <rthw.h>
#include <rtthread.h>
#include <stdint.h>
#include <rtdevice.h>

#define VIRTUAL_SERIAL_RX_BUFFER_SIZE 255

typedef struct{
	struct rt_ringbuffer rxBuffer;
	uint8_t (*Send)(uint8_t port,uint8_t *buf,uint8_t len);
}VIRTUAL_SERIAL_T;

rt_err_t rt_hw_vserial_register(rt_device_t device, const char* name, rt_uint32_t flag,VIRTUAL_SERIAL_T *serial);

void rt_hw_vserial_isr(rt_device_t device);



#endif
