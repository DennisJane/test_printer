#include "ringbuffer.h"
#include <rtthread.h>

#define MALLOC rt_malloc
#define FREE rt_free
#define MEMCPY rt_memcpy
#define INC(x) x=( x + 1 ) % rb->itemNum

void ringbuffer_init(ringbuffer_t *rb,uint8_t itemSize,uint16_t itemNum){
	rb->itemSize=itemSize;
	rb->itemNum=itemNum;
	rb->buffer=(uint8_t*)MALLOC(rb->itemNum*rb->itemSize);
	ringbuffer_reset(rb);
}
void ringbuffer_reset(ringbuffer_t *rb){
	rb->head=0;rb->tail=0;rb->len=0;
	for(uint32_t i=0;i<rb->itemNum*rb->itemSize;i++){
		rb->buffer[i]=0;
	}
}
int32_t ringbuffer_push(ringbuffer_t *rb,uint8_t *item){
	if( rb->len < rb->itemNum ){
		MEMCPY(rb->buffer + rb->head*rb->itemSize,item,rb->itemSize);
		INC(rb->head);
		rb->len++;
		return 1;
	}
	return 0;
}

int32_t ringbuffer_pop(ringbuffer_t *rb,uint8_t *item){
	if( rb->len > 0){
		if(item)
		MEMCPY(item,rb->buffer + rb->tail*rb->itemSize,rb->itemSize);
		INC(rb->tail);
		rb->len--;
		return 1;
	}
	return 0;
}
int32_t ringbuffer_peak(ringbuffer_t *rb,uint8_t *item){
	if( rb->len > 0){
		if(item)
		MEMCPY(item,rb->buffer + rb->tail*rb->itemSize,rb->itemSize);
		return 1;
	}
	return 0;
}
int32_t ringbuffer_getNum(ringbuffer_t *rb){
	return rb->len;
}














