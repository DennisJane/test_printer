#include "stm32f4xx.h"
#include "libStepAxis.h"

void stepAxis_construct(stepAxis_t *a){
	extern stepMotor_t defaultMotor[];
	extern trigger_t defaultMaxTrigger[],defaultMinTrigger[];
	a->motor=defaultMotor;
	a->maxTrigger=defaultMaxTrigger;
	a->minTrigger=defaultMinTrigger;
	a->enableTrigger=false;
}

void stepAxis_setTrigger(stepAxis_t *a,trigger_t *maxTrigger,trigger_t *minTrigger){
	a->maxTrigger=maxTrigger;
	a->minTrigger=minTrigger;
}

void stepAxis_init(stepAxis_t *a){
	for(int i=0;i<STEPPER_NUM;i++){
		trigger_init(a->maxTrigger+i);
		trigger_init(a->minTrigger+i);
		stepMotor_init(a->motor+i);
		a->hardPos[i]=0;
	}
	stepAxis_reset(a);
	#ifdef RT_ADVANCE
	//used for libStepAxis extruder advance step 10khz
	extern tim_base_t tim4;
	tim_init(&tim4);
	tim_setITUpdate(&tim4,ENABLE);
	tim_setEnable(&tim4,ENABLE);
	#endif
}

void stepAxis_reset(stepAxis_t *a){
	a->masterStep=0;
	a->busyFlag=0;
	for(int i=0;i<STEPPER_NUM;i++){
		a->counter[i]=0;
		a->step[i]=0;
		a->stepNeed[i]=0;
		a->dir[i]=false;
	}
	a->enableTrigger=false;
	#ifdef RT_ADVANCE
	a->advance=0;
	a->currentAdvance=0;
	a->extruderStep=0;
	#endif
}
void stepAxis_setHardPos(stepAxis_t *a,int32_t hardPos[]){
	for(int i=0;i<STEPPER_NUM;i++){
		a->hardPos[i]=hardPos[i];
	}
}
int32_t* stepAxis_getHardPos(stepAxis_t *a){
	return a->hardPos;
}
void stepAxis_setEnable(stepAxis_t *a,bool en[STEPPER_NUM]){
	for(int i=0;i<STEPPER_NUM;i++){
		if(en[i]){
			stepMotor_setEnable(a->motor+i,true);
		}
	}
}
void stepAxis_setDisable(stepAxis_t *a,bool dis[STEPPER_NUM]){
	for(int i=0;i<STEPPER_NUM;i++){
		if(dis[i]){
			stepMotor_setEnable(a->motor+i,false);
		}
	}
}

#ifdef RT_ADVANCE
//10khz ISR
void stepAxis_AdvanceISR(stepAxis_t *a){
	if(a->extruderStep>0){
		a->extruderStep--;
		stepMotor_setDir(a->motor+2,true);
		stepMotor_oneStep(a->motor+2);
	}else if(a->extruderStep<0){
		a->extruderStep++;
		stepMotor_setDir(a->motor+2,false);
		stepMotor_oneStep(a->motor+2);
	}
}

#endif

