//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
#pragma once

#define UARTx(x) uart##x
//<o> stdout uartx <1=>uart1  <2=>uart2  <3=>uart3  <4=>uart4  <5=>uart5  <6=>uart6  <7=>uart7  <8=>uart8 
#define STDOUT_UART UARTx(3)

//<o> stdout baudrate <1-99999999>
#define STDOUT_BAUD	115200

// <o> Finsh Console on USART: <0=>USART 1 <1=>USART 2 <2=> USART 3 <3=> USART 4
#define FINSH_DEVICE_ID		2

/* whether use board external SRAM memory */
// <e>Use external SRAM memory on the board
// 	<i>Enable External SRAM memory
#define STM32_EXT_SRAM          0
//	<o>Begin Address of External SRAM
//		<i>Default: 0x68000000
#define STM32_EXT_SRAM_BEGIN    0x68000000 /* the begining address of external SRAM */
//	<o>End Address of External SRAM
//		<i>Default: 0x68080000
#define STM32_EXT_SRAM_END      0x68080000 /* the end address of external SRAM */
// </e>







