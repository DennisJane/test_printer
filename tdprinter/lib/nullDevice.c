#include "nullDevice.h"

void nullDevice_init(nullDevice_t *dev)
{
	rt_device_t device=&dev->parent;
    device->type        = RT_Device_Class_Char;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

    device->init        = 0;
    device->open        = 0;
    device->close       = 0;
    device->read        = 0;
    device->write       = 0;
    device->control     = 0;
	
    device->user_data   = dev;
	
    rt_device_register(device,"nullDev",0);
}


