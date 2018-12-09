#include "lib/libConfig.h"

#if defined(TDPRINTER_BRAIN)

#include <rtthread.h>
#include <stdint.h>
#include "lib/my_atof.h"
#include <finsh.h>
#include "lib/libConfig.h"
#include "lib/lib_pid.h"
#include "stm32f4_drivers/drivers.h"
#include "bsp/bsp.h"
#include "drivers/spi.h"


#define DEFAULT_TEMP -200.0f

static float status[2]={0,0};
static float temper=0,temperCtl=0,value=0;
static int initFlag=10;
#define temperFl status[0]
#define temperDst status[1]
static pid_t pid;
static lowpass_t tempf;
static struct rt_spi_device *max6675=0;
static void HeaterManagerInit(void){
	rt_device_t _max6675=rt_device_find("max6675");
	max6675 = (struct rt_spi_device *)(_max6675);
	//P=0.05 20c dec
	pid_init(&pid,0.04,0.05,2.0,0.75,1.0,0.15);
	lowpass_init(&tempf,0.1);
}
static void HeaterControler(void){
	if(initFlag>0){
		initFlag--;
		temperFl=temperFl*0.5f+temper*0.5f;
		lowpass_reset(&tempf,temperFl);
		pid_reset(&pid,temperFl);
		pwm_set(&heater,0);
	}else{
		if(temper + 5.0f > tempf.result)value=tempf.result + 5.0f;
		else if(value - 5.0f < tempf.result)value=tempf.result - 5.0f;
		lowpass_input(&tempf,temper,0.25f);
		temperFl=lowpass_output(&tempf,0);
		pid_input(&pid,temperDst-temperFl,0.25f);
		if(pid.sum<0.0f)pid.sum=0;
			temperCtl=pid_output(&pid,0);
			if(temperDst<0){
				pwm_set(&heater,0);
			}else{
				pwm_set(&heater,temperCtl);
		}
	}
}
static uint16_t rxBuffer[1]={0};
static const struct rt_spi_message readmsg={
	0,
	rxBuffer,
	1,	//length
	0,	//next
	1,1,//cs take,release
};
static void HeaterManage(float dt){
	rt_spi_transfer_message(max6675,(struct rt_spi_message*)&readmsg);
	rxBuffer[0]=rxBuffer[0]>>3;
	temper=(float)(rxBuffer[0])/4;
	HeaterControler();
}

void app_sboard(void *useless){
	HeaterManagerInit();
	while(1){
		rt_thread_delay(25);
		HeaterManage(0.25f);
	}
}

static int slock=0;
int sboard_getTemper(void){
	return temperFl;
}
void sboard_setTemper(uint8_t _temper){
	if(slock==0)
	temperDst=_temper;
}
int sboard_getTemperDst(void){
	return temperDst;
}
int sboard_setLed(uint8_t en){
	return 0;
}
int sboard_setFan(uint8_t en){
	return 0;
}
int sboard_getStatus(void){
	return 0;
}
void sboard_setLock(uint8_t lock){
	slock=lock;
}
uint8_t sboard_getLock(void){
	return slock;
}

FINSH_FUNCTION_EXPORT_ALIAS(sboard_getTemper,getTemper,"getTemper()");
FINSH_FUNCTION_EXPORT_ALIAS(sboard_setTemper,setTemper,"setTemper(u8)");
FINSH_FUNCTION_EXPORT_ALIAS(sboard_setLock,setTempLock,"setTempLock(u8)");


#endif //defined(TDPRINTER_BRAIN)

