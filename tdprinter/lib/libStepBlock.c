#include "libStepBlock.h"
#include "libStepTimer.h"
#include "libPlanner.h"

void stepBlock_construct(stepBlock_t *b){
	b->event=0;
	stepAxis_construct(&b->stepAxis);
}

void stepBlock_init(stepBlock_t *b){
	stepAxis_init(&b->stepAxis);
	stepBlock_reset(b);
	if(b->event){
		rt_event_delete(b->event);
	}
	b->event=rt_event_create("evtStepAxis",RT_IPC_FLAG_FIFO);
	rt_event_send(b->event,STEP_EVENT_BLOCK_IDLE);
	rt_event_send(b->event,STEP_EVENT_BLOCK_COMPLETE);
	b->currentBuffer=0;
	//used for libStepBlock DDA step
	extern tim_base_t tim1;
	tim_init(&tim1);
	tim_setITUpdate(&tim1,ENABLE);
	tim_setEnable(&tim1,ENABLE);
}
void stepBlock_reset(stepBlock_t *b){
	b->head=0;
	b->tail=0;
	b->len=0;
	stepAxis_reset(&b->stepAxis);
}
__STATIC_INLINE void inc(int32_t *num){
	num[0]++;
	if(num[0] >= MOTION_BUFFER_SIZE){
		num[0]=0;
	}
}

#define currentLine (b->currentLine)
#define timeElasped (b->timeElasped)
#define currentRate (b->currentRate)
#define masterCounter (b->masterCounter)

static bool stepBlock_loadNextMotion(stepBlock_t *b){
	if(b->len>0){
		currentLine=b->buffer[b->tail];
		b->currentBuffer=b->buffer+b->tail;
		#ifdef JOKE
		if(b->buffer[b->tail].busy==true){
			goto LABEL_STOP_BLOCK;
		}
		b->buffer[b->tail].busy=true;
		#endif
		stepAxis_reload(&b->stepAxis,currentLine.masterStep,currentLine.steps,currentLine.dir);
		inc(&b->tail);
		b->len--;
		rt_event_control(b->event,RT_IPC_CMD_RESET,0);
		timeElasped=0;
		if(currentLine.useAccel){
			currentRate=currentLine.entryRate;
		}else{
			currentRate=currentLine.nominalRate;
		}
		#ifdef RT_ADVANCE
		if(currentLine.extruderMoved){
			float advance=param->advanceK*currentRate*currentLine.mmPerStep[2]*currentRate*currentLine.mmPerStep[2]*0.1f;
			stepAxis_setAdvance(&b->stepAxis,advance);
		}else{
			stepAxis_setAdvance(&b->stepAxis,-param->advanceDeprime);
		}
		#endif
		masterCounter=0;
		stepTimer_setRate(currentRate);
		stepTimer_setEnable(true);
		return true;
	}else{
		stepTimer_setEnable(false);
		return false;
	}
}

int error_inputBlock_notsync=0;
void stepBlock_inputMotion(stepBlock_t *b,motionLine_t *block){
	//MOTION_BUFFER_SIZE - 1 为了与libPlanner库兼容
	if(b->len>=MOTION_BUFFER_SIZE-1){
		while(b->len>=MOTION_BUFFER_SIZE-1){
			rt_event_recv(b->event,STEP_EVENT_BLOCK_COMPLETE,RT_EVENT_FLAG_AND|RT_EVENT_FLAG_CLEAR,10,0);
		}
	}else{
		rt_event_recv(b->event,STEP_EVENT_BLOCK_COMPLETE,RT_EVENT_FLAG_AND|RT_EVENT_FLAG_CLEAR,0,0);
	}
	if(b->len<MOTION_BUFFER_SIZE-1){
		b->buffer[b->head]=block[0];
		inc(&b->head);
		b->len++;
		//recalculate planner
		if(block[0].useAccel){
			planner_recalculate(b);
		}
		if(RT_EOK==rt_event_recv(b->event,STEP_EVENT_BLOCK_IDLE,RT_EVENT_FLAG_AND|RT_EVENT_FLAG_CLEAR,0,0)){
			stepBlock_loadNextMotion(b);
		}
	}else{
		error_inputBlock_notsync++;
		rt_kprintf("stepBlock_inputMotion:not sync++\r\n");
	}
}

rt_err_t stepBlock_waitForIdle(stepBlock_t *b,rt_int32_t tick){
	return rt_event_recv(b->event,STEP_EVENT_BLOCK_IDLE,RT_EVENT_FLAG_AND,tick,0);
}

//必须由上层保证inputMotion没有在阻塞
void stepBlock_forceStop(stepBlock_t *b){
	stepTimer_setEnable(false);
	//clear IDLE to prevent loadNextMotion exec
	rt_event_recv(b->event,STEP_EVENT_BLOCK_IDLE,RT_EVENT_FLAG_AND|RT_EVENT_FLAG_CLEAR,0,0);
	//let inputMotion unblock
	b->len=0;
	rt_event_send(b->event,STEP_EVENT_BLOCK_COMPLETE);
	rt_thread_delay(1);
	//true stop
	rt_event_send(b->event,STEP_EVENT_BLOCK_COMPLETE);
	rt_event_send(b->event,STEP_EVENT_BLOCK_IDLE);
	b->head=0;
	b->tail=0;
	b->len=0;
}

void stepBlock_setEnableAll(stepBlock_t *b,bool en){
	bool set[STEPPER_NUM];
	for(int i=0;i<STEPPER_NUM;i++)set[i]=true;
	if(en){
		stepBlock_setEnable(b,set);
	}else{
		stepBlock_setDisable(b,set);
	}
}

/*
Timer ISR 在motionBuffer非空时使能
在motionBuffer空时禁用
Timer使能Preload缓冲，在当前tick缓存下一个tick的周期
*/
int error_stepBlockOverrun=0;
void stepBlock_ISR(stepBlock_t *b){
	if(stepBlock_isBusy(b)){	//防止所有步数为0的状态
		stepBlock_step(b);
		masterCounter++;
		
	}else{
		//rt_kprintf("stepBlock_ISR:overrun++\r\n");
		error_stepBlockOverrun++;
	}
	if(currentLine.useAccel){
		float dt=stepTimer_getDT();
		timeElasped+=dt;
		if( masterCounter < currentLine.accelBefore ){
			currentRate += currentLine.accelRate*dt;		//误差会积累,暂时这样
			if(currentRate > currentLine.nominalRate)currentRate=currentLine.nominalRate;
			currentLine.trueNominalRate=currentRate;
		}else 
		if( masterCounter >= currentLine.decelAfter  ){
			currentRate -= currentLine.accelRate*dt;
			if(currentRate < currentLine.exitRate)currentRate=currentLine.exitRate;
			currentLine.trueExitRate=currentRate;
		}else{
			currentRate = currentLine.nominalRate;
		}
		stepTimer_setRate(currentRate);
	}
	#ifdef RT_ADVANCE
	if(currentLine.extruderMoved){
		float advance=param->advanceK*currentRate*currentLine.mmPerStep[2]*currentRate*currentLine.mmPerStep[2]*0.1f;
		stepAxis_setAdvance(&b->stepAxis,advance);
	}
	#endif
	if(stepBlock_isBusy(b)==0){
		b->currentBuffer[0].trueNominalRate=currentLine.trueNominalRate;
		b->currentBuffer[0].trueExitRate=currentLine.trueExitRate;
		b->currentBuffer[0].trueTime=timeElasped;
		if(stepBlock_loadNextMotion(b)){
			rt_event_send(b->event,STEP_EVENT_BLOCK_COMPLETE);
		}else{
			rt_event_send(b->event,STEP_EVENT_BLOCK_COMPLETE);
			rt_event_send(b->event,STEP_EVENT_BLOCK_IDLE);
		}
	}
}
#include <finsh.h>
int stepBlock_getErr(void){
	return error_stepBlockOverrun;
}
FINSH_FUNCTION_EXPORT_ALIAS(stepBlock_getErr,sbGetErr,"stepBlock_getOverRunErr");








