#pragma once

#include <rtthread.h>
#include <stdint.h>
#include <stdbool.h>
#include "libConfig.h"
#include "libStepAxis.h"
#include "param/param.h"

#define STEP_EVENT_BLOCK_COMPLETE (uint32_t)(1<<0)
#define STEP_EVENT_BLOCK_IDLE (uint32_t)(1<<1)

typedef struct{
	//stage1
	//used by stepBlock DDA
	int32_t		masterStep;
	int32_t		steps[STEPPER_NUM];		//abs steps
	bool		dir[STEPPER_NUM];
	
	//stage2
	float distance;			//mm
	float nominalSpeed;
	float nominalRate;		//steps/s
	
	//stage3
	float accelRate;
	float accelSpeed;
	
	//stage4
	float jerkEntrySpeed;	//mm/s	//jerk safe junction speed
	float entrySpeed;		//
	
	//stage5
	//calc_trapezoid_accel_param
	float		entryRate,exitRate;
	int32_t 	accelBefore,decelAfter;
	
	//effeciency
	bool recalculate_flag;
	bool busy;
	bool nominal_length_flag;
	bool useAccel;
	float mmPerStep[4];
	float speedFactor[4];
	bool extruderMoved;
	//debug
	int entrySetBy;
	float trueNominalRate;
	float trueExitRate;
	float trueTime;
	float speedReadyForStop;
	float speedAfterAccel;
	float prevSpeed[4];
	float prevExitSpeed;
}motionLine_t;

typedef struct stepBlock_tag{
	stepAxis_t 		stepAxis;
	motionLine_t 	buffer[MOTION_BUFFER_SIZE];
	motionLine_t	currentLine;
	motionLine_t    *currentBuffer;
	int32_t 		head,tail,len;
	rt_event_t	    event;
	int32_t 		masterCounter;
	float currentRate,timeElasped;	//used for accel
}stepBlock_t;

void stepBlock_construct(stepBlock_t *b);
void stepBlock_init(stepBlock_t *b);
void stepBlock_reset(stepBlock_t *b);

//如果队列满,会等待BLOCK_COMPLETE事件
void stepBlock_inputMotion(stepBlock_t *b,motionLine_t *block);



rt_err_t stepBlock_waitForIdle(stepBlock_t *b,rt_int32_t tick);
void stepBlock_ISR(stepBlock_t *b);
void stepBlock_forceStop(stepBlock_t *b);
void stepBlock_setEnableAll(stepBlock_t *b,bool en);

#define stepBlock_step(b) stepAxis_step(&(b)->stepAxis)
#define stepBlock_isBusy(b) stepAxis_isBusy(&(b)->stepAxis)
#define stepBlock_setEnable(b,en) stepAxis_setEnable(&((b)->stepAxis),en)
#define stepBlock_setDisable(b,en) stepAxis_setDisable(&((b)->stepAxis),en)
#define stepBlock_setHardPos(b,pos) stepAxis_setHardpos(&(b)->stepAxis,pos)
#define stepBlock_getHardPos(b)	stepAxis_getHardPos(&(b)->stepAxis)





