#pragma once
#include <rtthread.h>

typedef struct{
	struct rt_device parent;
}nullDevice_t;

void nullDevice_init(nullDevice_t *device);

