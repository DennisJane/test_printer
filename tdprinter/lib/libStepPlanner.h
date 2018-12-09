#ifndef _H_LIB_STEP_PLANNER_
#define _H_LIB_STEP_PLANNER_

#include "libStepBlock.h"

typedef struct{
	stepBlock_t stepBlock;
	motionLine_t block;
	//stage1
	float axisDist[STEPPER_NUM],distance;
	float nominalDt;
	float curSpeed[STEPPER_NUM];
	float defaultEntrySpeed,defaultExitSpeed;
	//stage2
	//used for jerk
	float prevSpeed[STEPPER_NUM];
	float prevExitSpeed;
}stepPlanner_t;

void stepPlanner_construct(stepPlanner_t *p);
void stepPlanner_init(stepPlanner_t *p);

//
void stepPlanner_inputMotion(stepPlanner_t *p,int32_t steps[STEPPER_NUM],float dist[STEPPER_NUM],float speed,float length,bool useAccel,float defaultEntrySpeed,float defaultExitSpeed);



#endif





