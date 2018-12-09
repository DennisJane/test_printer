#include "iwdg.h"
#include "stm32f4xx.h"


void IWDT_INIT(void){
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	/* IWDG counter clock: LSI/32 */
	IWDG_SetPrescaler(IWDG_Prescaler_256);	//6.4ms/load
	IWDG_SetReload(1000);	//6.4s
	/* Reload IWDG counter */
	IWDG_ReloadCounter();
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
	
}
