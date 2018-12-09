#include "libStepGCode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_atof.h"
#include "app/app_retarget.h"
#include "app/app_subboard.h"
#include "lib/param/param.h"

#if STEPPER_NUM==6
static const char
axisCode[STEPPER_NUM]={'X','Y','E','A','B','C'};
#elif STEPPER_NUM==5
static const char
axisCode[STEPPER_NUM]={'X','Y','E','A','B'};
#elif STEPPER_NUM==4
static const char
axisCode[STEPPER_NUM]={'X','Y','E','A'};
#else
#error "STEPPER_NUM error"
#endif


void stepGCodeProcessor_init(stepGCode_t *g){
	g->speed=10.0f;
	g->G1UseAccel=true;
	g->G0SpeedFactor=1.0f;
	g->G1SpeedFactor=1.0f;
	for(int i=0;i<STEPPER_NUM;i++){
		g->seen[i]=false;
		g->relative[i]=false;
		g->value[i]=0.0f;
	}
}

static float CodeFP(stepGCode_t *g)
{
	return my_atof(g->codeLp);
}

static int CodeLong(stepGCode_t *g)
{
	return atoi(g->codeLp);		//没有参数默认为0
}

static bool CodeSeek(stepGCode_t *g,char code)
{
	char *lp = strchr(g->lineBuffer , code);
	if(lp){
		g->codeLp=lp+1;
		return true;
	}else{
		return false;
	}
}
static void Trim(char *str){
	char *lp = strchr(str,';');
	if(lp){
		lp[0]='\0';
	}
	lp = strchr(str,'(');
	if(lp){
		lp[0]='\0';
	}
}
static void ReadAxis(stepGCode_t *g){
	if(CodeSeek(g,'F')) {
		g->speed=CodeFP(g)/60.0f;		//direct use Feedrate as mm/s
	}
	for(int8_t i=0; i < STEPPER_NUM; i++) {
		if(CodeSeek(g,axisCode[i]))
		{
			g->seen[i]=true;
			g->value[i]=CodeFP(g);
		}else{
			g->seen[i]=false;
			g->value[i]=0;
		}
	}
	if(CodeSeek(g,'Z')){
		for(int8_t i=3;i<STEPPER_NUM;i++){
			g->seen[i]=true;
			g->value[i]=CodeFP(g);
		}
	}
}

void stepGCodeProcessor_run(stepGCode_t *g){
	int ret=1;
	Trim(g->lineBuffer);
	//Seek GCode
	if(CodeSeek(g,'G'))
	{
		long code=CodeLong(g);
		switch(code)
		{
			case 0: // G0
				ReadAxis(g);
				stepPosition_inputNextPosition(&g->stepPos,g->value,g->relative,g->seen,g->speed*g->G0SpeedFactor,1);
				break;
			case 1: // G1 Move
				ReadAxis(g);
				stepPosition_inputNextPosition(&g->stepPos,g->value,g->relative,g->seen,g->speed*g->G1SpeedFactor,g->G1UseAccel);
				break;
			case 21:
				break;
			case 28:	//Home Axis
				g->stepPos.planner.stepBlock.stepAxis.enableTrigger=true;
				ReadAxis(g);
				stepPosition_inputNextPosition(&g->stepPos,g->value,g->relative,g->seen,g->speed,1);
				while(RT_EOK!=stepBlock_waitForIdle(&g->stepPos.planner.stepBlock,RT_WAITING_FOREVER)){;}
				g->stepPos.planner.stepBlock.stepAxis.enableTrigger=false;
				break;
			case 90: // G90
				for(int i=0;i<STEPPER_NUM;i++)
					g->relative[i]=false;
				break;
			case 91: // G91
				for(int i=0;i<STEPPER_NUM;i++)
					g->relative[i]=true;
				break;
			case 92: // G92
				ReadAxis(g);
				{
					bool relative[STEPPER_NUM];
					for(int i=0;i<STEPPER_NUM;i++){
						relative[i]=false;
					}
					stepPosition_setMm(&g->stepPos,g->value,relative,g->seen);
				}
				break;
			default:
				rt_kprintf("ok unhandled GCode -> %s\r\n",g->lineBuffer);
				ret=0;
				break;
		}
	}else if(CodeSeek(g,'M')){
		long code=CodeLong(g);
		switch(code)
		{
			case 0:	//M0 :emergency stop
			case 84:	//Stop Idle Hold
				stepPosition_stop(&g->stepPos);
				//stepBlock_setEnableAll(&g->stepPos.planner.stepBlock,false);
				break;
			case 17:
				stepBlock_setEnableAll(&g->stepPos.planner.stepBlock,true);
				break;
			case 1:	//M1 : wait for idle & disable motor
			case 18://M18 : Wait for idle & disable all motor
				while(RT_EOK!=stepBlock_waitForIdle(&g->stepPos.planner.stepBlock,RT_WAITING_FOREVER)){;}
				stepBlock_setEnableAll(&g->stepPos.planner.stepBlock,false);
				break;
			case 2:	//M2 : wait for idle
				while(RT_EOK!=stepBlock_waitForIdle(&g->stepPos.planner.stepBlock,RT_WAITING_FOREVER)){;}
				break;
			case 10:
				ReadAxis(g);
				stepBlock_setDisable(&g->stepPos.planner.stepBlock,g->seen);
				break;
			case 11:
				ReadAxis(g);
				stepBlock_setEnable(&g->stepPos.planner.stepBlock,g->seen);
				break;
			case 12:
				ReadAxis(g);
				break;
			case 82:
				g->relative[2]=false;
				break;
			case 83:
				g->relative[2]=true;
				break;
			case 92:	//set steps per mm
				break;
			case 93:	//get steps per mm
				break;
			case 110:
				break;
			case 104:
			{
				if(CodeSeek(g,'S')){
					sboard_setTemper(CodeLong(g));
				}else{
					sboard_setTemper(0);
				}
				break;
			}
			case 105:		//get temperaure
			{
				rt_sprintf(g->lineBuffer,"T:%d",sboard_getTemper());
				ret=2;
				break;
			}
			case 106:
			case 245:
				if(CodeSeek(g,'S')){
					sboard_setFan1(CodeLong(g));
				}else{
					sboard_setFan1(80);
				}
				break;
			case 107:
			case 246:
				sboard_setFan1(0);
				break;
			case 109:		//M109 ; M104 + wait for temperature to be reached
			{
				if(CodeSeek(g,'S')){
					sboard_setTemper(CodeLong(g));
				}
				while(1){
					if(sboard_getTemper() + 2 > sboard_getTemperDst() )break;
					rt_thread_delay(10);
				}
				break;
			}
			case 114:
			{
				int sboard_getTemper(void);
				#if defined(TDPRINTER_A)
				sprintf(g->lineBuffer,"X:%.3f Y:%.3f E:%.3f A:%.3f B:%.3f C:%.3f",g->stepPos.mm[0],g->stepPos.mm[1],g->stepPos.mm[2],g->stepPos.mm[3],g->stepPos.mm[4],g->stepPos.mm[5]);
				#else
				sprintf(g->lineBuffer,"X:%.3f Y:%.3f E:%.3f A:%.3f",g->stepPos.mm[0],g->stepPos.mm[1],g->stepPos.mm[2],g->stepPos.mm[3]);
				#endif
				ret=2;
				break;
			}
			case 140:
				break;
			case 420:	//LED ON/OFF
				break;
			case 443:
				g->G1UseAccel=true;
				break;
			case 444:
				g->G1UseAccel=false;
				break;
			case 591:
				ReadAxis(g);
				{
					for(int i=0;i<STEPPER_NUM;i++){
						if(g->seen[i]==true){
							param->positionOffset[i]=g->value[i];
						}
					}
				}
				ret=1;
				break;
			case 592:
				{
					bool relative[STEPPER_NUM],seen[STEPPER_NUM];
					for(int i=0;i<STEPPER_NUM;i++){
						relative[i]=true;
						seen[i]=true;
					}
					stepPosition_setMm(&g->stepPos,param->positionOffset,relative,seen);
				}
				ret=1;
				break;
			case 801:
				ReadAxis(g);
				for(int i=0;i<4;i++){
					if(g->seen[i]==true){
						param->defaultAccelSpeed[i]=g->value[i];
					}
				}
				ret=1;
				break;
			case 802:
				ReadAxis(g);
				for(int i=0;i<4;i++){
					if(g->seen[i]==true){
						param->maxSpeed[i]=g->value[i];
					}
				}
				ret=1;
				break;
			case 803:
				ReadAxis(g);
				for(int i=0;i<4;i++){
					if(g->seen[i]==true){
						param->maxAccelSpeed[i]=g->value[i];
					}
				}
				ret=1;
				break;
			case 804:
				ReadAxis(g);
				for(int i=0;i<4;i++){
					if(g->seen[i]==true){
						param->maxSpeedDelta[i]=g->value[i];
					}
				}
				ret=1;
				break;
			case 805:
				ReadAxis(g);
				for(int i=0;i<4;i++){
					if(g->seen[i]==true){
						param->defaultEntrySpeed[i]=g->value[i];
					}
				}
				ret=1;
				break;
			case 806:
				ReadAxis(g);
				if(g->seen[0]==true){
					g->G0SpeedFactor=g->value[0];
				}
				ret=1;
				break;
			case 807:
				ReadAxis(g);
				if(g->seen[0]==true){
					g->G1SpeedFactor=g->value[0];
				}
				ret=1;
				break;
			case 808:
				ReadAxis(g);
				for(int i=0;i<STEPPER_NUM;i++){
					if(g->seen[i]==true){
						param->mm2step[i]=g->value[i];
					}
				}
				ret=1;
				break;
			case 809:
				ReadAxis(g);
				if(g->seen[0]==true){
					param->advanceK=g->value[0];
				}
				ret=1;
				break;
			case 810:
				ReadAxis(g);
				if(g->seen[0]==true){
					param->advanceDeprime=g->value[0];
				}
				ret=1;
				break;
			case 995:
				if(param_reset()==0){
					ret=1;
				}else{
					sprintf(g->lineBuffer,"param reset failed");
					ret=2;
				}
				break;
			case 996:
				if(param_load()==0){
					ret=1;
				}else{
					sprintf(g->lineBuffer,"param load failed");
					ret=2;
				}
				break;
			case 997:
				if(param_save()==0){
					ret=1;
				}else{
					sprintf(g->lineBuffer,"param save failed");
					ret=2;
				}
				break;
			case 998:
				__disable_irq();
				NVIC_SystemReset();
				while(1){;}
			case 999:
				retarget_finshMode(true);
				ret=1;
				break;
			default:
				rt_device_write(g->device[g->currentDevice],0,"ok unhandled MCode -> %s\r\n",0);
				ret=0;
				break;
		}
	}
	switch(ret){
		case 0:
			break;
		case 1:
			rt_device_write(g->device[g->currentDevice],0,"ok\r\n",0);
			break;
		case 2:
			rt_device_write(g->device[g->currentDevice],0,"ok ",0);
			rt_device_write(g->device[g->currentDevice],0,g->lineBuffer,0);
			rt_device_write(g->device[g->currentDevice],0,"\r\n",0);
			break;
		default:
			rt_device_write(g->device[g->currentDevice],0,"ok UNKNOW ERROR\r\n",0);
		break;
	}
}

