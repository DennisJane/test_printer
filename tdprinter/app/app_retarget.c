#include <rtthread.h>
#include <finsh.h>
#include "bsp/bsp.h"
#include <stdbool.h>
#include <stdint.h>
#include "lib/fakeDevice.h"
#include "lib/nullDevice.h"
#include "lib/fileDevice.h"
#include "lib/libConfig.h"

fakeDevice_t uartCode;		//fakeDevice ID=0
nullDevice_t nullDev;

static rt_event_t event;
#define EVENT_UART_RXNE (uint32_t)(1<<0)
static rt_size_t uartSize=0;
static rt_device_t uart;
static rt_device_t targetUart=0;

void retarget_thread(void *param);

void retarget_init(void){
	fakeDevice_init(&uartCode,0);
	nullDevice_init(&nullDev);
	event=rt_event_create("evt_retarget",RT_IPC_FLAG_FIFO);
	uart=rt_device_find(FINSH_DEVICE);
	rt_thread_t t=rt_thread_create("retarget",retarget_thread,0,THREAD_RETARGET_STACK_SIZE,THREAD_RETARGET_PRIO,1);
	rt_thread_startup(t);
}

void retarget_thread(void *param){
	while(1){
		while(targetUart){
			uint8_t c;
			if(rt_device_read(targetUart,0,&c,1)==1){
				rt_device_write(uart,0,&c,1);
			}else if(rt_device_read(uart,0,&c,1)==1){
				rt_device_write(targetUart,0,&c,1);
			}
		}
		if(rt_event_recv(event,EVENT_UART_RXNE,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,1,0)==RT_EOK){
			if(uartSize<1)uartSize=1;
			char *buf=rt_malloc(uartSize+1);
			if(buf){
				int trueSize=rt_device_read(uart,0,buf,uartSize);
				if(trueSize>0){
					buf[trueSize]=0;
					fakeDevice_fill(&uartCode,buf);
					rt_kprintf("%s",buf);
				}
				rt_free(buf);
			}
		}else{
			rt_thread_delay(1);
		}
	}
}

rt_err_t uartCodeFiller(rt_device_t dev, rt_size_t size){
	uartSize=size;
	rt_event_send(event,EVENT_UART_RXNE);
	return RT_EOK;
}

void retarget_finshMode(uint8_t enable){
	if(enable){
		finsh_set_device(FINSH_DEVICE);
	}else{
		finsh_set_device("nullDev");
		rt_device_open(uart,RT_DEVICE_OFLAG_RDWR);
		rt_device_set_rx_indicate(uart,uartCodeFiller);
	}
}

void retarget_uartCode(char *c){
	fakeDevice_fill(&uartCode,c);
	char enter[2]="\r\n";
	fakeDevice_fill(&uartCode,enter);
}
void retarget_setRetrget(char *uartName){
	targetUart=rt_device_find(uartName);
}
void getPutcCounter(){
	extern int globalStdoutCounter;
	rt_kprintf("stdCnt=%d\r\n",globalStdoutCounter);
}


FINSH_FUNCTION_EXPORT_ALIAS(retarget_uartCode,g,"gcode("")");
FINSH_FUNCTION_EXPORT_ALIAS(retarget_finshMode,finshMode,"finshMode(enable))");
FINSH_FUNCTION_EXPORT_ALIAS(retarget_setRetrget,setRetarget,"setRetarget(char *uartName)");
FINSH_FUNCTION_EXPORT_ALIAS(getPutcCounter,stdoutCnt,"getPutcCounter()");

