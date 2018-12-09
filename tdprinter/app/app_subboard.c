#include <rtthread.h>
#include <stdint.h>
#include "lib/my_atof.h"
#include <stdlib.h>
#include <finsh.h>
#include "lib/libConfig.h"

#if !defined(TDPRINTER_BRAIN)
#pragma warning subSystem:TDPRINTER

static void processLine(char *line);
static rt_device_t uart;

static int counter=0,csum=0,retarget=0;
int temper=0,pressure=0;
uint16_t temperDst=0;

uint8_t fan1=0,fan2=0;
float fanrate=1.00;

static char line[16];
static uint16_t lineLp=0;
static uint8_t txBuf[5]={0x55,0xAA,0x00,0x00,0x00};
static void sendData(void){
	switch(counter){
		case 0:
			txBuf[2]=0;
			txBuf[3]=fan1*fanrate;
			break;
		case 1:
			txBuf[2]=1;
			txBuf[3]=fan2*fanrate;
			break;
		case 2:
			{
				uint16_t temp=temperDst;	//LSB 1 = 0x01 0x00
				temp+=512;
				//txBuf	MSB 1 = 0x00 0x01
				txBuf[2+0]=temp>>8;
				txBuf[2+1]=temp&0x00FF;
			}
			break;
		default:
			break;
	}
	txBuf[4]=txBuf[2]+txBuf[3];
	txBuf[4]=~txBuf[4]+1;
	rt_device_write(uart,0,&txBuf,5);
	if(++counter>2){
		counter=0;
	}
}
void app_sboard(void *useless){
	uint8_t c;
	uart=rt_device_find(SB_UART);
	if(uart==RT_NULL){
		rt_kprintf("app_sboard:uart not found\r\n");
		return;
	}
	while(1){
		if(1==rt_device_read(uart,0,&c,1)){
			if(retarget)rt_kprintf("%c",c);
			if(c=='\n' || c==0)continue;
			line[lineLp]=c;
			if(c=='\r'){
				line[lineLp]='\0';
				lineLp=0;
				processLine(line);
			}else if(lineLp<15){
				lineLp++;
			}
		}else{
			static int idleSend=0;
			if(idleSend++>40){
				idleSend=0;
				sendData();
			}
			rt_thread_delay(1);
		}
	}
}

static void processLine(char *line){
	switch(line[0]){
		case 't':
			temper=atoi(line+1);
			csum++;
			sendData();
			if(temper>200){
				fan2=220;
			}else if(temper>70){
				fan2=temper;
			}else{
				fan2=60;
			}
			break;
		case 'p':
			pressure=atoi(line+1);
			break;
		default:
			break;
	}
}

static int slock=0;
int sboard_getTemper(void){
	return temper;
}
int sboard_getPressure(void){
	return pressure;
}
void sboard_setTemper(uint16_t _temper){
	if(slock==0)
	temperDst=_temper;
}
int sboard_getTemperDst(void){
	return temperDst;
}
int sboard_setLed(uint8_t en){
	return 0;
}
int sboard_setFan(uint8_t fan){
	fan1=fan;
	fan2=fan;
	return fan;
}
//fan1 = model fan
int sboard_setFan1(uint8_t fan){
	fan1=fan;
	return fan;
}
//fan2 = extruder fan
int sboard_setFan2(uint8_t fan){
	fan2=fan;
	return fan;
}
int sboard_getStatus(void){
	return csum;
}
void sboard_setLock(uint8_t lock){
	slock=lock;
}
int sboard_setRetarget(int r){
	retarget=r;
	return r;
}
FINSH_FUNCTION_EXPORT_ALIAS(sboard_getTemper,getTemper,"getTemper()");
FINSH_FUNCTION_EXPORT_ALIAS(sboard_getPressure,getPressure,"getTemper()");
FINSH_FUNCTION_EXPORT_ALIAS(sboard_setTemper,setTemper,"setTemper(u8)");
FINSH_FUNCTION_EXPORT_ALIAS(sboard_setLed,setLed,"setLed(u8)");
FINSH_FUNCTION_EXPORT_ALIAS(sboard_setFan,setFan,"setFan(u8)");
FINSH_FUNCTION_EXPORT_ALIAS(sboard_setFan1,setFan1,"setFan(u8)");
FINSH_FUNCTION_EXPORT_ALIAS(sboard_setFan2,setFan2,"setFan(u8)");
FINSH_FUNCTION_EXPORT_ALIAS(sboard_getStatus,getSboard,"getSboardStatus");
FINSH_FUNCTION_EXPORT_ALIAS(sboard_setRetarget,setSbRetarget,"setSbRetarget(en)");



#endif //!defined(TDPRINTER_BRAIN)
