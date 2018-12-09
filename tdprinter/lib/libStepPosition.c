#include <arm_math.h>
#include "libStepPosition.h"
#include <stdlib.h>

#define MINIMUM_MOTION_STEP 10
#define MINIMUM_POSITION_SPEED 0.1f
#define LENGTH_ENTRY_SPEED_SLOWDOWN 4.0f

//private
void stepPosition_setMmDst(stepPosition_t *p,float mm[],bool relative[],bool mask[]);
void stepPosition_resetFromHardware(stepPosition_t *p);
void stepPosition_storeToHardware(stepPosition_t *p);
void stepPosition_updateSrc(stepPosition_t *p);
float stepPosition_getOffsetLength(stepPosition_t *p,int32_t out_step[]);
void stepPosition_setStep2Mm(stepPosition_t *p,float step2mm[]);

//输入下一个位置到motionBuffer
//用于gcode解析器调用
void stepPosition_inputNextPosition(stepPosition_t *p,float pos[],bool relative[],bool mask[],float speed,bool useAccel){
	//输入位置到p->mmDst
	stepPosition_setMmDst(p,pos,relative,mask);
	{
		int32_t stepOffset[STEPPER_NUM],stepLength=0;
		float length;
		//多功能函数,根据输入p->mmDst 输出 length(mm主移动距离) stepOffset(int各轴移动步数)
		//length主要用于planner中dt的解算,dt=length/speed
		length=stepPosition_getOffsetLength(p,stepOffset);
		for(int i=0;i<STEPPER_NUM;i++){
			stepLength+=abs(stepOffset[i]);
		}
		//保证steps不全为0
		if(stepLength > 0){
			//记录 有效(挤出料) 移动的距离
			if(stepOffset[2]>0){
				p->distanceMoved+=length;
			}
			if(speed<MINIMUM_POSITION_SPEED){
				speed=MINIMUM_POSITION_SPEED;
			}
			if(stepOffset[3]!=0){
				stepPlanner_inputMotion(&p->planner,stepOffset,p->mmOffset,speed,length,useAccel,param->defaultEntrySpeed[3],param->defaultEntrySpeed[3]);
			}else if(stepOffset[0]==0 && stepOffset[1]==0){
				stepPlanner_inputMotion(&p->planner,stepOffset,p->mmOffset,speed,length,useAccel,param->defaultEntrySpeed[2],param->defaultEntrySpeed[2]);
			}else{
				if(length>LENGTH_ENTRY_SPEED_SLOWDOWN){
					stepPlanner_inputMotion(&p->planner,stepOffset,p->mmOffset,speed,length,useAccel,param->defaultEntrySpeed[0],param->defaultEntrySpeed[0]);
				}else{
					float smallMotionEntrySpeed=param->defaultEntrySpeed[0]*(length/LENGTH_ENTRY_SPEED_SLOWDOWN);
					if(smallMotionEntrySpeed<MINIMUM_POSITION_SPEED)smallMotionEntrySpeed=MINIMUM_POSITION_SPEED;
					
					stepPlanner_inputMotion(&p->planner,stepOffset,p->mmOffset,speed,length,useAccel,smallMotionEntrySpeed,smallMotionEntrySpeed);
				}
			}
			stepPosition_updateSrc(p);
		}else{
			//rt_kprintf("stepPos:drop step %d,%d,%d,%d\r\n",steps[0],steps[1],steps[2],steps[3]);
			return;
		}
	}
}
void stepPosition_stop(stepPosition_t *p){
	stepBlock_forceStop(&p->planner.stepBlock);
	stepPosition_resetFromHardware(p);
	stepBlock_setEnableAll(&p->planner.stepBlock,false);
}


static void mm2step(stepPosition_t *p,float mm[],int32_t step[]){
	for(int i=0;i<STEPPER_NUM;i++){
		step[i]=lround(p->mm2step[i]*mm[i]);
	}
}
static void step2mm(stepPosition_t *p,int32_t step[],float mm[]){
	for(int i=0;i<STEPPER_NUM;i++){
		mm[i]=step[i]/p->mm2step[i];
	}
}

void stepPosition_construct(stepPosition_t *p){
	for(int i=0;i<STEPPER_NUM;i++){
		p->mm2step[i]=param->mm2step[i];
	}
	stepPlanner_construct(&p->planner);
}

void stepPosition_init(stepPosition_t *p){
	bool relative[STEPPER_NUM];
	bool mask[STEPPER_NUM];
	float mm[STEPPER_NUM];
	for(int i=0;i<STEPPER_NUM;i++){
		relative[i]=false;
		mask[i]=true;
		mm[i]=0;
	}
	p->distanceMoved=0;
	stepPosition_setMm(p,mm,relative,mask);
	stepPosition_setMmDst(p,mm,relative,mask);
	stepPlanner_init(&p->planner);
}
void stepPosition_setMm(stepPosition_t *p,float mm[],bool relative[],bool mask[]){
	for(int i=0;i<STEPPER_NUM;i++){
		if(mask[i]){
			if(relative[i]){
				p->mm[i]+=mm[i];
			}else{
				p->mm[i] =mm[i];
			}
		}
	}
	mm2step(p,p->mm,p->step);
	//sync -> hardPos
	stepPosition_storeToHardware(p);
}
void stepPosition_setMmDst(stepPosition_t *p,float mm[],bool relative[],bool mask[]){
	for(int i=0;i<STEPPER_NUM;i++){
		if(mask[i]){
			if(relative[i]){
				p->mmDst[i]+=mm[i];
			}else{
				p->mmDst[i] =mm[i];
			}
		}
	}
}

void stepPosition_resetFromHardware(stepPosition_t *p){
	for(int i=0;i<STEPPER_NUM;i++){
		p->step[i]=p->planner.stepBlock.stepAxis.hardPos[i];
	}
	step2mm(p,p->step,p->mm);
	step2mm(p,p->step,p->mmDst);
}
void stepPosition_storeToHardware(stepPosition_t *p){
	for(int i=0;i<STEPPER_NUM;i++){
		p->planner.stepBlock.stepAxis.hardPos[i]=p->step[i];
	}
	step2mm(p,p->step,p->mm);
	step2mm(p,p->step,p->mmDst);
}

//更新mm和int位置 到 目的位置
void stepPosition_updateSrc(stepPosition_t *p){
	memcpy(p->step,p->stepDst,sizeof(p->step));
	memcpy(p->mm,p->mmDst,sizeof(p->mm));
}


//XYZ轴优先计算笛卡尔坐标长度,ABC最长的记为Z轴长度
//单独E轴直接为长度
float stepPosition_getOffsetLength(stepPosition_t *p,int32_t out_step[]){
	int moveAxis=0;
	//将 mmDst 映射到 intDst 上
	mm2step(p,p->mmDst,p->stepDst);
	for(int i=0;i<STEPPER_NUM;i++){
		//计算出需要移动的int步数
		out_step[i]=p->stepDst[i] - p->step[i];
		//计算出gcode中这次移动的mm偏移值,用于planner的advance和jerk?解算
		p->mmOffset[i]=p->mmDst[i] - p->mm[i];
		if(out_step[i]!=0){
			moveAxis++;
		}
	}
	{
		//如果只有一个轴移动,主移动距离默认
		if(moveAxis==1){
			for(int i=0;i<STEPPER_NUM;i++){
				if(out_step[i]!=0){
					return fabsf(p->mmOffset[i]);
				}
			}
		//如果多个轴移动,计为XYZ 3轴的笛卡尔距离
		}else if(moveAxis>1){	//cartesianAxis XYZ
			float cartesianAxis[3]={p->mmOffset[0],p->mmOffset[1],0};
			//因为TDB_A型有3个Z轴,Z轴记为ABC中最长的一个
			for(int i=3;i<STEPPER_NUM;i++){
				if(fabsf(p->mmOffset[i])>cartesianAxis[2])cartesianAxis[2]=fabsf(p->mmOffset[i]);
			}
			{
				float32_t pow,result;
				arm_power_f32(cartesianAxis,3,&pow);
				arm_sqrt_f32(pow,&result);
				return result;
			}
		}
	}
	return 1.0f;
}


