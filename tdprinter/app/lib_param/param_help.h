#pragma once
#include "param.h"
typedef enum{
	S8,
	U8,
	S16,
	U16,
	S32,
	U32,
	FP
}paramType_t;

typedef struct{
	uint8_t type;
	uint8_t size;
	uint16_t num;
	uint8_t *addr;
	char *descript;
}paramItem_t;

void paramHelpInit(void);

void paramItemInit(paramItem_t *p,paramType_t type,uint16_t num,void *addr,char* descript);

paramItem_t* getRamParam(uint32_t index);
paramItem_t* getStatusParam(uint32_t index);
uint32_t getRamNum(void);
uint32_t getStatusNum(void);



