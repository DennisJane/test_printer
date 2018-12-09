#include "hmi_hal.h"
#include "hmi_list.h"
#include <dfs_posix.h>
#include "screen_main.h"
#include "hmi_button.h"
#include "lib/corePrinter.h"
#include "stdlib.h"

extern button_t item[ITEM_NUM];
extern button_t navi[NAVI_NUM];
extern char working_directory[];



typedef struct chain_tag{
	rt_uint8_t d_type;
	char *filename;
	struct chain_tag *prev;
	struct chain_tag *next;
}chain_t;

void chain_add(chain_t **c,struct	dirent *file){
	chain_t *newtag=rt_malloc(sizeof(chain_t));
	newtag->d_type=file->d_type;
	newtag->filename=rt_malloc(file->d_namlen+1);
	rt_memcpy(newtag->filename,file->d_name,file->d_namlen);
	newtag->filename[file->d_namlen]=0;
	
	newtag->prev=*c;
	newtag->next=(*c)->next;
	(*c)->next=newtag;
	
	*c=newtag;
}


void chain_clear(chain_t *zeroOff)
{
	chain_t *temp1;
	chain_t *temp2;	
	temp1=zeroOff;
	
	while(temp1->next)
	{
		temp2=temp1->next;
		temp1->next=temp2->next;
		
		rt_free(temp2);
	}
	
}


char *telltype(char *fname)
{
	char *type;
	char *temp;
	u8 i=0;
	while(i<250&&*fname!='\0')
	{
		i++;
		if(*fname=='.') temp=fname ;//最后的.
		fname++;
	}
	if(i==250) return 0;//错误的字符串.
	temp++;	
// 	while(*temp=='\0')//得到后缀名
//	{	
//		*type=*temp;
//		temp++;
//		type++;
//  }
//	*type='\0';	   
	
	return temp;
}	


typedef struct{
	int sid;
	button_t *item;
	button_t *navi;
	DIR *dir;
	struct dirent *dirItem;
	char *fileName[ITEM_NUM];
	chain_t zeroOff;
	int count;
	int NUM;
	chain_t *curOff;
}userdata_t;

static userdata_t userData={0,item,navi};

static void onLoad(hscreen_t *s);
static void onTimer(hscreen_t *s);
static void onButton(hscreen_t *s,int id);

hscreen_t fileItem={
	0,		//id
	0,		//prev
	onLoad,
	onButton,
	onTimer,
	&userData,		//user data
};

static int loadDir(hscreen_t *s,char *name){
	
	struct	dirent *file; 
	userdata_t *u=s->data;
	chdir(name);
	if(u->dir!=RT_NULL){
		closedir(u->dir);
	}
	u->dir=opendir(working_directory);
	chain_clear(&(u->zeroOff));
	u->curOff=&(u->zeroOff);
	u->NUM=0;
	u->count=0;
	
	file=readdir(u->dir);
	while(file!=0)
	{
		if(file->d_name[0]!='.')
		{
			if(file->d_type==1)
			{
				if(strcmp(telltype(file->d_name),"gcode"))
				{
					file=readdir(u->dir);
					continue;					
				}
			}
			u->NUM++;
			chain_add(&(u->curOff),file);
		}	
		file=readdir(u->dir);
	}
		
	return 0;
}
static void itemDirProcessor(button_t *this);
static void itemFileProcessor(button_t *this);
static void naviProcessor(button_t *this);
static void refreshList(hscreen_t *s);

static void listDir(hscreen_t *s,	chain_t *curOff){   //第二个参数没用
	userdata_t *u=s->data;
	
	chain_t *t=u->curOff;	
	
	for(int i=0;i<ITEM_NUM;i++)
	{
		button_setName(u->item+i,0);
		button_setFunc(u->item+i,0);
		button_refreshName(u->item+i,0,0);
	}
	if(u->NUM==0) return;	
	for(int i=0;i<ITEM_NUM;i++)
	{		
		button_setName(u->item+i,t->filename);
		if(t->d_type==2)
		{
			button_setFunc(u->item+i,itemDirProcessor);
			button_refreshName(u->item+i,"[","]");
		}
		else if(t->d_type==1)
		{
			button_setFunc(u->item+i,itemFileProcessor);
			button_refreshName(u->item+i,0,0);
		}
		else
		{
			button_setName(u->item+i,0);
			button_setFunc(u->item+i,0);
			button_refreshName(u->item+i,0,0);
		}
		
		u->count=u->count+1;		
		t=t->prev;
		u->curOff=t;
		if(t->prev==0)
		{
			break;
		}
		
	}
}


static void itemDirProcessor(button_t *this){
	hscreen_t *s=this->parent;
	userdata_t *u=s->data;
	chdir(this->name);
	refreshList(s);
}

extern void status_hmi_chang(bool flag);
extern corePrinter_t cp;

extern void switch_dialog(int flag,char *str);
static void cancelPrint(void){
//	corePrinter_stop(&cp);
//	status_hmi_chang(0);
//	
//	switch_menu(0); //??
	switch_dialog(1,"");
}


static void itemFileProcessor(button_t *this){
//	hscreen_t *s=this->parent;
	//userdata_t *u=s->data;

	switch_dialog(0,this->name);

}

static void refreshList(hscreen_t *s){
	userdata_t *u=s->data;
	if(u->dir==RT_NULL){
		loadDir(s,"/");
		listDir(s,u->curOff);
	}else{
		loadDir(s,working_directory);
		listDir(s,u->curOff);
	}
}
static void naviProcessor(button_t *this){
	hscreen_t *s=this->parent;
	userdata_t *u=s->data;
	int redundant;
	switch(this->bid){
		case NAVI_BTN_BASE:
			chdir("..");
			loadDir(s,working_directory);
			listDir(s,u->curOff);
			break;
		case NAVI_BTN_BASE+1:
			redundant=u->count%ITEM_NUM;
			if(u->count>6)
			{
				if(redundant==0)
				{
					for(int i=0;i<ITEM_NUM;i++)
					{
						u->curOff=u->curOff->next;
						u->count--;
					}
				}
				else
				{
					for(int i=0;i<redundant;i++)
					{
						u->curOff=u->curOff->next;
						u->count--;
					}
				}
				
				for(int i=0;i<ITEM_NUM;i++)
				{
					u->curOff=u->curOff->next;
					u->count--;
				}
				listDir(s,u->curOff);	
			}
			break;
		case NAVI_BTN_BASE+2:
			if((u->NUM-u->count>0))
			{
				listDir(s,u->curOff);	
			}
			break;
		default:
			break;
	}
}



static void onLoad(hscreen_t *s){
	//rt_kprintf("screen_print:onLoad\r\n");
	userdata_t *u=s->data;
	for(int i=0;i<ITEM_NUM;i++){
		button_setParent(u->item+i,s);
		button_setName(u->item+i,0);
		button_setFunc(u->item+i,0);
		button_setUserId(u->item+i,i);
		button_refreshName(u->item+i,0,0);
	}
	if(cp.gcodeDevice.currentDevice==1){
		for(int i=0;i<NAVI_NUM;i++){
			button_setParent(u->navi+i,s);
			button_setName(u->navi+i,0);
			button_setFunc(u->navi+i,0);
		}
		button_refreshName(u->navi,0,0);
		button_setName(u->item,"<正在打印中，点击取消....>");
		button_setFunc(u->item,cancelPrint);
		button_refreshName(u->item,0,0);
		return;
	}
	for(int i=0;i<NAVI_NUM;i++){
		button_setParent(u->navi+i,s);
		button_setName(u->navi+i,0);
		button_setFunc(u->navi+i,naviProcessor);
	}
	button_refreshName(u->navi,0,0);
	u->NUM=0;
	u->count=0;
	u->zeroOff.prev=0;
	u->zeroOff.next=0;
	u->curOff=&u->zeroOff;
	refreshList(s);
	
	//rt_kprintf("screen_print:onLoad finish\r\n");
}

static void onTimer(hscreen_t *s){
	
}

static void onButton(hscreen_t *s,int id){
	userdata_t *u=s->data;
	if(button_checkPressed(u->item,ITEM_NUM,0,id))return;
	if(button_checkPressed(u->navi,NAVI_NUM,0,id))return;
}








