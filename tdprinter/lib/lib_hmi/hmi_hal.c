#include "hmi_hal.h"
#include "lib/libConfig.h"

#include "lib/lib_hmi/screen_main.h"

void hmi_thread(void *param);
static void RxStateMachine(hmi_t *s,uint8_t data);

void hmi_construct(hmi_t *s,int id){
	s->device=0;
	s->thread=0;
	s->curScreen=0;
	s->id=id;
}

void hmi_init(hmi_t *s,char *uartDev,hscreen_t *initScreen){
	s->device=rt_device_find(uartDev);
	if(s->device==RT_NULL){rt_kprintf("hmi_init:uart %s not found\r\n",uartDev);return;}
	HMI_DriverInit(s->device);
	
	s->curScreen=initScreen;
	s->curScreen->parent=0;
	s->thread=rt_thread_create("hmi_hal0",hmi_thread,s,THREAD_HMI_STACK_SIZE,THREAD_HMI_PRIORITY,1);
	rt_thread_startup(s->thread);
}

rt_timer_t timer;

void hmi_thread(void *param){
	hmi_t *s=param;
		
	SetScreen(0);

	rt_thread_delay(10);
	if(s->curScreen->onLoad){s->curScreen->onLoad(s->curScreen);}
	uint8_t c;
	while(1){
		if(1==rt_device_read(s->device,0,&c,1)){
			RxStateMachine(s,c);
		}else{
			rt_thread_delay(1);
			static int i=0;
			if(i++>10){
				if(s->curScreen->onTimer){s->curScreen->onTimer(s->curScreen);}
			}
		}
	}
}

//EE B1 11 00 01 07 10 01 00 FF FC FF +FF(missing)
//EE B1 11 00 01 08 10 01 00 FF FC FF 
//EE B1 11 00 00 09 10 01 00 FF FC FF

#define CONTROL_CMD 0xB1
#define CONTROL_READ 0x11

extern hscreen_t dialogScreen;
extern void reboot(int delay);

static void RxStateMachine(hmi_t *s,uint8_t data){
	static uint32_t state=0,len=0;
	static uint8_t payload[32],lp;
	switch(state){
		case 0:
			if(data==0xEE)state++;break;
		case 1:
			payload[0]=data;
			state++;break;
		case 2:
			payload[1]=data;
			lp=0;
			if(payload[0]==CONTROL_CMD && payload[1]==CONTROL_READ){
				len=5;
				state++;
			}else{
				state=0;
			}
			break;
		case 3:
			payload[2+lp++]=data;
			if(lp<len)break;
			else state++;
		case 4:
			if(payload[0]==CONTROL_CMD && payload[1]==CONTROL_READ){
				uint16_t sid=payload[3];
				uint16_t cid=payload[5];
				if(sid==0)  //主菜单
				{
					if(s->curScreen->onButton)
						s->curScreen->onButton(s->curScreen,cid);
				}
				else if(sid==2)  // 对话框
				{		
					if(s->curScreen->onButton)
						s->curScreen->onButton(s->curScreen,cid);					
				}
				else if(sid==3)
				{
					if(s->curScreen->onButton)
						s->curScreen->onButton(s->curScreen,cid);			
				}
				else if(sid==4)
					if(cid==3) reboot(1);
			}
			state=0;
			break;
		default:
			state=0;break;
	}
}

void hmi_switch_screen(hmi_t  *s,hmi_switch_mode smode,hscreen_t *dst){
	
	s->curScreen=dst;
	SetScreen(dst->sid);
	if(dst->onLoad) dst->onLoad(dst);
}
