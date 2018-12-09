#include "lib/param/param.h"
#include "lib/corePrinter.h"
#include "app/app_subboard.h"
#include "hmi_hal.h"
#include "stdlib.h"
#include "screen_main.h"
#include "hmi_button.h"

#define dialog_item_NUM   3


static void onLoad(hscreen_t *s);
static void onButton(hscreen_t *s,int id);
static void onTimer(hscreen_t *s);
static void button_printOk(button_t *b);
static void button_Cancel(button_t *b);
static void button_stopOk(button_t *b);
static void button_filamentCancel(button_t *b);
static void levelOk(button_t *b);

extern void status_hmi_chang(bool flag);
extern corePrinter_t cp;
extern hmi_t hmi;
extern hscreen_t mainScreen;
extern void retarget_uartCode(char *c);
bool filament_enable=0;

static button_t dailog_item[dialog_item_NUM]={
	{2,1,11,NULL,NULL,},
	{2,2,22,NULL,NULL,},      //确定按钮
	{2,3,33,NULL,NULL,},			//取消按钮
};

typedef struct{
	int cur_dialog;
	char *filename;
	button_t *item;
}userdata_t;

static userdata_t data={	0,"",dailog_item};



hscreen_t dialogScreen={
	2,		//id
	0,		//prev
	onLoad,
	onButton,
	onTimer,
	&data,		//user data
};

static void onButton(hscreen_t *s,int id){
	if(button_checkPressed(dailog_item,dialog_item_NUM,s->sid,id))return;
}

static void onTimer(hscreen_t *s){
	char *temperate;
	if(data.cur_dialog==2||data.cur_dialog==3)
	{
		if(filament_enable&&sboard_getTemper()>=219)
		{
			if(data.cur_dialog==2)
			{
				retarget_uartCode("G28 E1000 F90");
				SetTextValue(dialogScreen.sid,11,"进料中....");
			}
			else if(data.cur_dialog==3)
			{
				retarget_uartCode("G28 E-1000 F90");
				SetTextValue(dialogScreen.sid,11,"退料中....");
			}
			filament_enable=0;
		}
		rt_snprintf(temperate,10,"%d",sboard_getTemper());	
		SetControlValue2(dialogScreen.sid,4,"当前温度:",temperate,"  目的温度:220");	
	}
	else if(data.cur_dialog==4)
	{
		
	}
}

static void onLoad(hscreen_t *s){}

	
void switch_dialog(int flag,char *str,hscreen_t *pre){

	hmi.curScreen=&dialogScreen;	
	dialogScreen.parent=pre;
	data.filename=str;
	data.cur_dialog=flag;
	switch(flag)
	{
		case 0:							//打印确定？
			dailog_item[1].onPressed=button_printOk;
			dailog_item[2].onPressed=button_Cancel;
			SetScreen(dialogScreen.sid);	
			SetTextValue(dialogScreen.sid,11,str);	
			break;
		case 1:							//取消确定？
			dailog_item[1].onPressed=button_stopOk;
			dailog_item[2].onPressed=button_Cancel;
			SetScreen(dialogScreen.sid);	
			SetTextValue(dialogScreen.sid,11,"停止打印？");	
			break;
		case 2:											//进料
		case 3:
			dailog_item[1].onPressed=button_filamentCancel;
			dailog_item[2].onPressed=button_filamentCancel;		
		
			sboard_setLock(0);
			sboard_setTemper(220);
			sboard_setLock(1);	
		
			retarget_uartCode("G91\r\nM11 E\r\n");			
			filament_enable=1;
		
			SetScreen(dialogScreen.sid);	
			SetTextValue(dialogScreen.sid,11,str);	
			break;		
		case 4:
			dailog_item[1].onPressed=levelOk;
			dailog_item[2].onPressed=levelOk;
			
			SetScreen(dialogScreen.sid);	
			SetTextValue(dialogScreen.sid,11,str);	
		
			retarget_uartCode("G91\r\nM11 XYZ\r\n");
			retarget_uartCode("G28 Z-300 F600\r\n");
			retarget_uartCode("G0 Z2 F400\r\n");
			retarget_uartCode("G28 Z-4 F400\r\n");
			retarget_uartCode("G28 X400 Y-400 F3000\r\n");
			retarget_uartCode("M0 XYZ\r\n");
		
			SetTextValue(dialogScreen.sid,11,"OK，请手动调平");
			break;
	}	
}

static void button_printOk(button_t *b){
	hmi.curScreen=&mainScreen;
	status_hmi_chang(1);
	SetScreen(0); 
	corePrinter_start(&cp,data.filename);
	switch_menu(2);
}

static void button_stopOk(button_t *b){										//取消打印 点击弹出对话框中的确定按钮
	
	corePrinter_stop(&cp);
	status_hmi_chang(0);
	
	hmi.curScreen=&mainScreen;
	SetScreen(0); 
	switch_menu(0); //更新
}

	
static void button_Cancel(button_t *b){
	hmi_switch_screen(&hmi,hmi_screen_jump,&mainScreen);
//	hmi.curScreen=&mainScreen;
//	SetScreen(0);
//	//main_onButton(0,0);
}

static void button_filamentCancel(button_t *b){
	filament_enable=0;
	hmi_switch_screen(&hmi,hmi_screen_jump,&mainScreen);
	
	corePrinter_stop(&cp);
	//retarget_uartCode("M0 E\r\n");
	sboard_setTemper(20);	
}

static void levelOk(button_t *b){
	corePrinter_stop(&cp);
	hmi_switch_screen(&hmi,hmi_screen_jump,&mainScreen);
}