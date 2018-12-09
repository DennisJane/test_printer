#include "hmi_hal.h"
#include "stdlib.h"
#include "screen_main.h"
#include "hmi_button.h"

static void onLoad(hscreen_t *s);
static void onTimer(hscreen_t *s);
static void onButton(hscreen_t *s,int id);
static void flushMenu(hscreen_t *s);

button_t menu[MENU_NUM]={
	{0,0,100,},
	{0,1,101,},
	{0,2,102,},
};
button_t item[ITEM_NUM]={
	{0,3,103,},
	{0,4,104,},
	{0,5,105,},
	{0,6,106,},
	{0,7,107,},
	{0,8,108,},
};
button_t navi[NAVI_NUM]={
	{0,20,50,},
	{0,21,50,},
	{0,22,50,},
};

typedef struct{
	int curMenu;
	button_t *menu;
	button_t *item;
	button_t *navi;
	hscreen_t *subScr[MENU_NUM];
}userdata_t;

extern hscreen_t fileItem,configScreen,configItem,statusItem;


userdata_t userData={
	2,
	menu,item,navi,
	&fileItem,&configItem,&statusItem,
};

hscreen_t mainScreen={
	0,		//id
	0,		//prev
	onLoad,
	onButton,
	onTimer,
	&userData,		//user data
};

static void menuButtonProcessor(button_t *this){
	hscreen_t *s=this->parent;
	userdata_t *u=s->data;
	switch(this->bid)
	{
		case 0:
		case 2:
			u->curMenu=this->bid;		//menu bid start from 0
			flushMenu(s);
			break;
		case 1:	
			hmi.curScreen=&configScreen;
			if(configScreen.onLoad) configScreen.onLoad(&configScreen);
			SetScreen(configScreen.sid);
		}
}

static const char *menuName[]={
"´òÓ¡",
"ÉèÖÃ",
"×´Ì¬",
};


static void flushMenu(hscreen_t *s){
	userdata_t *u=s->data;
	for(int i=0;i<MENU_NUM;i++){
		if(i==u->curMenu){
			button_refreshName(u->menu+i,0,"->");
		}else{
			button_refreshName(u->menu+i,0,0);
		}
	}
	if(u->subScr[u->curMenu]){
		(*u->subScr[u->curMenu]->onLoad)(u->subScr[u->curMenu]);
	}
}

static void onLoad(hscreen_t *s){
	//rt_kprintf("screen_main:onLoad\r\n");
	userdata_t *u=s->data;
	button_setParent(u->menu+0,s);
	button_setParent(u->menu+1,s);
	button_setParent(u->menu+2,s);

	button_setName(u->menu+0,(char*)menuName[0]);
	button_setName(u->menu+1,(char*)menuName[1]);
	button_setName(u->menu+2,(char*)menuName[2]);

	button_setFunc(u->menu+0,menuButtonProcessor);
	button_setFunc(u->menu+1,menuButtonProcessor);
	button_setFunc(u->menu+2,menuButtonProcessor);

	for(int i=0;i<MENU_NUM;i++){
		u->subScr[i]->parent=s;
	}
	flushMenu(s);
	
}

static void onTimer(hscreen_t *s){
	userdata_t *u=s->data;
	if(u->subScr[u->curMenu]){
		(*u->subScr[u->curMenu]->onTimer)(u->subScr[u->curMenu]);
	}
	return;
}


static void onButton(hscreen_t *s,int id){
	//rt_kprintf("screen_main:onButton\r\n");
	userdata_t *u=s->data;
	if(button_checkPressed(u->menu,MENU_NUM,s->sid,id))return;
	if(button_checkPressed(u->item,ITEM_NUM,s->sid,id))return;
	if(button_checkPressed(u->navi,NAVI_NUM,s->sid,id))return;
}

void switch_menu(int menu){
	
	userData.curMenu=menu;
	onLoad(&mainScreen);
	//onButton(s,id);
}

void changmenu(int menu){
	userData.curMenu=menu;
}


