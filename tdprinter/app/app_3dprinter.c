#include "stm32f4xx.h"
#include <rtthread.h>
#include <finsh.h>

#include "lib/libConfig.h"
#include "lib/corePrinter.h"
#include "lib/lib_hmi/hmi_hal.h"

#include "lib/lib_hmi/hmi_driver.h"


corePrinter_t cp;
hmi_t hmi;
extern hscreen_t mainScreen;

void TDPrinter_init(void){
	rt_kprintf("%s",TD_VERSION);
	hmi_construct(&hmi,0);	
	hmi_init(&hmi,HMI_UART,&mainScreen);
	SetBuzzer(15);
	
	corePrinter_construct(&cp);
	rt_device_t fakeDev=rt_device_find("fakeDev0");
	corePrinter_init(&cp,fakeDev);
}

static void getTrigger(void){
	rt_kprintf("Trigger:MAX(");
	for(int i=0;i<STEPPER_NUM;i++){
		rt_kprintf("%d ",trigger_read(cp.gcodeDevice.stepPos.planner.stepBlock.stepAxis.maxTrigger+i));
	}
	rt_kprintf("),MIN(");
	for(int i=0;i<STEPPER_NUM;i++){
		rt_kprintf("%d ",trigger_read(cp.gcodeDevice.stepPos.planner.stepBlock.stepAxis.minTrigger+i));
	}
	rt_kprintf(")\r\n");
}
static void fakeAllTrigger(void){
	for(int i=0;i<STEPPER_NUM;i++){
		cp.gcodeDevice.stepPos.planner.stepBlock.stepAxis.maxTrigger[i].fakeOutput=true;
		cp.gcodeDevice.stepPos.planner.stepBlock.stepAxis.minTrigger[i].fakeOutput=true;
	}
}
FINSH_FUNCTION_EXPORT_ALIAS(getTrigger,getTrigger,getTrigger(););
FINSH_FUNCTION_EXPORT_ALIAS(fakeAllTrigger,fakeTrigger,fakeTrigger(););

static int cp_start(char *filename){
	corePrinter_start(&cp,filename);
	return 0;
}
static int cp_stop(void){
	corePrinter_stop(&cp);
	return 0;
}

FINSH_FUNCTION_EXPORT_ALIAS(cp_start,coreStart,coreStart("filename"));
FINSH_FUNCTION_EXPORT_ALIAS(cp_stop,coreStop,coreStop());





