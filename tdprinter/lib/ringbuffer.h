#pragma once

#include <stdint.h>

typedef struct{
	uint8_t itemSize;	//u8=1 u16=2 u32=4 fp=4 u64=8 dfp=8
	uint16_t itemNum;
	
	uint16_t head,tail,len;
	uint8_t *buffer;
}ringbuffer_t;

void ringbuffer_init(ringbuffer_t *rb,uint8_t itemSize,uint16_t itemNum);
void ringbuffer_reset(ringbuffer_t *rb);
int32_t ringbuffer_push(ringbuffer_t *rb,uint8_t *item);
int32_t ringbuffer_pop(ringbuffer_t *rb,uint8_t *item);
int32_t ringbuffer_peak(ringbuffer_t *rb,uint8_t *item);
int32_t ringbuffer_getNum(ringbuffer_t *rb);













