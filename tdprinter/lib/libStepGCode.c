#include "libStepGCode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libStepGCodeProcessor.h"


void stepGCode_construct(stepGCode_t *g){
	g->event=rt_event_create("GCodeEvent",RT_IPC_FLAG_FIFO);
	stepPosition_construct(&g->stepPos);
	g->thread=0;
}
void stepGCode_thread(void *root);
void stepGCode_init(stepGCode_t *g){
	stepGCode_reset(g);
	stepPosition_init(&g->stepPos);
}

void stepGCode_reset(stepGCode_t *g){
	rt_event_control(g->event,RT_IPC_CMD_RESET,0);
	rt_event_send(g->event,GCODE_EVENT_STOPPED|GCODE_EVENT_BUFFER_EMPTY);
	g->device[0]=0;
	g->device[1]=0;
	g->lineBuffer[0]=0;
	g->codeLp=g->lineBuffer;
	g->lineLength=0;
	g->thread=0;
	stepGCodeProcessor_init(g);
}

int32_t stepGCode_start(stepGCode_t *g,int id,rt_device_t device){
	if(id<2 && RT_EOK==rt_event_recv(g->event,GCODE_EVENT_STOPPED,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,0)){
		g->device[id]=device;
		g->currentDevice=id;
		if(g->thread==0){
			g->thread=rt_thread_create("gcodeThread",stepGCode_thread,g,THREAD_GCODE_STACK_SIZE,THREAD_GCODE_PRIORITY,1);
			rt_thread_startup(g->thread);
			rt_event_send(g->event,GCODE_EVENT_STARTED);
		}
		return RT_EOK;
	}else{
		rt_kprintf("stepGCode_start:id=%d!<2 OR not STOPED\r\n",id);
	}
	return RT_ERROR;
}

int32_t stepGCode_stop(stepGCode_t *g){
	if(g->thread){
		rt_thread_delete(g->thread);
		g->thread=0;
	}else{
		rt_kprintf("stepGCode_stop:already stop\r\n");
	}
	stepGCode_reset(g);
	stepPosition_stop(&g->stepPos);	//stop motion & read position from hardware	
	return RT_ERROR;
}
//reserved
int32_t stepGCode_pause(stepGCode_t *g,bool act){
	rt_kprintf("stepGCode_pause:not implement\r\n");
	return RT_ERROR;
}

static int32_t getLine(stepGCode_t *g,rt_device_t dev){
	uint8_t c;
	g->lineLength=0;
	while(1){
		if(rt_device_read(dev,0,&c,1)==1){
			g->lineBuffer[g->lineLength]=c;
			if(g->lineLength < GCODE_BUFFER_SIZE){
				g->lineLength++;
			}
			if(c=='\r' || c=='\n' || c=='\0'){
				if(g->lineLength<3){	//sizeof("\r\n")=3
					return 0;
				}else{
					g->lineBuffer[g->lineLength]='\0';
					return 1;
				}
			}
		}else{
			rt_thread_delay(1);
		}
	}
}

void stepGCode_thread(void *root){
	stepGCode_t *g=(stepGCode_t*)root;
	while(1){
		if(RT_EOK==rt_event_recv(g->event,GCODE_EVENT_STARTED,RT_EVENT_FLAG_AND,1,0)){
			if(g->device[g->currentDevice]){
				if(getLine(g,g->device[g->currentDevice])){
					stepGCodeProcessor_run(g);		//see @ libStepGCodeProcessor.c
				}
			}else{
				rt_thread_delay(1);
			}
		}
	}
}

