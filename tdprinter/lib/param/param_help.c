#include "param_help.h"
#include <rtthread.h>
#include "gps/interface.h"
#include "ahrs/ahrs.h"
#include "input/input_gcs.h"
#include "quatcopter/quatcopter.h"
#include "param/param_help.h"
#include "init.h"

#define PARAM_NUM 60
#define STATUS_NUM 100


static paramItem_t ramParam[PARAM_NUM];
static paramItem_t statusParam[STATUS_NUM];
static uint8_t dummy;
extern quatcopter_t q;

int paramHelpInit(void){
	// ram/flash config param
	{//dummy
		int i;
		for(i=0;i<PARAM_NUM;i++)
			paramItemInit(ramParam+i,U8,0,&dummy,"dummy");
	}
	paramItemInit(ramParam+0,U32,1,(void*)&(param_getRamParam()->version),"version");
	paramItemInit(ramParam+1,FP,3,(void*)param_getRamParam()->acc_offset,"accOff");
	paramItemInit(ramParam+2,FP,3,(void*)param_getRamParam()->acc_gain,"accGain");
	paramItemInit(ramParam+3,FP,3,(void*)param_getRamParam()->gyr_offset,"gyrOff");
	paramItemInit(ramParam+4,FP,3,(void*)param_getRamParam()->gyr_gain,"gyrGain");
	paramItemInit(ramParam+5,FP,3,(void*)param_getRamParam()->mag_offset,"magOff");
	paramItemInit(ramParam+6,FP,3,(void*)param_getRamParam()->mag_gain,"magGain");
	
	paramItemInit(ramParam+7,FP,6,(void*)&(param_getRamParam()->accXYParam),"accParamXY");
	paramItemInit(ramParam+8,FP,6,(void*)&(param_getRamParam()->accZParam),"accParamZ");
	paramItemInit(ramParam+9,FP,6,(void*)&(param_getRamParam()->rateXYParam),"rateParamXY");
	paramItemInit(ramParam+10,FP,6,(void*)&(param_getRamParam()->rateZParam),"rateParamZ");
	paramItemInit(ramParam+11,FP,6,(void*)&(param_getRamParam()->attXYParam),"attParamXY");
	paramItemInit(ramParam+12,FP,6,(void*)&(param_getRamParam()->attZParam),"attParamZ");
	paramItemInit(ramParam+13,FP,6,(void*)&(param_getRamParam()->spdXYParam),"spdParamXY");
	paramItemInit(ramParam+14,FP,6,(void*)&(param_getRamParam()->spdZParam),"spdParamZ");
	paramItemInit(ramParam+15,FP,6,(void*)&(param_getRamParam()->posXYParam),"posParamXY");
	paramItemInit(ramParam+16,FP,6,(void*)&(param_getRamParam()->posZParam),"posParamZ");
	
	paramItemInit(ramParam+21,FP,3,(void*)&(param_getRamParam()->input2rateXYParam),"i2rateXY:min,max,gain");
	paramItemInit(ramParam+22,FP,3,(void*)&(param_getRamParam()->input2rateZParam),"i2rateZ:m,m,g");
	paramItemInit(ramParam+23,FP,3,(void*)&(param_getRamParam()->input2angleXYParam),"i2angleXY:m,m,g");
	paramItemInit(ramParam+24,FP,3,(void*)&(param_getRamParam()->input2spdXYParam),"i2spdXY:m,m,g");
	paramItemInit(ramParam+25,FP,3,(void*)&(param_getRamParam()->input2spdZParam),"i2spdZ:m,m,g");
	
	paramItemInit(ramParam+26,FP,4,(void*)&(param_getRamParam()->altitudeParam),"altParam:vSumGain,TC,Delay,lp");
	paramItemInit(ramParam+27,FP,3,(void*)&(param_getRamParam()->qAltitudeParam),"qAlt(filter):spdErr,spdDelta,acc");
	
	paramItemInit(ramParam+28,S32,2,(void*)&(param_getRamParam()->naviDelayParam),"navi(filter):delay spd,pos 40ms");
	paramItemInit(ramParam+29,FP,6,(void*)&(param_getRamParam()->naviSpdParam),"navi(filter):spdParam PIDIOa");
	paramItemInit(ramParam+30,FP,6,(void*)&(param_getRamParam()->naviPosParam),"navi(filter):posParam PIDIOa");
	paramItemInit(ramParam+31,FP,1,(void*)&(param_getRamParam()->naviAccFiltParam),"navi(filter):acc filter a");
	
	
	paramItemInit(ramParam+32,FP,1,(void*)&(param_getRamParam()->powerStop),"powerStop");
	paramItemInit(ramParam+33,FP,1,(void*)&(param_getRamParam()->powerStart),"powerStart");
	paramItemInit(ramParam+34,FP,1,(void*)&(param_getRamParam()->powerScale),"powerScale");
	paramItemInit(ramParam+35,U32,1,(void*)&(param_getRamParam()->motorNum),"motorNum");
	paramItemInit(ramParam+36,U32,1,(void*)&(param_getRamParam()->motorMatrixMask),"motorMask");
	paramItemInit(ramParam+37,FP,4,(void*)(param_getRamParam()->motorMatrix[0]),"motorMatrix1");
	paramItemInit(ramParam+38,FP,4,(void*)(param_getRamParam()->motorMatrix[1]),"motorMatrix2");
	paramItemInit(ramParam+39,FP,4,(void*)(param_getRamParam()->motorMatrix[2]),"motorMatrix3");
	paramItemInit(ramParam+40,FP,4,(void*)(param_getRamParam()->motorMatrix[3]),"motorMatrix4");
	paramItemInit(ramParam+41,FP,4,(void*)(param_getRamParam()->motorMatrix[4]),"motorMatrix5");
	paramItemInit(ramParam+42,FP,4,(void*)(param_getRamParam()->motorMatrix[5]),"motorMatrix6");
	paramItemInit(ramParam+43,FP,4,(void*)(param_getRamParam()->motorMatrix[6]),"motorMatrix7");
	paramItemInit(ramParam+44,FP,4,(void*)(param_getRamParam()->motorMatrix[7]),"motorMatrix8");
	paramItemInit(ramParam+45,FP,3,(void*)&(param_getRamParam()->localAccelVG),"localAccelVG");
	paramItemInit(ramParam+46,FP,3,(void*)&(param_getRamParam()->localMagnetVG),"localMagnetV1");
	paramItemInit(ramParam+47,FP,3,(void*)&(param_getRamParam()->attAccMixFactor),"attAccMixFct[3]");
	paramItemInit(ramParam+48,FP,3,(void*)&(param_getRamParam()->attMagMixFactor),"attMagMixFct[3]");
	paramItemInit(ramParam+49,FP,1,(void*)&(param_getRamParam()->gravity),"gravity:G");
	paramItemInit(ramParam+50,U32,1,(void*)&(param_getRamParam()->minSV),"gpsMinSV");
	paramItemInit(ramParam+51,FP,1,(void*)&(param_getRamParam()->motorFixGain),"motorFixGain");
	paramItemInit(ramParam+52,FP,4,(void*)&(param_getRamParam()->horizon),"horizon");
		
	//status param
	{//dummy
		int i;
		for(i=0;i<STATUS_NUM;i++)
			paramItemInit(statusParam+i,U8,0,&dummy,"dummy");
	}
	{//基本测量
		#define INERTIAL_BASE 0
		rt_tick_t *rt_tick_getLp(void);
		int16_t* getSensorRawData_PM(char type);
		#define adis getSensorRawData_PM
		paramItemInit(statusParam+INERTIAL_BASE+0,U32,1,(void*)rt_tick_getLp(),"tickCounter");
		paramItemInit(statusParam+INERTIAL_BASE+1,S16,3,(void*)adis('a'),"rawAcc");
		paramItemInit(statusParam+INERTIAL_BASE+2,S16,3,(void*)adis('g'),"rawGyr");
		paramItemInit(statusParam+INERTIAL_BASE+3,S16,3,(void*)adis('m'),"rawMag");
		paramItemInit(statusParam+INERTIAL_BASE+4,FP,3,(void*)q.ahrs.caliAcc,"caliAcc");
		paramItemInit(statusParam+INERTIAL_BASE+5,FP,3,(void*)q.ahrs.caliGyr,"caliGyr");
		paramItemInit(statusParam+INERTIAL_BASE+6,FP,3,(void*)q.ahrs.caliMag,"caliMag");
		paramItemInit(statusParam+INERTIAL_BASE+7,FP,3,(void*)q.ahrs.caliAcc,"filtAcc");
		paramItemInit(statusParam+INERTIAL_BASE+8,FP,3,(void*)q.ahrs.caliGyr,"filtGyr");
		paramItemInit(statusParam+INERTIAL_BASE+9,FP,3,(void*)q.ahrs.caliMag,"filtMag");
	}
	{//姿态测量
		paramItemInit(statusParam+10,FP,3,(void*)&(q.eula),"q0:eula");
		paramItemInit(statusParam+11,FP,4,(void*)&(q.ahrs.attitude),"attQuat");
		paramItemInit(statusParam+12,FP,4,(void*)&(q.ahrs.rawAttitude),"rawAttQuat");
		
	}
	{//gps raw
		#define GPS_BASE 13
		gps_info_t* getNeo6mScope();
		gps_info_t *gps=getNeo6mScope();
		paramItemInit(statusParam+GPS_BASE+0,S32,3,(void*)(&gps->status),"gps:st,numSV,iTOW");
		paramItemInit(statusParam+GPS_BASE+1,FP,3,(void*)(&gps->enuSpd),"gps:enuSpd");
		paramItemInit(statusParam+GPS_BASE+2,DFP,3,(void*)(&gps->lla_double),"gps:lla");
		paramItemInit(statusParam+GPS_BASE+3,FP,3,(void*)(&gps->ecefPos),"gps:ecefPos");
		paramItemInit(statusParam+GPS_BASE+4,FP,3,(void*)(&gps->pAcc),"gps:posA,spdA,pDop");
	}
	{//SBUS
		short* sbusGetStatus(void);
		short* sbusGetChannel(void);
		//float* getControlValue(void);
		//paramItemInit(statusParam+21,S16,1,(void*)sbusGetStatus(),"sbusStatus");
		//paramItemInit(statusParam+22,S16,9,(void*)sbusGetChannel(),"sbus0-9");
		//paramItemInit(statusParam+23,S16,9,(void*)(sbusGetChannel()+9),"sbus10-18");
		paramItemInit(statusParam+18,FP,4,(void*)input_gcsGetRaw(),"RC_input");
	}
	{//电机控制
		paramItemInit(statusParam+19,FP,4,(void*)q.motor.output,"motorPower");
		paramItemInit(statusParam+20,FP,4,(void*)q.motor.input,"ctlPower");
	}
	{//qStablizer
		#define QSTABLIZER_BASE 21
		extern float qstablizer_rateScope[6];
		extern float qstablizer_attScope[6];
		paramItemInit(statusParam+QSTABLIZER_BASE+0,FP,2,(void*)(qstablizer_rateScope+0),"q1:rateScopeX");
		paramItemInit(statusParam+QSTABLIZER_BASE+1,FP,2,(void*)(qstablizer_rateScope+2),"q1:rateScopeY");
		paramItemInit(statusParam+QSTABLIZER_BASE+2,FP,2,(void*)(qstablizer_rateScope+4),"q1:rateScopeZ");
		paramItemInit(statusParam+QSTABLIZER_BASE+3,FP,2,(void*)(qstablizer_attScope+0),"q1:attScopeX");
		paramItemInit(statusParam+QSTABLIZER_BASE+4,FP,2,(void*)(qstablizer_attScope+2),"q1:attScopeY");
		paramItemInit(statusParam+QSTABLIZER_BASE+5,FP,2,(void*)(qstablizer_attScope+4),"q1:attScopeZ");
		
	}
	{//qAltitude
		#define QALT_BASE 30
		paramItemInit(statusParam+QALT_BASE+0,FP,4,(void*)(q.motor.input),"q0.motor:input");
		paramItemInit(statusParam+QALT_BASE+1,FP,1,(void*)&(q.q2->lastThrottle),"q2:lastThrottle");
		paramItemInit(statusParam+QALT_BASE+2,FP,2,(void*)&(q.q2->posControler.pid[0].result),"spdZ:result,sum");
		paramItemInit(statusParam+QALT_BASE+3,FP,2,(void*)&(q.q2->accControler.pid[0].result),"accZ:result,sum");
		paramItemInit(statusParam+QALT_BASE+4,FP,1,(void*)&(q.q2->fakeOutput.values[0]),"q2:fakeOut[0]");
		extern float qAltitude_scope[6];
		paramItemInit(statusParam+QALT_BASE+5,FP,2,(void*)(qAltitude_scope+0),"qAltScope:posDst,posSrc");
		paramItemInit(statusParam+QALT_BASE+6,FP,2,(void*)(qAltitude_scope+2),"qAltScope:spdDst,spdSrc");
		paramItemInit(statusParam+QALT_BASE+7,FP,2,(void*)(qAltitude_scope+4),"qAltScope:accDst,accSrc");
	}
	{//point
		#define QPOINT_BASE 40
		paramItemInit(statusParam+QPOINT_BASE+0,DFP,3,(void*)&(q.q3->llaPosDst),"qpoint:llaPosDst");
		paramItemInit(statusParam+QPOINT_BASE+1,FP,3,(void*)&(q.q3->enuPos),"qpoint:enuPos");
		paramItemInit(statusParam+QPOINT_BASE+2,FP,3,(void*)&(q.q3->enuPosDst),"qpoint:enuPosDst");
		paramItemInit(statusParam+QPOINT_BASE+3,FP,3,(void*)&(q.q3->enuErr_round),"qpoint:enuPosErr_round");
		//paramItemInit(statusParam+QPOINT_BASE+4,FP,3,(void*)&(q.q3->enuSpd),"qpoint:enuSpd");
		//paramItemInit(statusParam+QPOINT_BASE+5,FP,3,(void*)&(q.q3->enuSpdDst),"qpoint:enuSpdDst");
		//paramItemInit(statusParam+QPOINT_BASE+6,FP,3,(void*)&(q.q3->enuAccelDst),"qpoint:enuAccDst");
		//paramItemInit(statusParam+QPOINT_BASE+7,FP,3,(void*)&(q.q3->bodyAccelDst),"qpoint:bodyAccDst");
		paramItemInit(statusParam+QPOINT_BASE+4,FP,1,(void*)&(q.q3->spdDstFilterParam),"qpoint:spdFilterParam");
		extern float qPoint_spdScope[4];
		extern float qPoint_posScope[4];
		paramItemInit(statusParam+QPOINT_BASE+5,FP,2,(void*)(qPoint_spdScope),"qpoint:spdXScope");
		paramItemInit(statusParam+QPOINT_BASE+6,FP,2,(void*)(qPoint_spdScope+2),"qpoint:spdYScope");
		paramItemInit(statusParam+QPOINT_BASE+7,FP,2,(void*)(qPoint_posScope),"qpoint:posXScope");
		paramItemInit(statusParam+QPOINT_BASE+8,FP,2,(void*)(qPoint_posScope+2),"qpoint:posYScope");
		paramItemInit(statusParam+QPOINT_BASE+9,FP,1,(void*)&(q.q3->roundRadian),"qpoint:roundRadian");
	}
	{//mode
		#define MODE_BASE 50
		paramItemInit(statusParam+MODE_BASE+0,U16,1,(void*)&q.mode.status,"flight:statusRaw");
		paramItemInit(statusParam+MODE_BASE+1,U8,1,(void*)&q.mode.flight,"flight:modeRaw");
		paramItemInit(statusParam+MODE_BASE+2,U8,1,(void*)&q.motor.mode,"motor:modeRaw");
		//+3
		paramItemInit(statusParam+MODE_BASE+4,U8,1,(void*)&q.mode.flightDst,"flight:modeDst");
		paramItemInit(statusParam+MODE_BASE+5,U8,1,(void*)&q.motor.mode,"flight:motor:modeRaw");
		//+4
	}
	{//NAVI
		#define NAVI_SCOPE_BASE 60
		paramItemInit(statusParam+NAVI_SCOPE_BASE+0,FP,3,(void*)q.enuAccel,"q0:enuAccel");
		paramItemInit(statusParam+NAVI_SCOPE_BASE+2,FP,2,(void*)q.navi.spdScope,"q0.navi:scopeSpdX(est,mes)");
		paramItemInit(statusParam+NAVI_SCOPE_BASE+3,FP,2,(void*)q.navi.posScope,"q0.navi:scopePosX(est,mes)");
		paramItemInit(statusParam+NAVI_SCOPE_BASE+4,FP,2,(void*)(q.navi.spdScope+2),"q0.navi:scopeSpdY(est,mes)");
		paramItemInit(statusParam+NAVI_SCOPE_BASE+5,FP,2,(void*)(q.navi.posScope+2),"q0.navi:scopePosY(est,mes)");
		paramItemInit(statusParam+NAVI_SCOPE_BASE+6,FP,2,(void*)(q.navi.spdScope+4),"q0.navi:scopeSpdZ(est,mes)");
		paramItemInit(statusParam+NAVI_SCOPE_BASE+7,FP,2,(void*)(q.navi.posScope+4),"q0.navi:scopePosZ(est,mes)");
	}
	{//Navi
		//paramItemInit(statusParam+32,FP,3,(void*)attitude_getNaviAccel(),"naviAccel");
	}
	{//navi
		/*#define NAVI_BASE 20
		paramItemInit(statusParam+NAVI_BASE+0,FP,3,(void*)(q.ahrs.caliAcc),"q0.ahrs:caliAcc");
		paramItemInit(statusParam+NAVI_BASE+1,FP,2,(void*)(q.navi.accel_m),"q0.navi:accFilt");
		paramItemInit(statusParam+NAVI_BASE+2,FP,2,(void*)(q.navi.accel_b),"q0.navi:accB");
		paramItemInit(statusParam+NAVI_BASE+3,FP,2,(void*)(q.navi.accel_e),"q0.navi:accE");
		paramItemInit(statusParam+NAVI_BASE+4,FP,2,(void*)(q.navi.speed_m),"q0.navi:spdM");
		paramItemInit(statusParam+NAVI_BASE+5,FP,2,(void*)(q.navi.speed_e),"q0.navi:spdE");
		paramItemInit(statusParam+NAVI_BASE+6,FP,2,(void*)(q.navi.speed_b),"q0.navi:spdB");
		paramItemInit(statusParam+NAVI_BASE+7,FP,2,(void*)(q.navi.posit_m),"q0.navi:posM");
		paramItemInit(statusParam+NAVI_BASE+8,FP,2,(void*)(q.navi.posit_e),"q0.navi:posE");*/
	}
	{//Altitude Mixer
		#define ALTMIX_BASE 70
		paramItemInit(statusParam+ALTMIX_BASE+0,FP,2,(void*)&q.altitude.altitudeEst,"baroAlt:AltEst,SpdEst");
		paramItemInit(statusParam+ALTMIX_BASE+1,FP,3,(void*)&q.enuAccel,"q0:enuAccel[3]");
		paramItemInit(statusParam+ALTMIX_BASE+2,FP,5,(void*)&q.altitude.filter.Am,"alt:Am,Hm,Hvd,Ab,Hb");
		paramItemInit(statusParam+ALTMIX_BASE+3,FP,3,(void*)&q.altitude.filter.Ve,"alt:Ve,He,Hv");
		paramItemInit(statusParam+ALTMIX_BASE+4,FP,3,(void*)&q.altitude.filter.k1,"alt.pidf:k1,k2,k3");
		extern float altitude_apm_scope[2];
		paramItemInit(statusParam+ALTMIX_BASE+5,FP,2,(void*)altitude_apm_scope,"alt_scope:rawAlt,altDelayed");
		paramItemInit(statusParam+ALTMIX_BASE+6,FP,1,(void*)&q.inertial->data.pressure,"sensor:rawAltnonFilt");
		/*float *getAltMixScope(void);
		float *getAltMixParam(void);
		paramItemInit(statusParam+33,FP,4,(void*)getAltMixParam(),"altMixParam:FB Ae,Ve,Hb,???");
		paramItemInit(statusParam+34,FP,3,(void*)getAltMixScope()    ,"altMixScope:Am,Ab,Ae");
		paramItemInit(statusParam+35,FP,3,(void*)(getAltMixScope()+3),"altMixScope:Vbe,Vae,Vdif");
		paramItemInit(statusParam+36,FP,3,(void*)(getAltMixScope()+6),"altMixScope:Hm,Hve,Hdif");*/
	}
	{//navi_kf
		extern float navi_kf_pos_est[3],navi_kf_vel_est[3];
		extern float navi_kf_posX_scope[2];
		extern float navi_kf_posY_scope[2];
		extern float navi_kf_spdX_scope[2];
		extern float navi_kf_spdY_scope[2];
		extern float kf_vel_RQ[2];
		extern float kf_pos_RQ[2];
		paramItemInit(statusParam+90,FP,2,(void*)(navi_kf_spdX_scope),"naviKF:velX_scope");
		paramItemInit(statusParam+91,FP,2,(void*)(navi_kf_spdY_scope),"naviKF:velY_scope");
		paramItemInit(statusParam+92,FP,2,(void*)(navi_kf_posX_scope),"naviKF:posX_scope");
		paramItemInit(statusParam+93,FP,2,(void*)(navi_kf_posY_scope),"naviKF:posY_scope");
		paramItemInit(statusParam+94,FP,2,(void*)(kf_vel_RQ),"naviKF:vel_RQ");
		paramItemInit(statusParam+95,FP,2,(void*)(kf_pos_RQ),"naviKF:pos_RQ");
		paramItemInit(statusParam+96,FP,3,(void*)(navi_kf_vel_est),"naviKF:vel");
		paramItemInit(statusParam+97,FP,3,(void*)(navi_kf_pos_est),"naviKF:pos");
	}
	{//DT
		extern int gCounter[];
		paramItemInit(statusParam+98,S32,3,gCounter,"cnt:press,rate,gps");
		paramItemInit(statusParam+99,FP,3,(void*)(&q.rateDT),"quat:rateDT,pressDT,gpsDT");
	}
	{//DEBUG
		//gps_info_t* gps_simulator_getRaw(void);
		//int* getLoggerParam(void);
		//paramItemInit(statusParam+68,S32,3,(void*)getLoggerParam(),"logger");
		
	}
	return 0;
}
//INIT_PARAM_FRAME_EXPORT(paramHelpInit);
void paramItemInit(paramItem_t *p,paramType_t type,uint16_t num,void *addr,char* descript){
	p->type=type;
	switch(type){
	case S8:
	case U8:
		p->size=1;
		break;
	case S16:
	case U16:
		p->size=2;
		break;
	case S32:
	case U32:
	case FP:
		p->size=4;
		break;
	default:
		p->size=8;
		break;
	}
	p->num=num;
	p->addr=addr;
	p->descript=descript;
}


paramItem_t* getRamParam(uint32_t index){
	if(index<PARAM_NUM)
	return ramParam+index;
	else
	return 0;
}

paramItem_t* getStatusParam(uint32_t index){
	if(index<STATUS_NUM)
	return statusParam+index;
	else
	return 0;
}

uint32_t getRamNum(void){
	return PARAM_NUM;
}
uint32_t getStatusNum(void){
	return STATUS_NUM;
}


