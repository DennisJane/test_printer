#include "stm32f4xx.h"
#include "tim1_timer.h"

void RCC_Configuration(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
}

static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

void TIM1_TIMER_INIT(void)
{
	//uint16_t PrescalerValue;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_Configuration();
	NVIC_Configuration();

	/*If the APB prescaler (PPRE1, PPRE2 in the RCC_CFGR register) is configured to a 
	division factor of 1, TIMxCLK = PCLKx. Otherwise, the timer clock frequencies are set to 
	twice to the frequency of the APB domainto which the timers are connected:
	TIMxCLK = 2xPCLKx.*/
	//TIM3CLK = 2 * PCLK1
    //PCLK1 = HCLK / 2 => TIM3CLK = HCLK = SystemCoreClock
	//3M/3000=1khz
	//PrescalerValue = (uint16_t) ((SystemCoreClock) / 3000000) - 1;
	TIM_TimeBaseStructure.TIM_Period = 24000-1;	//24Mhz/24k=1k
	TIM_TimeBaseStructure.TIM_Prescaler = 7-1;	//168M/7=24Mhz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	/* TIM IT enable */
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	
	TIM1_setPreload(true);
}


