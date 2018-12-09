#pragma once

typedef struct hmi_button_tag{
	int sid;
	int bid;
	int tid;
	char *name;
	void *parent;
	void (*onPressed)(struct hmi_button_tag *this);
	int userid;
}button_t;

typedef void (*button_cb)(button_t *b);

void button_setParent(button_t *b,void *parent);
void button_setName(button_t *b,char *name);
void button_refreshName(button_t *b,char *pre,char *post);
void button_setUserId(button_t *b,int userid);
void button_onPressed(button_t *b);
void button_setFunc(button_t *b,button_cb func);
int  button_checkPressed(button_t b[],int num,int sid,int bid);











