#pragma once

#include <rtthread.h>
#include "lib/ringbuffer.h"

typedef struct{
	struct rt_device parent;
	ringbuffer_t rb;
	
}fakeDevice_t;


#include <rtthread.h>
#include "fakeDevice.h"

//�ⲿ�ýӿ�
void fakeDevice_fill(fakeDevice_t *dev,char *buffer);		//������
void fakeDevice_init(fakeDevice_t *dev,int id);

