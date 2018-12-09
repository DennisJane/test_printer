#pragma once

#include "hmi_hal.h"
#include "hmi_button.h"

#define LIST_VALUE_LEN 20

typedef enum{
	U8,
	S8,
	U16,
	S16,
	U32,
	S32,
	FP,
	DFP,
	STRING,
	STRINGLP,
	CALLBACK,
	FUNC,
}item_type_t;

typedef struct{
	char *name;
	item_type_t type;
	void *lp;
	void *param;
	char value[LIST_VALUE_LEN];
}hmi_item_t;

typedef struct{
	hscreen_t *parent;
	hmi_item_t *list;
	int listNum,buttonNum;
	button_t *listButton;
	button_t *naviButton;
	int curIndex;
	char naviBuf[LIST_VALUE_LEN];
}hmi_list_t;

void hmi_list_load(hmi_list_t *s);
void hmi_list_refresh(hmi_list_t *s);











