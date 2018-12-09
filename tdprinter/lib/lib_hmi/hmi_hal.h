#pragma once

#include <stdint.h>
#include "hmi_driver.h"

struct hscreen_tag;
struct hmi_tag;

typedef struct hscreen_tag{
	int sid;	//screen id
	struct hscreen_tag *parent;
	void (*onLoad)(struct hscreen_tag *self);
	void (*onButton)(struct hscreen_tag *self,int id);
	void (*onTimer)(struct hscreen_tag *self);
	void *data;
}hscreen_t;

typedef struct hmi_tag{
	int id;
	hscreen_t *curScreen;
	rt_device_t device;
	rt_thread_t thread;
}hmi_t;

extern hmi_t hmi;

typedef enum{
	hmi_screen_call,
	hmi_screen_jump,
}hmi_switch_mode;

void hmi_construct(hmi_t *s,int id);
void hmi_init(hmi_t *s,char *uartDev,hscreen_t *initScreen);

//use for screen
void hmi_switch_screen(hmi_t  *s,hmi_switch_mode smode,hscreen_t *dst);







