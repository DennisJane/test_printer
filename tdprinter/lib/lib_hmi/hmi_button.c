#include "hmi_hal.h"
#include "hmi_button.h"
#include <string.h>

void button_setParent(button_t *b,void *parent){
	b->parent=parent;
}
void button_setName(button_t *b,char *name){
	b->name=name;
}
void button_refreshName(button_t *b,char *pre,char *post){
	SetControlValue2(b->sid,b->tid,pre,b->name,post);
}
void button_setFunc(button_t *b,button_cb func){
	b->onPressed=func;
}
void button_setUserId(button_t *b,int userid){
	b->userid=userid;
}
void button_onPressed(button_t *b){
	if(b->onPressed){
		(b->onPressed)(b);
	}
}
int button_checkPressed(button_t b[],int num,int sid,int bid){
	for(int i=0;i<num;i++){
		if(b[i].sid==sid && b[i].bid==bid){
			button_onPressed(b+i);
			return 1;
		}
	}
	return 0;
}




