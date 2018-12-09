#include "libStepPlanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>	//use by Jerk(xAccel)
#include "libPlanner.h"
#include "param/param.h"

#pragma diag_suppress 117

void stepPlanner_construct(stepPlanner_t *p) {
	stepBlock_construct(&p->stepBlock);
}
void stepPlanner_init(stepPlanner_t *p){
	stepBlock_init(&p->stepBlock);
	
	//p->stage2
	for(int i=0;i<4;i++){
		p->prevSpeed[i] = 0;
	}
	p->prevExitSpeed=0;
}

static uint8_t fillBlockSteps(motionLine_t *line,int32_t steps[]) {
	line->masterStep=0;
	for(int i=0; i<STEPPER_NUM; i++) {
		line->steps[i]=abs(steps[i]);
		if(line->steps[i] > line->masterStep) {
			line->masterStep=line->steps[i];
		}
		line->dir[i]=steps[i]>0?1:0;
	}
	return 0;
}


#define block p->block
#define X_AXIS 0
#define Y_AXIS 1
#define E_AXIS 2
#define Z_AXIS 3


int log_on=0;
int slowdown_on=1;
float slowdownMinSegTime=0.05f;
float minSegTime=0.03f;


static void limitSpeed(stepPlanner_t *p);
static void limitAccelSpeed(stepPlanner_t *p);
static float calcJerkSpeed(stepPlanner_t *p);
#ifdef JOKE
static float calcThetaJerkSpeed(stepPlanner_t *p);
#endif

void stepPlanner_inputMotion(stepPlanner_t *p,int32_t steps[STEPPER_NUM],float dist[STEPPER_NUM],float speed,float length,bool useAccel,float defaultEntrySpeed,float defaultExitSpeed) {
    //apply microstep
    for(int i=0;i<STEPPER_NUM;i++){
        steps[i]*=param->microStep[i];
    }
    // Mark block as not busy (Not executed by the stepper interrupt)
	block.busy = false;
	block.useAccel = useAccel;
	block.entrySetBy=0;
	//fill stepPlanner->stage1
	rt_memcpy(p->axisDist,dist,STEPPER_NUM*sizeof(float));
	p->distance=length;
	p->nominalDt=length/speed;
	p->defaultEntrySpeed=defaultEntrySpeed;
	p->defaultExitSpeed=defaultExitSpeed;
	
	//SLOWDOWN minSegTimeLimit
	//实测这样的slowdown是没有意义的,最多起保护作用
	if(slowdown_on && p->stepBlock.len<10){	//magic number 10 = ...
		if(p->nominalDt<slowdownMinSegTime){
			float scale=(p->stepBlock.len>0)?(1.0f/((float)p->stepBlock.len)):1.0f;
			p->nominalDt=p->nominalDt + (slowdownMinSegTime - p->nominalDt)*scale;
			speed=length/p->nominalDt;
			if(log_on){
				rt_kprintf("stepPlanner:SLOWDOWN minSegTimeLimit Len=%d\r\n",p->stepBlock.len);
			}
		}
	}
			
	//fill motionLine->stage1
	fillBlockSteps(&block,steps);	
	
	{
		float masterStepFP=(float)(block.masterStep);
		for(int i=0; i < 4; i++) {
			p->curSpeed[i] = p->axisDist[i] / p->nominalDt;
			block.speedFactor[i] = (float)(abs(steps[i]))/masterStepFP;
			block.mmPerStep[i] = fabsf(dist[i])/masterStepFP;
		}
	}
	if( dist[2]==0 || (dist[0]==0 && dist[1]==0) ){
		block.extruderMoved = false;
	}else{
		block.extruderMoved = true;
	}
	
	//fill motionLine->stage2
	block.distance=p->distance;
	block.nominalSpeed=speed;
	block.nominalRate=(float)(block.masterStep)/p->nominalDt;
	
	//limit stepPlanner->stage1 & motionLine->stage2 = p->curSpeed & block->nominalSpeed/Rate
	limitSpeed(p);

	//fill & limit motionLine->stage3 = block->accelRate/Speed
	// fill motionLine->accelSpeed
	if(steps[0]!=0 || steps[1]!=0){
		block.accelSpeed=param->defaultAccelSpeed[0];
	}else if(steps[3]!=0){
		block.accelSpeed=param->defaultAccelSpeed[3];
	}else if(steps[2]!=0){
		block.accelSpeed=param->defaultAccelSpeed[2];
	}else{
		RT_ASSERT(0);
	}
	// Compute and limit the acceleration rate for the trapezoid generator.
	limitAccelSpeed(p);
	
	//fill accelRate
	{
		float stepsPerMm = block.masterStep/p->distance;
		block.accelRate = block.accelSpeed * stepsPerMm;
	}
	
	block.entrySetBy=0;
	//scale b->nominalSpeed => jerkEntrySpeed by Jerk
	float jerkEntrySpeed=block.nominalSpeed;
	#ifdef USE_THREA_JERK
	jerkEntrySpeed=calcThetaJerkSpeed(p);
	block.entrySetBy+=1;
	#endif
	jerkEntrySpeed=MIN(block.nominalSpeed,calcJerkSpeed(p));
	
	//motionLine->stage4
	if(block.nominalSpeed < p->defaultEntrySpeed ){
		block.entrySpeed = block.nominalSpeed;
		block.entrySetBy+=10;
		if(log_on){
			rt_kprintf("stepPlanner:nominalSpeed<minimumPlannerSpeed\r\n");
		}
	}else if (jerkEntrySpeed < p->defaultEntrySpeed){
		block.entrySpeed = p->defaultEntrySpeed;
		block.entrySetBy+=100;
		if(log_on){
			rt_kprintf("stepPlanner:entrySpeed<minimumPlannerSpeed\r\n");
		}
	} else {
		block.entrySpeed=jerkEntrySpeed;
		block.entrySetBy+=1000;
	}
	if(log_on){
		char temp1[10],temp2[10];
		snprintf(temp1,10,"%f",jerkEntrySpeed);
		snprintf(temp2,10,"%f",p->prevExitSpeed);
		rt_kprintf("stepPlanner:entrySpeed=%s,prevExitSpeed=%s\r\n",temp1,temp2);
	}
	//debug purpose
	block.jerkEntrySpeed = jerkEntrySpeed;

	//更新&限制 prevSpeed&prevExitSpeed
	block.nominal_length_flag=true;
	block.speedAfterAccel = speedAfterAccel(block.jerkEntrySpeed,block.accelSpeed,block.distance);
	block.speedAfterAccel = MIN(block.speedAfterAccel , block.nominalSpeed);
	p->prevExitSpeed = block.speedAfterAccel;
	block.prevExitSpeed=p->prevExitSpeed;
	{
		float scale = p->prevExitSpeed / block.nominalSpeed;
		for (uint8_t i = 0; i < STEPPER_NUM; i++){
			p->prevSpeed[i] = p->curSpeed[i]*scale;
			block.prevSpeed[i]=p->prevSpeed[i];
		}
	}
	if(block.speedAfterAccel!=block.nominalSpeed){
		block.nominal_length_flag = false;
	}
	
	{
		//为使运动停止时能减速至minmumPlannerSpeed,使新加入的block的exitSpeed=minmumPlannerSpeed,
		//并使entrySpeed限制为speedReadyForStop,这可能造成entrySpeed<jerkEntrySpeed,
		//但是在之后如果有新block加入,libPlanner的reverse_pass会检测entrySpeed<jerkEntrySpeed并把entrySpeed提升至jerkEntrySpeed
		block.speedReadyForStop = speedAfterAccel(p->defaultExitSpeed,block.accelSpeed,block.distance);
			if( block.speedReadyForStop < block.entrySpeed){
				block.entrySpeed=block.speedReadyForStop;
				block.entrySetBy+=10000;
			}
			if (block.nominalSpeed > block.speedReadyForStop){
				block.nominal_length_flag = false;
			}
	}
	block.recalculate_flag = true; // Always calculate trapezoid for new block

	{
		float entryFactor=block.entrySpeed/block.nominalSpeed;
		float exitFactor=defaultExitSpeed/block.nominalSpeed;
		exitFactor=MIN(1.0f,exitFactor);
		entryFactor=MIN(1.0f,entryFactor);
		calculate_trapezoid_for_block(&block,entryFactor,exitFactor);
	}
	
	stepBlock_inputMotion(&p->stepBlock,&block);
}

static void limitSpeed(stepPlanner_t *p){
	float speedFactor = 1.0; //factor <=1 do decrease speed
	for(int i=0; i < 4; i++) {
		if(fabsf(p->curSpeed[i]) > param->maxSpeed[i])
			speedFactor = MIN(speedFactor, param->maxSpeed[i] / fabs(p->curSpeed[i]));
	}
	// Correct the speed
	if( speedFactor < 1.0f ) {
		for(int i=0; i < 4; i++) {
			p->curSpeed[i] *= speedFactor;
		}
		block.nominalSpeed *= speedFactor;
		block.nominalRate *= speedFactor;
	}
}

static void limitAccelSpeed(stepPlanner_t *p){
	// limit acceleration per axis
	float accelFactor = 1.0f;
	float axisAccelSpeed;
	for(int i=0; i < 4; i++) {
		axisAccelSpeed = block.accelSpeed * block.speedFactor[i];
		if(axisAccelSpeed > param->maxAccelSpeed[i]){
			accelFactor = MIN(accelFactor, param->maxAccelSpeed[i] / axisAccelSpeed);
		}
	}
	block.accelSpeed = block.accelSpeed * accelFactor;
}

static float calcJerkSpeed(stepPlanner_t *p){
	float jerkSpeed = MIN(p->prevExitSpeed,block.nominalSpeed);
	if( block.nominalSpeed <= p->defaultEntrySpeed || p->prevExitSpeed <= p->defaultEntrySpeed) {
		if(log_on){
			rt_kprintf("calcJerkSpeed:jerkSpeed=MIN(prev,nominal)\r\n");
		}
	}else if( p->stepBlock.len == 0 ) {
		jerkSpeed = MIN(jerkSpeed,p->defaultEntrySpeed);
		if(log_on){
			rt_kprintf("calcJerkSpeed:len=0,minimumPlannerSpeed\r\n");
		}
	}else {
		//分解为一维deltaSpeed
		//加速:限制entrySpd的增量到maxDelta
		//减速:curSpd
		//反向:minimumSpd
		//匀速(小于maxDelta):curSpd
		float deltaSpeed,scale=1.0f;
		for (uint8_t i = 0; i < STEPPER_NUM; i++) {
			deltaSpeed = fabsf(p->curSpeed[i] - p->prevSpeed[i]);
			if( deltaSpeed < param->maxSpeedDelta[i] ){	//匀速
				continue;
			}else if( deltaSpeed + 0.001f >= fabsf(p->curSpeed[i]) + fabsf(p->prevSpeed[i])){	//反向
				scale=0;
				//jerkSpeed=MIN(jerkSpeed,param->minimumPlannerSpeed);
			}else if( fabsf(p->curSpeed[i]) > fabsf(p->prevSpeed[i]) ){	//加速
				float dirDeltaSpeed=(p->curSpeed[i] > 0.0f)?param->maxSpeedDelta[i] : -param->maxSpeedDelta[i];
				float newSpeed=p->prevSpeed[i] + dirDeltaSpeed;
				scale=MIN(scale,newSpeed/p->curSpeed[i]);
				if(scale>1.0f){
					RT_ASSERT(0);
				}
			}else{	//减速
				continue;
			}
		}
		jerkSpeed *=scale;
	}
	jerkSpeed = MAX(p->defaultEntrySpeed,jerkSpeed);
	if(log_on){
		float scale=jerkSpeed/block.nominalSpeed;
		char temp[10];
		snprintf(temp,10,"%f",scale);
		int j=jerkSpeed,n=block.nominalSpeed;
		rt_kprintf("calcJerkScale:entry=%d,norm=%d,scale=%s\r\n",j,n,temp);
	}
	return jerkSpeed;
}


