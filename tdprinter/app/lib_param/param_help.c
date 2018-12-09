#include "param_help.h"
#include <rtthread.h>

#define PARAM_NUM 1
#define STATUS_NUM 1

static paramItem_t ramParam[PARAM_NUM];
static paramItem_t statusParam[STATUS_NUM];

void paramHelpInit(void){
	
}
void paramItemInit(paramItem_t *p,paramType_t type,uint16_t num,void *addr,char* descript){
	p->type=type;
	switch(type){
	case S8:
	case U8:
		p->size=1;
		break;
	case S16:
	case U16:
		p->size=2;
		break;
	default:
		p->size=4;
		break;
	}
	p->num=num;
	p->addr=addr;
	p->descript=descript;
}


paramItem_t* getRamParam(uint32_t index){
	if(index<PARAM_NUM)
	return ramParam+index;
	else
	return 0;
}

paramItem_t* getStatusParam(uint32_t index){
	if(index<STATUS_NUM)
	return statusParam+index;
	else
	return 0;
}

uint32_t getRamNum(void){
	return PARAM_NUM;
}
uint32_t getStatusNum(void){
	return STATUS_NUM;
}


