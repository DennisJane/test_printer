#pragma once

#include <rtthread.h>
#include "lib/ringbuffer.h"

typedef struct{
	struct rt_device parent;
	ringbuffer_t rb;
	
}fakeDevice_t;


#include <rtthread.h>
#include "fakeDevice.h"

//外部用接口
void fakeDevice_fill(fakeDevice_t *dev,char *buffer);		//不阻塞
void fakeDevice_init(fakeDevice_t *dev,int id);

