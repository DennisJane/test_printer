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
//ֹͣ��ͬʱ�����HardPos��ֵ
void stepPosition_stop(stepPosition_t *p);
//����Դλ�ã�ͬʱ�����HardPos
void stepPosition_setMm(stepPosition_t *p,float mm[],bool relative[],bool mask[]);





