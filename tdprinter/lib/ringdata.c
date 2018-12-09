#include "ringdata.h"
#include <rtthread.h>

#define MALLOC rt_malloc
#define FREE rt_free
#define MEMCPY rt_memcpy

void ringdata_init(ringdata_t *rb,uint16_t itemNum){
	ringbuffer_t *_rb=(ringbuffer_t*)rb;
	_rb->itemNum=itemNum;
	_rb->itemSize=sizeof(ringdata_item_t);
	ringbuffer_init(_rb,_rb->itemSize,_rb->itemNum);
	ringdata_reset(rb);
}
void ringdata_reset(ringdata_t *rb){
	ringbuffer_t *_rb=(ringbuffer_t*)rb;
	rb->tail=0;
	rb->len=0;
	ringbuffer_reset(_rb);
}
int32_t ringdata_push(ringdata_t *rb,ringdata_item_t *item){
	ringbuffer_t *_rb=(ringbuffer_t*)rb;
	ringdata_item_t freeItem;
	while(_rb->len){
		ringbuffer_peak(_rb,(uint8_t*)&freeItem);
		if(freeItem.deleteLater){
			FREE(freeItem.data);
			ringbuffer_pop(_rb,0);
		}else{
			break;
		}
	}
	item->deleteLater=0;
	if(ringbuffer_push(_rb,(uint8_t*)item)==1){
		rb->len++;
		return 1;
	}
	return 0;
}
int32_t ringdata_pop(ringdata_t *rb,ringdata_item_t *item){
	ringbuffer_t *_rb=(ringbuffer_t*)rb;
	if( rb->len > 0){
		ringdata_item_t *data=(ringdata_item_t*)_rb->buffer;
		if(item){
			MEMCPY(item,&data[rb->tail],sizeof(ringdata_item_t));
		}
		data[rb->tail].deleteLater=1;
		rb->tail = (rb->tail+1)%_rb->itemNum;
		rb->len--;
		return 1;
	}
	return 0;
}
int32_t ringdata_peak(ringdata_t *rb,ringdata_item_t *item){
	ringbuffer_t *_rb=(ringbuffer_t*)rb;
	if( rb->len > 0){
		ringdata_item_t *data=(ringdata_item_t*)_rb->buffer;
		if(item){
			MEMCPY(item,&data[rb->tail],sizeof(ringdata_item_t));
		}
		return 1;
	}
	return 0;
}
int32_t ringdata_getNum(ringdata_t *rb){
	//ringbuffer_t *_rb=(ringbuffer_t*)rb;
	return rb->len;
}





