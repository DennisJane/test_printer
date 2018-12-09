#pragma once

//#define TDPRINTER_A
#define TDPRINTER_B
//#define TDPRINTER_BRAIN

//1<<4=16
#define MOTION_BUFFER_SIZE 16

#define THREAD_PRINTER_STACK_SIZE 512
#define THREAD_PRINTER_PRIORITY	  20
#define THREAD_GCODE_STACK_SIZE 1025
#define THREAD_GCODE_PRIORITY 20
#define THREAD_RETARGET_STACK_SIZE 512
#define THREAD_RETARGET_PRIO 20
#define THREAD_CORE_PRINTER_STACK_SIZE 1024
#define THREAD_CORE_PRINTER_PRIORITY 20
#define THREAD_HMI_STACK_SIZE 1024
#define THREAD_HMI_PRIORITY 20

#define trace(x)

#if defined(TDPRINTER_A)
	#define STEPPER_NUM 6
#elif defined(TDPRINTER_B)
	#define STEPPER_NUM 4
#elif defined(TDPRINTER_BRAIN)
	#define STEPPER_NUM 4
#endif

#if defined(TDPRINTER_A)
#define TD_VERSION "TDPRINTER_A v2.0\r\n"
#define HMI_UART "rtuart1"
#define DEFAULT_TEMP 220

#elif defined(TDPRINTER_B)
#define TD_VERSION "TDPRINTER_B v2.0\r\n"
#define HMI_UART "rtuart6"
#define SB_UART  "rtuart1"
#define DEFAULT_TEMP 220
#define MICROSTEP 16

#elif defined(TDPRINTER_BRAIN)
#define TD_VERSION "TDPRINTER_BRAIN v1.0\r\n"
#define HMI_UART "rtuart1"
#define DEFAULT_TEMP 150

#endif



