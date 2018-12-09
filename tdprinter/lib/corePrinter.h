#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <rtthread.h>

#include "lib/libConfig.h"
#include "libStepGCode.h"

#include "fileDevice.h"



struct corePrinter_tag;

typedef struct corePrinter_tag{
	stepGCode_t gcodeDevice;
	int id;
	fileDevice_t fileDevice;
	rt_device_t defaultDevice;
	int printFileComplete;
	char fileName[25];
	int timeUsed,timeBase;
	rt_thread_t thread;
	double totalDistance;
}corePrinter_t;


void corePrinter_construct(corePrinter_t *c);

void corePrinter_init(corePrinter_t *c,rt_device_t defaultDev);

void corePrinter_start(corePrinter_t *c,char *filename);

void corePrinter_stop(corePrinter_t *c);

/*
//input target coordinate, blocked func
void axis_inputPosition(double pos[],double mask[]);

//set current coordinate
void axis_setCurrentPosition(double pos[],double mask[]);

void axis_setRelative(double relative[]);

bool axis_isBusy(void);
void axis_waitForIdle(void);

void axis_pause(void);
void axis_resume(void);

void axis_stop(void);
void axis_setEndstop(bool enable[],bool mask[]);
void axis_setMotor(bool enable,bool mask[]);

void axis_timerISR(corePrinter_t *a);
*/

