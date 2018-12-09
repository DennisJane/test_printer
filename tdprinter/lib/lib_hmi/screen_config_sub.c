#include "hmi_hal.h"
#include "screen_main.h"
#include "hmi_button.h"
#include "hmi_list.h"
#include "lib/corePrinter.h"
#include "app/app_subboard.h"
#include "lib/param/param.h"


extern corePrinter_t cp;
int gTemperLocker=0;
static char tempMethodAuto[]={"�¶�[�Զ�]:0"};

static void temperLocker(hmi_item_t *u){
	if(gTemperLocker==0){
		gTemperLocker=1;
		if(cp.gcodeDevice.currentDevice==0){
			sboard_setTemper(DEFAULT_TEMP);
		}
		sboard_setLock(1);
		sprintf(tempMethodAuto,"�¶�[�ֶ�]:%d",sboard_getTemperDst());
	}else{
		gTemperLocker=0;
		sboard_setLock(0);
		if(cp.gcodeDevice.currentDevice==0){
			sboard_setTemper(0);
		}
		sprintf(tempMethodAuto,"�¶�[�Զ�]:%d",sboard_getTemperDst());
	}
}
static void temperModify(uint16_t temper){
	if(gTemperLocker==1){
		sboard_setLock(0);
		sboard_setTemper(temper);
		sboard_setLock(1);
		sprintf(tempMethodAuto,"�¶�[�ֶ�]:%d",sboard_getTemperDst());
	}
}
static void temperP1(hmi_item_t *u){
	if(sboard_getTemperDst()>DEFAULT_TEMP+50)return;
	temperModify(sboard_getTemperDst()+1);
}
static void temperM1(hmi_item_t *u){
	if(sboard_getTemperDst()<DEFAULT_TEMP-50)return;
	temperModify(sboard_getTemperDst()-1);
}
static void temperP10(hmi_item_t *u){
	if(sboard_getTemperDst()>DEFAULT_TEMP+50)return;
	temperModify(sboard_getTemperDst()+10);
}
static void temperM10(hmi_item_t *u){
	if(sboard_getTemperDst()<DEFAULT_TEMP-50)return;
	temperModify(sboard_getTemperDst()-10);
}
static void temperE0(hmi_item_t *u){
	temperModify(DEFAULT_TEMP);
}

extern void status_hmi_chang(bool flag);
static void stopPrint(void){
	corePrinter_stop(&cp);
	status_hmi_chang(0);
}


static void speedFactorP1(void){
	cp.gcodeDevice.G1SpeedFactor+=0.1f;
	cp.gcodeDevice.G0SpeedFactor=cp.gcodeDevice.G1SpeedFactor;
}
static void speedFactorM1(void){
	cp.gcodeDevice.G1SpeedFactor-=0.1f;
	cp.gcodeDevice.G1SpeedFactor=(cp.gcodeDevice.G1SpeedFactor<0.2f)?0.2f:cp.gcodeDevice.G1SpeedFactor;
	cp.gcodeDevice.G0SpeedFactor=cp.gcodeDevice.G1SpeedFactor;
}
static void speedFactorP10(void){
	cp.gcodeDevice.G1SpeedFactor+=1.0f;
	cp.gcodeDevice.G0SpeedFactor=cp.gcodeDevice.G1SpeedFactor;
}
static void speedFactorM10(void){
	cp.gcodeDevice.G1SpeedFactor-=1.0f;
	cp.gcodeDevice.G1SpeedFactor=(cp.gcodeDevice.G1SpeedFactor<0.2f)?0.2f:cp.gcodeDevice.G1SpeedFactor;
	cp.gcodeDevice.G0SpeedFactor=cp.gcodeDevice.G1SpeedFactor;
}
static void speedFactorReset(void){
	cp.gcodeDevice.G1SpeedFactor=1.0f;
	cp.gcodeDevice.G0SpeedFactor=cp.gcodeDevice.G1SpeedFactor;
}




static void zOffsetP1(hmi_item_t *u){
	param->positionOffset[3]+=0.02f;
}
static void zOffsetM1(hmi_item_t *u){
	param->positionOffset[3]-=0.02f;
}
static void zOffsetP10(hmi_item_t *u){
	param->positionOffset[3]+=0.50f;
}
static void zOffsetM10(hmi_item_t *u){
	param->positionOffset[3]-=0.50f;
}
static void zOffsetE0(hmi_item_t *u){
	param->positionOffset[3]=0.0f;
}


const static char *retStr[]={
"����ɹ�",
"����ʧ��",
"����ɹ�",
"����ʧ��",
"��λ�ɹ�",
"��λʧ��"
};
const static char *ret=" ";
static void static_param_save(void){
	if(param_save()==0){
		ret=retStr[0];
	}else{
		ret=retStr[1];
	}
}
static void static_param_load(void){
	if(param_load()==0){
		ret=retStr[2];
	}else{
		ret=retStr[3];
	}
}
static void static_param_reset(void){
	if(param_reset()==0){
		ret=retStr[4];
	}else{
		ret=retStr[5];
	}
}

void retarget_uartCode(char *c);

static void enableAll(void){
	retarget_uartCode("G91\r\nM11 XYEZ\r\n");
}
static void enableX(void){
	retarget_uartCode("G91\r\nM11 X\r\n");
}
static void enableY(void){
	retarget_uartCode("G91\r\nM11 Y\r\n");
}
static void enableE(void){
	retarget_uartCode("G91\r\nM11 E\r\n");
}
static void enableZ(void){
	retarget_uartCode("G91\r\nM11 Z\r\n");
}


static void ZP100(void){
	retarget_uartCode("G28 Z1 F600");
}

static void ZM100(void){
	retarget_uartCode("G28 Z-1 F600");
}
static void ZP1000(void){
	retarget_uartCode("G28 Z10 F600");
}
static void ZM1000(void){
	retarget_uartCode("G28 Z-10 F600");
}
static void EP1(void){
	retarget_uartCode("G28 E1 F120");
}
static void EP10(void){
	retarget_uartCode("G28 E10 F120");
}
static void EM1(void){
	retarget_uartCode("G28 E-1 F120");
}
static void EM10(void){
	retarget_uartCode("G28 E-10 F120");
}
static void XP1(void){
	retarget_uartCode("G28 X1 F3000");
}
static void XP10(void){
	retarget_uartCode("G28 X10 F3000");
}
static void XM1(void){
	retarget_uartCode("G28 X-1 F3000");
}
static void XM10(void){
	retarget_uartCode("G28 X-10 F3000");
}
static void YP1(void){
	retarget_uartCode("G28 Y1 F3000");
}
static void YP10(void){
	retarget_uartCode("G28 Y10 F3000");
}
static void YM1(void){
	retarget_uartCode("G28 Y-1 F3000");
}
static void YM10(void){
	retarget_uartCode("G28 Y-10 F3000");
}


extern void switch_dialog(int flag,char *str);
static void  in_filament(void){
	if(!cp.gcodeDevice.currentDevice)
		switch_dialog(2,"������...");
}

static void  out_filament(void){
	if(!cp.gcodeDevice.currentDevice)
		switch_dialog(3,"������...");
}

static void  level_plate(void){
	if(!cp.gcodeDevice.currentDevice)
		switch_dialog(4,"���ڳ�ʼ��λ��...");
}

//static void switchToItems2(button_t *b);
//static hmi_item_t itemsL1[]={
//	//page
//	{"�ٶȿ���",FUNC,switchToItems2,},
//	{"�¶ȿ���",FUNC,switchToItems2,},
//	{"�������",FUNC,switchToItems2,},
//	{"��ƽ",FUNC,level_plate,},
//	{"����",FUNC,in_filament,},
//	{"����",FUNC,out_filament,},
//	
//	{"�������",FUNC,static_param_save,},
//	{"��λ����",FUNC,static_param_reset,},
//	{" ",STRINGLP,&ret,},
//};

static hmi_item_t itemsL2pre_operation[]={
	//page
	{"��ƽ",FUNC,level_plate,},
	{"����",FUNC,in_filament,},
	{"����",FUNC,out_filament,},
};

static hmi_item_t itemsL2Speed[]={
	//page
	{"�ٶ�ϵ��:",FP,&cp.gcodeDevice.G1SpeedFactor,},
	{"�ٶ�ϵ��+0.1:",FUNC,speedFactorP1,},
	{"�ٶ�ϵ��-0.1:",FUNC,speedFactorM1,},
	{"�ٶ�ϵ��+1.0:",FUNC,speedFactorP10,},
	{"�ٶ�ϵ��-1.0",FUNC,speedFactorM10,},
	{"�ٶ�ϵ��=1.0",FUNC,speedFactorReset,},
};

static hmi_item_t itemsL2Temp[]={
	//page
	{(char*)tempMethodAuto,FUNC,temperLocker,},
	{"Ŀ���¶�+1",FUNC,temperP1,},
	{"Ŀ���¶�-1",FUNC,temperM1,},
	{"Ŀ���¶�+10",FUNC,temperP10,},
	{"Ŀ���¶�-10",FUNC,temperM10,},
	{"Ŀ���¶�=Ĭ��",FUNC,temperE0,},
};
static hmi_item_t itemsL2Moto[]={
	//page
	{"ʹ�ܵ��ALL:",FUNC,enableAll,},
	{"�رյ��ALL:",FUNC,stopPrint,},
	{"ʹ�ܵ��X:",FUNC,enableX,},
	{"ʹ�ܵ��Y:",FUNC,enableY,},
	{"ʹ�ܵ��Z:",FUNC,enableZ,},
	{"ʹ�ܵ��E:",FUNC,enableE,},	

	{"X������1mm:",FUNC,XP1,},
	{"X������1mm:",FUNC,XM1,},
	{"X������10mm:",FUNC,XP10,},
	{"X������10mm:",FUNC,XM10,},
	{"Y�����1mm:",FUNC,YP1,},
	{"Y��ǰ��1mm:",FUNC,YM1,},
	
	{"Y�����10mm:",FUNC,YP10,},
	{"Y��ǰ��10mm:",FUNC,YM10,},
	{"Z������1mm:",FUNC,ZP100,},
	{"Z������1mm:",FUNC,ZM100,},
	{"Z������10mm:",FUNC,ZP1000,},
	{"Z������10mm:",FUNC,ZM1000,},
	
	{"����1mm:",FUNC,EP1,},
	{"����1mm:",FUNC,EM1,},
	{"����10mm:",FUNC,EP10,},
	{"����10mm:",FUNC,EM10,},
};
/***************************************************************/
extern uint8_t fan1,fan2;
extern float fanrate;
bool fanLocker=0;
char str_enfan1[]={"����ȿ���"};
char str_fanAuto[]={"����ϵ��[�Զ�]"};
static int bkfan1=0;
static int bkfan2=0;

static void fanLock(hmi_item_t *u)
{
	if(fanLocker==0)
	{
		fanLocker=1;
		bkfan1=fan1;
		bkfan2=fan2;		
		sprintf(str_fanAuto,"����ϵ��[�ֶ�]:%2.1f",fanrate);
	}
	else
	{
		fanLocker=0;
		sboard_setFan1(bkfan1);
		sboard_setFan2(bkfan2);
		sprintf(str_fanAuto,"����ϵ��[�Զ�]");
		sprintf(str_enfan1,"����ȿ���");
		fanrate=1.0;
	}
}

static void enableFan1(hmi_item_t *u)
{
	if(fanLocker)	
	{
		if(fan1==0)
		{
			sboard_setFan1(120);	
			sprintf(str_enfan1,"�ر������");
		}
		else
		{
			sboard_setFan1(0);
			sprintf(str_enfan1,"���������");			
		}
	}
}

static void fanrateinc(hmi_item_t *u)
{
	if(fanLocker)	
	{
		fanrate=fanrate+0.1;
		fanrate=fanrate>3?3:fanrate;
		sprintf(str_fanAuto,"����ϵ��[�ֶ�]:%2.1f",fanrate);
	}
}

static void fanratedec(hmi_item_t *u)
{
	if(fanLocker)
	{
		fanrate=fanrate-0.1;
		fanrate=fanrate<0.2?0.2:fanrate;
		sprintf(str_fanAuto,"����ϵ��[�ֶ�]:%2.1f",fanrate);
	}
}

static hmi_item_t itemsL2fan[]={
	//page
	{(char*)str_fanAuto,FUNC,fanLock,},
	{(char*)str_enfan1,FUNC,enableFan1,},
//	{(char*)str_enfan2,FUNC,enableFan2,},
//	{"����ϵ��:",FP,&fanrate,},
	{"����ϵ��+0.1",FUNC,fanrateinc,},
	{"����ϵ��-0.1",FUNC,fanratedec,},
};

/**************************************************************/
static void onLoad(hscreen_t *s);
static void onTimer(hscreen_t *s);
static void onButton(hscreen_t *s,int id);

extern hscreen_t configItem;

extern button_t item[ITEM_NUM];
extern button_t navi[NAVI_NUM];


typedef struct{
	int sid;
	button_t *item;
	button_t *navi;
	hmi_list_t *list;
}userdata_t;

static hmi_list_t list={
	&configItem,
	0,
	//0,0,
	sizeof(itemsL2Speed)/sizeof(hmi_item_t),ITEM_NUM,
	item,navi,
};

static userdata_t userData={0,item,navi,&list};

hscreen_t configItem={
	0,					//id
	0,					//prev
	onLoad,
	onButton,
	onTimer,
	&userData,	//user data
};


void switchToItems2(button_t *b){
	userdata_t *u=configItem.data;
	hmi_list_t *temp_list=u->list;
	switch(b->bid)
	{
		case 3:  
			temp_list->list=itemsL2Speed;
			temp_list->listNum=sizeof(itemsL2Speed)/sizeof(hmi_item_t);
			break;
		case 4:
			temp_list->list=itemsL2Temp;
			temp_list->listNum=sizeof(itemsL2Temp)/sizeof(hmi_item_t);
			break;
		case 5:
			temp_list->list=itemsL2Moto;
			temp_list->listNum=sizeof(itemsL2Moto)/sizeof(hmi_item_t);
			break;
		case 6:
			temp_list->list=itemsL2pre_operation;
			temp_list->listNum=sizeof(itemsL2pre_operation)/sizeof(hmi_item_t);
			break;
		case 7:
			temp_list->list=itemsL2fan;
			temp_list->listNum=sizeof(itemsL2fan)/sizeof(hmi_item_t);
			break;
	}	
	hmi_list_load(temp_list);
}

extern hscreen_t configScreen;

static void onLoad(hscreen_t *s){
		
	//hmi_switch_screen(&hmi,hmi_screen_jump,&configScreen);
//	//rt_kprintf("screen_status:onLoad\r\n");
//	userdata_t *u=s->data;
//	u->list->list=itemsL2Speed;
//	u->list->listNum=sizeof(itemsL2Speed)/sizeof(hmi_item_t);
//	u->list->curIndex=0;
//	hmi_list_load(u->list);
}

static void onTimer(hscreen_t *s){
	userdata_t *u=s->data;
	static int i=0;
	if(i++>10){
		i=0;
		hmi_list_refresh(u->list);
	}
}

static void onButton(hscreen_t *s,int id){
	userdata_t *u=s->data;
	if(button_checkPressed(u->item,ITEM_NUM,s->sid,id))return;
	if(button_checkPressed(u->navi,NAVI_NUM,s->sid,id))return;
}







