#pragma once

#include <stdint.h>
#include "lib/libConfig.h"

//sizeof(PARAM_T)必须为4的倍数
typedef struct
{
    // 参数信息，pos=0。
    uint32_t version; // 参数结构的版本。
    //
	float positionOffset[4];
	
	//used for libStepPlanner.c
	float mm2step[STEPPER_NUM];
    int   microStep[STEPPER_NUM];
	float maxSpeed[4];	//XYEZ
	float maxSpeedDelta[4];
	float defaultEntrySpeed[4];
	float defaultAccelSpeed[4];	// [XY 0 E Z]
	float maxAccelSpeed[4];
	float advanceK;
	float advanceDeprime;
	//
    uint32_t checksum;
}param_t;

//APP
int32_t param_init(void);

//USR
param_t	*param_getRamParam(void);
const param_t	*param_getRomParam(void);
int32_t param_save(void);
int32_t param_load(void);
int32_t param_reset(void);
extern param_t *param;

