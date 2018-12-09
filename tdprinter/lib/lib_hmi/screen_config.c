#include "hmi_hal.h"
#include "stdlib.h"
#include "hmi_button.h"
#include "hmi_list.h"

static void onButton(hscreen_t *s,int id);
static void onLoad(hscreen_t *s);
extern hscreen_t configItem;

void SwitchToSub(struct hmi_button_tag *b);
void SwitchToMenu(struct hmi_button_tag *b);

#define  screen_config_id 3
#define  CONF_MENU_NUM  3
#define  CONF_SUB_NUM  6
#define  CONF_NAVI_NUM  3

button_t config_menu[CONF_MENU_NUM]={
	{screen_config_id,0,100,"打印",0,SwitchToMenu,},
	{screen_config_id,1,101,"设置",},
	{screen_config_id,2,102,"状态",0,SwitchToMenu,},
};

button_t config_sub[CONF_SUB_NUM]={
	{screen_config_id,3,103,"速度设置",0,SwitchToSub,},
	{screen_config_id,4,104,"温度设置",0,SwitchToSub,},
	{screen_config_id,5,105,"电机控制",0,SwitchToSub,},
	{screen_config_id,6,106,"预操作",0,SwitchToSub,},
	{screen_config_id,7,107,"风扇控制",0,SwitchToSub,},
	{screen_config_id,8,108,0,0,},
};
button_t config_navi[CONF_NAVI_NUM]={
	{screen_config_id,20,50,},
	{screen_config_id,21,50,},
	{screen_config_id,22,50,},
};



typedef struct{
	int cursubconfig;
	button_t *menu;	
	button_t *item;
	button_t *navi;
	hscreen_t *subscr[1];
}userdata_t;

static userdata_t userData={
	0,
	config_menu,config_sub,config_navi,
	&configItem,
};

hscreen_t configScreen={
	3,		//id
	0,		//prev
	onLoad,
	onButton,
	0,
	&userData,		//user data
};


static void onButton(hscreen_t *s,int id){
	userdata_t *u=s->data;
	if(button_checkPressed(u->menu,CONF_MENU_NUM,s->sid,id))return;
	if(button_checkPressed(u->item,CONF_SUB_NUM,s->sid,id))return;
	if(button_checkPressed(u->navi,CONF_NAVI_NUM,s->sid,id))return;
}

static void onLoad(hscreen_t *s){
	userdata_t *u=s->data;
	for(int i=0;i<CONF_MENU_NUM;i++){
		if(i==1){
			button_refreshName(u->menu+i,0,"->");
		}else{
			button_refreshName(u->menu+i,0,0);
		}
	}
	
	for(int i=0;i<CONF_SUB_NUM-2;i++){
			button_refreshName(u->item+i,0,0);
	}
	
	
}

extern void switchToItems2(button_t *b);
extern hscreen_t mainScreen;
extern void switch_menu(int menu);
extern void changmenu(int menu);

void SwitchToMenu(struct hmi_button_tag *b){
	
	hmi_switch_screen(&hmi,hmi_screen_jump,&mainScreen);
	switch_menu(b->bid);
}

void SwitchToSub(struct hmi_button_tag *b){

	hmi.curScreen=&mainScreen;
	SetScreen(0); 
	switch_menu(1);

	//userData.cursubconfig=b->bid;
	switchToItems2(b);
}

