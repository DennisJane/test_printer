#pragma once

#include <rtthread.h>
#include "libStepPosition.h"

#define GCODE_EVENT_STARTED	((uint32_t)(1<<0))
#define GCODE_EVENT_STOPPED ((uint32_t)(1<<1))
#define GCODE_EVENT_PAUSED	((uint32_t)(1<<2))
#define GCODE_EVENT_HOMED	((uint32_t)(1<<3))
#define GCODE_EVENT_BUFFER_EMPTY ((uint32_t)(1<<4))

#define GCODE_BUFFER_SIZE 128

typedef struct{
	stepPosition_t stepPos;
	char lineBuffer[GCODE_BUFFER_SIZE],*codeLp;
	int32_t lineLength;
	rt_event_t event;
	rt_thread_t thread;
	rt_device_t device[2];
	uint8_t currentDevice;
	
	//used for gcode processor
	float speed;
	bool  seen[STEPPER_NUM];
	bool  relative[STEPPER_NUM];		//TODO:move this to libStepPosition
	float value[STEPPER_NUM];
	bool  G1UseAccel;
	float G0SpeedFactor;
	float G1SpeedFactor;
}stepGCode_t;


void stepGCode_construct(stepGCode_t *g);
void stepGCode_init(stepGCode_t *g);

int32_t stepGCode_start(stepGCode_t *g,int id,rt_device_t device);
int32_t stepGCode_stop(stepGCode_t *g);
//int32_t stepGCode_pause(stepGCode_t *g,bool pause);

void stepGCode_reset(stepGCode_t *g);



