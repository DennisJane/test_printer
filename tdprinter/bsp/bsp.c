#include "bsp.h"
#include <stdio.h>
#include <rtthread.h>


const char *RTUART_NAME[]={
	"rtuart1",
	"rtuart2",
	"rtuart3",
	"rtuart4",
	"rtuart5",
	"rtuart6",
};

const USART_InitTypeDef stdout_config=
{
		STDOUT_BAUD,
		USART_WordLength_8b,
		USART_StopBits_1,
		USART_Parity_No,
		USART_Mode_Rx | USART_Mode_Tx,
		USART_HardwareFlowControl_None,
};

const struct serial_configure rtuart_default_config={
	BAUD_RATE_115200,
    DATA_BITS_8,
    STOP_BITS_1,
    PARITY_NONE,
    BIT_ORDER_LSB,
    NRZ_NORMAL,
    0,
};


tim_base_t tim1={
	1,
	TIM1,
	RCC_APB2PeriphClockCmd,
	RCC_APB2Periph_TIM1,
	{
		7-1,		//prescale: 168M/7=24Mhz
		TIM_CounterMode_Up,
		24000-1,	//24Mhz/24k=1k 
		0,			//clkDiv
		0,			//repet

	},
	{
		TIM1_UP_TIM10_IRQn,
		1,
		0,
		ENABLE,
	},
};
tim_base_t tim3={
	3,
	TIM3,
	RCC_APB1PeriphClockCmd,
	RCC_APB1Periph_TIM3,
	{
		7-1,		//prescale: 84M/7=12Mhz
		TIM_CounterMode_Up,
		24000-1,	//12Mhz/24k=0.5k 
		0,			//clkDiv
		0,			//repet
	},
	{
		TIM3_IRQn,
		0,
		0,
		DISABLE,
	},
};
tim_base_t tim4={
	4,
	TIM4,
	RCC_APB1PeriphClockCmd,
	RCC_APB1Periph_TIM4,
	{
		7-1,		//prescale: 84M/7=12Mhz
		TIM_CounterMode_Up,
		1200-1,	//12Mhz/(1.2k)=10k
		0,			//clkDiv
		0,			//repet
	},
	{
		TIM4_IRQn,
		1,
		1,
		ENABLE,
	},
};

pwm_t heater={
	2,
	TIM3,
	{
		TIM_OCMode_PWM1,
	    TIM_OutputState_Enable,
		TIM_OutputNState_Disable,
	    0,	//pulse
	    TIM_OCPolarity_Low,
		TIM_OCNPolarity_High,
		TIM_OCIdleState_Reset,
		TIM_OCNIdleState_Reset,
	},
	{GPIOB,GPIO_Pin_5,0},
	GPIO_AF_TIM3,
	0,
};

rt_serial_t rtuart1;
rt_serial_t rtuart2;
rt_serial_t rtuart3;
rt_serial_t rtuart4;
rt_serial_t rtuart5;
rt_serial_t rtuart6;
struct rt_spi_bus rtspi1;
struct rt_spi_bus rtspi2;
struct rt_spi_bus rtspi3;

gpio_t led1={GPIOC,GPIO_Pin_13,0};


gpio_t usb_otg_fs_vbus={GPIOA,GPIO_Pin_9,0};
gpio_t usb_otg_fs_dm={GPIOA,GPIO_Pin_11,0};
gpio_t usb_otg_fs_dp={GPIOA,GPIO_Pin_12,0};

gpio_t usb_otg_fs_ps={GPIOA,GPIO_Pin_10,1};
gpio_t usb_otg_fs_oc={GPIOD,GPIO_Pin_2,1};	//input pull-up
/*
gpio_t usb_otg_fs_vbus={GPIOB,GPIO_Pin_13,0};
gpio_t usb_otg_fs_dm={GPIOB,GPIO_Pin_14,0};
gpio_t usb_otg_fs_dp={GPIOB,GPIO_Pin_15,0};
gpio_t usb_otg_fs_ps={GPIOC,GPIO_Pin_4,1};
gpio_t usb_otg_fs_oc={GPIOC,GPIO_Pin_5,1};	//input pull-up
*/
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;
__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;
/**
* @}
*/ 


void bsp_core_init(void){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//2bit / 2bit
	core_init();
	uart_configure((uart_t*)&STDOUT_UART,(USART_InitTypeDef*)&stdout_config);
	printf("bsp:build time %s %s\r\n",__DATE__,__TIME__);
	extern uint32_t SystemCoreClock;
	printf("SysClock:%u\r\n",SystemCoreClock);
}
void rt_hw_console_output(const char *str){
	printf("%s",str);
}

void bsp_init(void){
	#if RTE_USART1 == 1
		rtuart1.config=rtuart_default_config;
		rt_uart_init(&rtuart1,(char*)RTUART_NAME[0],&uart1_dma);
	#endif
	#if RTE_USART2 == 1
		rtuart2.config=rtuart_default_config;
		rt_uart_init(&rtuart2,(char*)RTUART_NAME[1],&uart2_dma);
	#endif
	#if RTE_USART3 == 1
		rtuart3.config=rtuart_default_config;
		rt_uart_init(&rtuart3,(char*)RTUART_NAME[2],&uart3_dma);
	#endif
	#if RTE_UART4 == 1
		rtuart4.config=rtuart_default_config;
		rt_uart_init(&rtuart4,(char*)RTUART_NAME[3],&uart4_dma);
	#endif
	#if RTE_UART5 ==1
		rtuart5.config=rtuart_default_config;
		rt_uart_init(&rtuart5,(char*)RTUART_NAME[4],&uart5_dma);
	#endif
	#if RTE_USART6==1
		rtuart6.config=rtuart_default_config;
		rt_uart_init(&rtuart6,(char*)RTUART_NAME[5],&uart6_dma);
	#endif
	
	#if RTE_SPI1==1
		rt_spi_bus_init(&rtspi1,&spi1,"spi1");
	#endif
	#if RTE_SPI2==1
		rt_spi_bus_init(&rtspi2,&spi2,"spi2");
	#endif
	#if RTE_SPI3==1
		rt_spi_bus_init(&rtspi3,&spi3,"spi3");
	#endif
	
	rt_console_set_device(FINSH_DEVICE);
	#ifdef TDPRINTER_BRAIN
		max6675_init("spi3");
		sddev_init("spi1");
		
		tim_init(&tim3);
		pwm_init(&heater);
		tim_setEnable(&tim3,ENABLE);
	#else
		sddev_init("spi3");
		gpio_init(&led1,GPIO_Mode_OUT);
	#endif

	gpio_init_spd(&usb_otg_fs_vbus,GPIO_Mode_IN,GPIO_Speed_100MHz);
	gpio_init_spd(&usb_otg_fs_dm,GPIO_Mode_AF,GPIO_Speed_100MHz);
	gpio_init_spd(&usb_otg_fs_dp,GPIO_Mode_AF,GPIO_Speed_100MHz);
	gpio_init_spd(&usb_otg_fs_oc,GPIO_Mode_IN,GPIO_Speed_100MHz);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG1_FS); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG1_FS);
	/*GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_OTG2_FS); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_OTG2_FS);*/
	
	/* Init Host Library */
	USBH_Init(&USB_OTG_Core, 
	#ifdef USE_USB_OTG_FS  
			USB_OTG_FS_CORE_ID,
	#else 
			USB_OTG_HS_CORE_ID,
	#endif 
			&USB_Host,
			&USBH_MSC_cb, 
			&USR_cb);
}
void usb_thread_entry(void* parameter){
	while(1){
		USBH_Process(&USB_OTG_Core, &USB_Host);
		rt_thread_delay(1);
	}
}
















void USART1_IRQHandler(void)
{
#if RTE_USART1 == 1
    stm32_rtuart_handler(&rtuart1);
#endif
}
void USART2_IRQHandler(void)
{
#if RTE_USART2 == 1
    stm32_rtuart_handler(&rtuart2);
#endif
}
void USART3_IRQHandler(void)
{
#if RTE_USART3 == 1
    stm32_rtuart_handler(&rtuart3);
#endif
}
void USART4_IRQHandler(void)
{
#if RTE_UART4 == 1
    stm32_rtuart_handler(&rtuart4);
#endif
}
void USART5_IRQHandler(void)
{
#if RTE_UART5 == 1
    stm32_rtuart_handler(&rtuart5);
#endif
}
void USART6_IRQHandler(void)
{
#if RTE_USART6 == 1
    stm32_rtuart_handler(&rtuart6);
#endif
}

/*

#define DMAx_Streamy_IRQHandler(x,y) DMA##x##_Stream##y##_IRQHandler
#define RTUART1_DMA_IRQHANDLER DMAx_Streamy_IRQHandler(2,7)
#define RTUART2_DMA_IRQHANDLER DMAx_Streamy_IRQHandler(1,6)
#define RTUART3_DMA_IRQHANDLER DMAx_Streamy_IRQHandler(1,3)
#define RTUART4_DMA_IRQHANDLER DMAx_Streamy_IRQHandler(1,4)
#define RTUART5_DMA_IRQHANDLER DMAx_Streamy_IRQHandler(1,7)
#define RTUART6_DMA_IRQHANDLER DMAx_Streamy_IRQHandler(2,6)

void RTUART1_DMA_IRQHANDLER(void){
#if RTE_USART1 == 1
    rt_interrupt_enter();
	uart_dma_clearTxFlag(&uart1_dma);
	rt_hw_serial_dma_tx_isr(&rtuart1);
    rt_interrupt_leave();
#endif
}
void RTUART2_DMA_IRQHANDLER(void){
#if RTE_USART2 == 1
    rt_interrupt_enter();
	uart_dma_clearTxFlag(&uart2_dma);
	rt_hw_serial_dma_tx_isr(&rtuart2);
    rt_interrupt_leave();
#endif
}
void RTUART3_DMA_IRQHANDLER(void){
#if RTE_USART3 == 1
    rt_interrupt_enter();
	uart_dma_clearTxFlag(&uart3_dma);
	rt_hw_serial_dma_tx_isr(&rtuart3);
    rt_interrupt_leave();
#endif
}
void RTUART4_DMA_IRQHANDLER(void){
#if RTE_UART4 == 1
    rt_interrupt_enter();
	uart_dma_clearTxFlag(&uart4_dma);
	rt_hw_serial_dma_tx_isr(&rtuart4);
    rt_interrupt_leave();
#endif
}
void RTUART5_DMA_IRQHANDLER(void){
#if RTE_UART5 == 1
    rt_interrupt_enter();
	uart_dma_clearTxFlag(&uart5_dma);
	rt_hw_serial_dma_tx_isr(&rtuart5);
    rt_interrupt_leave();
#endif
}
void RTUART6_DMA_IRQHANDLER(void){
#if RTE_USART6 == 1
    rt_interrupt_enter();
	uart_dma_clearTxFlag(&uart6_dma);
	rt_hw_serial_dma_tx_isr(&rtuart6);
    rt_interrupt_leave();
#endif
}

*/









