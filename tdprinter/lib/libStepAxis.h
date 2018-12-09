#pragma once

#include "libStepMotor.h"
#include "libTrigger.h"
#include <math.h>

#define RT_ADVANCE
#define EXTRUDER_NUM 1

#define AXIS_MASK(x) ((uint8_t)(0x01)<<x)

//dir,homeDir是实际的方向,X东Y北Z地
typedef struct stepAxis_tag{
	stepMotor_t	*motor;
	trigger_t 	*maxTrigger,*minTrigger;
	uint8_t		busyFlag;					//电机忙标志位
	int32_t		masterStep;					//主轴所需步数
    int32_t 	step[STEPPER_NUM];			//本轴运动步数
	int32_t 	counter[STEPPER_NUM];   	//协同运动计数器
	int32_t 	stepNeed[STEPPER_NUM];		//本轴所需运动步数
	int32_t     hardPos[STEPPER_NUM];		//底层位置保存,坐标系真实方向
	bool		dir[STEPPER_NUM];			//用于hardPos积分,归位使能判断,为坐标系真实方向
	bool enableTrigger;
	
	#ifdef RT_ADVANCE
	float advance,currentAdvance;	//steps(float)
	int extruderStep;	//dda + (advance-currentAdvance)
	#endif
}stepAxis_t;


void stepAxis_construct(stepAxis_t *a);
void stepAxis_init(stepAxis_t *a);
void stepAxis_setTrigger(stepAxis_t *a,trigger_t *maxTrigger,trigger_t *minTrigger);
//reset不会重设hardPos,在init时重设为0
void stepAxis_reset(stepAxis_t *a);
void stepAxis_setHardPos(stepAxis_t *a,int32_t hardPos[]);
int32_t* stepAxis_getHardPos(stepAxis_t *a);
void stepAxis_setEnable(stepAxis_t *a,bool en[STEPPER_NUM]);
void stepAxis_setDisable(stepAxis_t *a,bool dis[STEPPER_NUM]);

__STATIC_INLINE void stepAxis_reload(stepAxis_t *a,int32_t masterStep,int32_t step[],bool dir[]){
	a->busyFlag=0x00;
	a->masterStep=masterStep;
	for(int i=0;i<STEPPER_NUM;i++){
		a->counter[i]=0;
		a->step[i]=0;
		a->stepNeed[i]=step[i];
		a->dir[i]=dir[i];
		if(a->stepNeed[i]>0){
			a->busyFlag|=AXIS_MASK(i);
		}
		#ifdef RT_ADVANCE
		if(i!=2){	//isExtruderAxis
			stepMotor_setDir(a->motor+i,dir[i]);
		}
		#endif
	}
}

__STATIC_INLINE uint8_t stepAxis_step(stepAxis_t *a){
	for(int i=0;i<STEPPER_NUM;i++){
		//if axis is busy
		if(a->busyFlag & AXIS_MASK(i)){
			if(a->step[i]>=a->stepNeed[i]){
				a->busyFlag &= ~AXIS_MASK(i);
			}else 
			if(a->enableTrigger){
				if(a->dir[i]==true && trigger_read(a->maxTrigger+i)){		//true为触发
					a->busyFlag &= ~AXIS_MASK(i);
				}else if(a->dir[i]==false && trigger_read(a->minTrigger+i)){
					a->busyFlag &= ~AXIS_MASK(i);
				}
			}
			if(a->busyFlag & AXIS_MASK(i)){
				a->counter[i] += a->stepNeed[i];
				if(a->counter[i] >= a->masterStep){
					a->counter[i]-=a->masterStep;
					a->hardPos[i]+=a->dir[i]?1:-1;
					a->step[i]++;
					#ifdef RT_ADVANCE
						if(i==2){
							a->extruderStep+=a->dir[i]?1:-1;
						}else{
							stepMotor_oneStep(a->motor+i);
						}
					#else
						stepMotor_oneStep(a->motor+i);
					#endif
				}
			}
		}
	}
	return a->busyFlag;
}
__STATIC_INLINE uint8_t stepAxis_isBusy(stepAxis_t *a){
	return a->busyFlag;
}

#ifdef RT_ADVANCE
void stepAxis_AdvanceISR(stepAxis_t *a);

__STATIC_INLINE void stepAxis_applyAdvance(stepAxis_t *a){
	float advance=ceilf(a->advance - a->currentAdvance);
	a->currentAdvance+=advance;
	a->extruderStep+=(int32_t)(advance);
}
__STATIC_INLINE void stepAxis_setAdvance(stepAxis_t *a,float advance){
	if(advance>300.0f){
		//rt_kprintf("stepAxisAdv:advance>200=%d\r\n",(int)(advance));
		advance=300.0f;
	}
	a->advance=advance;
	stepAxis_applyAdvance(a);
}

__STATIC_INLINE void stepAxis_resetAdvance(stepAxis_t *a){
	a->advance=0;
	a->currentAdvance=0;
}

#endif
