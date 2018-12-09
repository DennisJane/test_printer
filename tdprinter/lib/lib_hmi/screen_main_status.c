#include "hmi_hal.h"
#include <dfs_posix.h>
#include "screen_main.h"
#include "hmi_button.h"
#include "hmi_list.h"
#include "lib/corePrinter.h"
#include "app/app_subboard.h"

void status_hmi_chang(bool flag);

extern button_t item[ITEM_NUM];
extern button_t navi[NAVI_NUM];

extern corePrinter_t cp;
extern void switch_dialog(int flag,char *str);

static void cancelPrint(void){
	switch_dialog(1,"");
}
static void timeUsedCallback(hmi_item_t *u){
	int second=cp.timeUsed/100;
	rt_snprintf(u->value,LIST_VALUE_LEN,"%dh:%dm:%ds",second/3600,(second/60)%60,second%60);
}
static void temperCallback(hmi_item_t *u){
	rt_snprintf(u->value,LIST_VALUE_LEN,"%d",sboard_getTemper());
}
static void temperDstCallback(hmi_item_t *u){
	rt_snprintf(u->value,LIST_VALUE_LEN,"%d",sboard_getTemperDst());
}


float gFlowRatio=1.0f;
static void flowratioP1(hmi_item_t *u){
	gFlowRatio+=0.01f;
	stepPosition_t *sp=(stepPosition_t*)&cp;
	sp->mm2step[2]=gFlowRatio*param->mm2step[2];
}
static void flowratioM1(hmi_item_t *u){
	if(gFlowRatio<0.2f)return;
	gFlowRatio-=0.01f;
	stepPosition_t *sp=(stepPosition_t*)&cp;
	sp->mm2step[2]=gFlowRatio*param->mm2step[2];
}
static void flowratioP10(hmi_item_t *u){
	gFlowRatio+=0.10f;
	stepPosition_t *sp=(stepPosition_t*)&cp;
	sp->mm2step[2]=gFlowRatio*param->mm2step[2];
}
static void flowratioM10(hmi_item_t *u){
	if(gFlowRatio<0.2f)return;
	gFlowRatio-=0.10f;
	stepPosition_t *sp=(stepPosition_t*)&cp;
	sp->mm2step[2]=gFlowRatio*param->mm2step[2];
}
static void flowratioE0(hmi_item_t *u){
	gFlowRatio=1.0f;
	stepPosition_t *sp=(stepPosition_t*)&cp;
	sp->mm2step[2]=gFlowRatio*param->mm2step[2];
}

static int8_t triggers[6]={0,0,0,0,0,0};

static hmi_item_t items[]={
	
	//page1
	{"文件名:",STRING,&cp.fileName,},
	{"进度:",S32,&cp.printFileComplete,},
	{"温度:",CALLBACK,temperCallback,},
	{"目的温度:",CALLBACK,temperDstCallback,},
	{"用时:",CALLBACK,timeUsedCallback,},
	{"暂无打印",FUNC,NULL,},
	//{"取消打印",FUNC,cancelPrint,},
	
	//page2
	{"bufLen:",S32,&cp.gcodeDevice.stepPos.planner.stepBlock.len,},
	{"distance:",FP,&cp.gcodeDevice.stepPos.distanceMoved,},
	{"totalDistance:",DFP,&cp.totalDistance,},
};

void status_hmi_chang(bool flag){				//1-打印中;0-无打印
	if(flag)
	{
		items[5].name="取消打印";       ////打印中，状态里显示取消打印
		items[5].lp=cancelPrint;
	}
	else
	{
		items[5].name="暂无打印";      
		items[5].lp=NULL;		
	}
}

hscreen_t statusItem;
static hmi_list_t list={
	&statusItem,	//parent
	items,
	sizeof(items)/sizeof(hmi_item_t),ITEM_NUM,
	item,navi,
};

typedef struct{
	int sid;
	hmi_list_t *list;
}userdata_t;

static userdata_t userData={0,&list};

static void onLoad(hscreen_t *s);
static void onTimer(hscreen_t *s);

hscreen_t statusItem={
	0,		//id
	0,		//prev
	onLoad,
	0,
	onTimer,
	&userData,		//user data
};



static void onLoad(hscreen_t *s){
	//rt_kprintf("screen_status:onLoad\r\n");
	userdata_t *u=s->data;
	hmi_list_load(u->list);
}

static void onTimer(hscreen_t *s){
	static int i=0;
	if(i++>10){
		i=0;
		hmi_list_refresh(&list);
		for(int i=0;i<STEPPER_NUM;i++){
			if(trigger_read(cp.gcodeDevice.stepPos.planner.stepBlock.stepAxis.maxTrigger+i)){
				triggers[i]=1;
			}else if(trigger_read(cp.gcodeDevice.stepPos.planner.stepBlock.stepAxis.minTrigger+i)){
				triggers[i]=-1;
			}else{
				triggers[i]=0;
			}
		}
	}
}






