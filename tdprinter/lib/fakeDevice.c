#include <rtthread.h>
#include "fakeDevice.h"

#define RB_SIZE 64

//libStepGCode用接口
rt_size_t fakeDevice_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size){
	fakeDevice_t *device=(fakeDevice_t*)dev->user_data;
	if(ringbuffer_pop(&device->rb,buffer)){
		return 1;
	}
	return 0;
}
rt_size_t fakeDevice_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size){
	rt_kprintf("%s",buffer);
	return size;
}

//外部用接口
void fakeDevice_fill(fakeDevice_t *dev,char *buffer){
	while(*buffer){
		ringbuffer_push(&dev->rb,(uint8_t*)buffer);
		buffer++;
	}
}


void fakeDevice_init(fakeDevice_t *dev,int id)
{
	rt_device_t device=&(dev->parent);
    device->type        = RT_Device_Class_Char;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

    device->init        = 0;
    device->open        = 0;
    device->close       = 0;
    device->read        = fakeDevice_read;
    device->write       = fakeDevice_write;
    device->control     = 0;
	
	ringbuffer_init(&dev->rb,1,RB_SIZE);
    device->user_data   = dev;
	
    /* register a character device */
	char buf[10];
	rt_sprintf(buf,"fakeDev%d",id);
    rt_device_register(&dev->parent,buf,0);
}

