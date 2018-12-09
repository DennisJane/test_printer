#include "hmi_list.h"
#include <stdio.h>

#define DIV_AND_FLOOR(x,y) (x/y+((x%y>0)?1:0))

static void hmi_list_navi_next_func(button_t *this);
static void hmi_list_navi_prev_func(button_t *this);
static void hmi_list_navi_ret_func(button_t *this);

void hmi_list_load(hmi_list_t *s){
	//s->curIndex=0;
	for(int i=0;i<3;i++){
		button_setParent(s->naviButton+i,s);
		button_setName(s->naviButton+i,0);
		button_setFunc(s->naviButton+i,0);
	}
	sprintf(s->naviBuf,"%d/%d", s->curIndex/s->buttonNum+1 , DIV_AND_FLOOR(s->listNum,s->buttonNum));
	button_setName(s->naviButton+0,s->naviBuf);
	button_refreshName(s->naviButton,0,0);
	button_setFunc(s->naviButton+0,hmi_list_navi_ret_func);
	button_setFunc(s->naviButton+1,hmi_list_navi_prev_func);
	button_setFunc(s->naviButton+2,hmi_list_navi_next_func);

	hmi_list_refresh(s);
}

void hmi_list_refresh(hmi_list_t *s){
	for(int i=0;i<s->buttonNum;i++){
		int listIndex=i+s->curIndex;
		button_t *curButton=&s->listButton[i];
		if(listIndex < s->listNum){
			hmi_item_t *curItem=&s->list[listIndex];
			button_setName(curButton,curItem->name);
			switch(curItem->type){
				case U8:
					rt_snprintf(curItem->value,LIST_VALUE_LEN,"%u",*(uint8_t*)curItem->lp);
					break;
				case U16:
					rt_snprintf(curItem->value,LIST_VALUE_LEN,"%u",*(uint16_t*)curItem->lp);
					break;
				case U32:
					rt_snprintf(curItem->value,LIST_VALUE_LEN,"%u",*(uint32_t*)curItem->lp);
					break;
				case S8:
					rt_snprintf(curItem->value,LIST_VALUE_LEN,"%d",*(int8_t*)curItem->lp);
					break;
				case S16:
					rt_snprintf(curItem->value,LIST_VALUE_LEN,"%d",*(int16_t*)curItem->lp);
					break;
				case S32:
					rt_snprintf(curItem->value,LIST_VALUE_LEN,"%d",*(int32_t*)curItem->lp);
					break;
				case FP:
					snprintf(curItem->value,LIST_VALUE_LEN,"%f",*(float*)curItem->lp);
					break;
				case DFP:
					snprintf(curItem->value,LIST_VALUE_LEN,"%lf",*(double*)curItem->lp);
					break;
				case STRING:
					rt_snprintf(curItem->value,LIST_VALUE_LEN,"%s",(char*)curItem->lp);
					break;
				case STRINGLP:
					rt_snprintf(curItem->value,LIST_VALUE_LEN,"%s",*(char**)curItem->lp);
					break;
				case CALLBACK:
					(*(void (*)(hmi_item_t *u))(curItem->lp))(curItem);
					break;
				default:
					break;
			}
			switch(curItem->type){
				case FUNC:
					button_setFunc(curButton,(button_cb)curItem->lp);
					button_refreshName(curButton,"<",">");
					break;
				default:
					button_setFunc(curButton,0);
					button_refreshName(curButton,0,curItem->value);
					break;
			}
		}else{
			button_setFunc(curButton,0);
			button_setName(curButton,0);
			button_refreshName(curButton,0,0);
		}
	}
}


static void hmi_list_navi_next_func(button_t *this){
	hmi_list_t *s=this->parent;
	if(s->curIndex + s->buttonNum < s->listNum){
		s->curIndex += s->buttonNum;
		hmi_list_refresh(s);
		sprintf(s->naviBuf,"%d/%d", s->curIndex/s->buttonNum+1 , DIV_AND_FLOOR(s->listNum,s->buttonNum));
		button_refreshName(s->naviButton,0,0);
	}		
}
static void hmi_list_navi_prev_func(button_t *this){
	hmi_list_t *s=this->parent;
	if(s->curIndex > 0){
		s->curIndex -= s->buttonNum;
		if(s->curIndex < 0)s->curIndex=0;
		hmi_list_refresh(s);
		sprintf(s->naviBuf,"%d/%d", s->curIndex/s->buttonNum+1 , DIV_AND_FLOOR(s->listNum,s->buttonNum));
		button_refreshName(s->naviButton,0,0);
	}		
}
static void hmi_list_navi_ret_func(button_t *this){
	hmi_list_t *s=this->parent;
	s->curIndex = 0;
	hmi_list_refresh(s);
	sprintf(s->naviBuf,"%d/%d", s->curIndex/s->buttonNum+1 , DIV_AND_FLOOR(s->listNum,s->buttonNum));
	button_refreshName(s->naviButton,0,0);		
}












