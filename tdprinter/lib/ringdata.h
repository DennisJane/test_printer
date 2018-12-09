#pragma once
#include "ringbuffer.h"

//为了解决不能在ISR里free,创建了ringdata

typedef struct{
	uint8_t *data;
	uint16_t size;
	uint16_t deleteLater;	
}ringdata_item_t;

typedef struct{
	ringbuffer_t rb;
	uint16_t tail,len;	//another tail
}ringdata_t;

void ringdata_init(ringdata_t *rb,uint16_t itemNum);
void ringdata_reset(ringdata_t *rb);
int32_t ringdata_push(ringdata_t *rb,ringdata_item_t *item);
int32_t ringdata_pop(ringdata_t *rb,ringdata_item_t *item);
int32_t ringdata_peak(ringdata_t *rb,ringdata_item_t *item);
int32_t ringdata_getNum(ringdata_t *rb);


