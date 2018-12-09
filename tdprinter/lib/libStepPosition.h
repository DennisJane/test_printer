#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "libConfig.h"
#include "libStepPlanner.h"

typedef struct{
	stepPlanner_t planner;
	float		mm2step[STEPPER_NUM];
	float 		mm[STEPPER_NUM],mmDst[STEPPER_NUM];
	int32_t		step[STEPPER_NUM],stepDst[STEPPER_NUM];
	float 		mmOffset[STEPPER_NUM];
	float		distanceMoved;
}stepPosition_t;

//construct
void stepPosition_construct(stepPosition_t *p);
void stepPosition_init(stepPosition_t *p);

//public
void stepPosition_inputNextPosition(stepPosition_t *p,float pos[],bool relative[],bool mask[],float speed,bool useAccel);
//停止，同时会读入HardPos的值
void stepPosition_stop(stepPosition_t *p);
//设置源位置，同时会更新HardPos
void stepPosition_setMm(stepPosition_t *p,float mm[],bool relative[],bool mask[]);





