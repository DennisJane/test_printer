#include "libConfig.h"
#include "libStepMotor.h"
#include "libTrigger.h"
#include "stm32f4xx.h"



//inv为反向
//true为触发,false为松开
//fake为始终为松开
//X Y E A B C

//---------------------------TDPRINTER-A----------------------
#if defined(TDPRINTER_A)

stepMotor_t defaultMotor[STEPPER_NUM]={
//en,clk,dir  inv[3]
[0]={{GPIOA,GPIO_Pin_3 ,1},{GPIOC,GPIO_Pin_4 ,0}, {GPIOA,GPIO_Pin_7 ,0}, true,false,false},
[1]={{GPIOA,GPIO_Pin_6 ,1},{GPIOA,GPIO_Pin_5 ,0}, {GPIOA,GPIO_Pin_4 ,0}, true,false,true},
[2]={{GPIOE,GPIO_Pin_7 ,1},{GPIOE,GPIO_Pin_8 ,0}, {GPIOE,GPIO_Pin_9 ,0}, true,false,false},
[3]={{GPIOC,GPIO_Pin_5 ,1},{GPIOB,GPIO_Pin_0 ,0}, {GPIOB,GPIO_Pin_2 ,0}, true,false,false},
[4]={{GPIOE,GPIO_Pin_14,1},{GPIOE,GPIO_Pin_15,0}, {GPIOB,GPIO_Pin_11,0}, true,false,false},
[5]={{GPIOE,GPIO_Pin_10,1},{GPIOE,GPIO_Pin_11,0}, {GPIOE,GPIO_Pin_13,0}, true,false,false},
};

//true为触发
trigger_t defaultMaxTrigger[STEPPER_NUM]={
	//gpio , inv , fake
	[0]={{GPIOD,GPIO_Pin_11,1},false,false},
	[1]={{GPIOD,GPIO_Pin_12,1},false,true },
	[2]={{GPIOD,GPIO_Pin_13,1},false,true },	//E
	[3]={{GPIOD,GPIO_Pin_13,1},false,false},
	[4]={{GPIOD,GPIO_Pin_14,1},false,false},
	[5]={{GPIOD,GPIO_Pin_15,1},false,false},
};
trigger_t defaultMinTrigger[STEPPER_NUM]={
	//gpio , inv , fake
	[0]={{GPIOD,GPIO_Pin_11,1},false,true},
	[1]={{GPIOD,GPIO_Pin_12,1},false,false},
	[2]={{GPIOD,GPIO_Pin_13,1},false,true},		//E
	[3]={{GPIOD,GPIO_Pin_13,1},false,true},
	[4]={{GPIOD,GPIO_Pin_14,1},false,true},
	[5]={{GPIOD,GPIO_Pin_15,1},false,true},
};

//default param defined in param.c:param_reset()
/*
float defaultStep2mm[STEPPER_NUM]={
	80.0f,
	80.0f,
	96.275202f,
	1595.74468f,
	1595.74468f,
	1595.74468f,
};*/

//---------------------------TDPRINTER-B----------------------
#elif defined(TDPRINTER_B)


stepMotor_t defaultMotor[STEPPER_NUM]={
//en,clk,dir  inv[3]
[0]={{GPIOC,GPIO_Pin_14,1},{GPIOA,GPIO_Pin_0 ,0}, {GPIOC,GPIO_Pin_3 ,0}, true,false,false},
[1]={{GPIOC,GPIO_Pin_2 ,1},{GPIOC,GPIO_Pin_1 ,0}, {GPIOC,GPIO_Pin_0 ,0}, true,false,true},
[2]={{GPIOA,GPIO_Pin_4 ,1},{GPIOA,GPIO_Pin_5 ,0}, {GPIOA,GPIO_Pin_6 ,0}, true,false,false},
[3]={{GPIOA,GPIO_Pin_1 ,1},{GPIOA,GPIO_Pin_2 ,0}, {GPIOA,GPIO_Pin_3 ,0}, true,false,false},
};

trigger_t defaultMaxTrigger[STEPPER_NUM]={
	//gpio , inv , fake
	[0]={{GPIOB,GPIO_Pin_12,1},false,false},
	[1]={{GPIOB,GPIO_Pin_13,1},false,true },
	[2]={{GPIOB,GPIO_Pin_14,1},false,true },	//E
	[3]={{GPIOB,GPIO_Pin_15,1},false,true},
};
trigger_t defaultMinTrigger[STEPPER_NUM]={
	//gpio , inv , fake
	[0]={{GPIOB,GPIO_Pin_12,1},false,true},
	[1]={{GPIOB,GPIO_Pin_13,1},false,false},
	[2]={{GPIOB,GPIO_Pin_14,1},false,true},		//E
	[3]={{GPIOB,GPIO_Pin_15,1},false,false},
};

//---------------------------TDPRINTER-BRAIN----------------------
#elif defined(TDPRINTER_BRAIN)

stepMotor_t defaultMotor[STEPPER_NUM]={
//en,clk,dir  inv[3]
[0]={{GPIOC,GPIO_Pin_3,1},{GPIOC,GPIO_Pin_2,0},{GPIOB,GPIO_Pin_9,0},true,false,true},
[1]={{GPIOC,GPIO_Pin_5,1},{GPIOA,GPIO_Pin_1,0},{GPIOA,GPIO_Pin_0,0},true,false,true},
[2]={{GPIOB,GPIO_Pin_8,1},{GPIOB,GPIO_Pin_7,0},{GPIOC,GPIO_Pin_12,0},true,false,true},
[3]={{GPIOC,GPIO_Pin_1,1},{GPIOC,GPIO_Pin_0,0},{GPIOC,GPIO_Pin_4,0},true,false,true},
};
 
trigger_t defaultMaxTrigger[STEPPER_NUM]={
	//gpio , inv , fake
	[0]={{GPIOC,GPIO_Pin_9,1},false,false},
	[1]={{GPIOC,GPIO_Pin_8,1},false,true },
	[2]={{GPIOC,GPIO_Pin_6,1},false,true },	//E
	[3]={{GPIOC,GPIO_Pin_7,1},false,true},
};
trigger_t defaultMinTrigger[STEPPER_NUM]={
	//gpio , inv , fake
	[0]={{GPIOC,GPIO_Pin_9,1},false,true},
	[1]={{GPIOC,GPIO_Pin_8,1},false,false},
	[2]={{GPIOC,GPIO_Pin_6,1},false,true},		//E
	[3]={{GPIOC,GPIO_Pin_7,1},false,false},
};

/*
float defaultStep2mm[STEPPER_NUM]={
	79.7158674,
	79.7158674,
	96.275202,
	13003.90117,
};

//maxZ=400/60=7mm/s = 1600*7 /s =11200 steps/s
//maxX=2000/60=33.33mm/s = 2666 steps/s
float defaultMaxRate[STEPPER_NUM]={
	3000.0f,
	3000.0f,
	1000.0f,
	10000.0f
};*/

#endif



