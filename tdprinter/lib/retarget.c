#include "stm32f4_drivers/drivers.h"
#include "bsp/bsp.h"
#include <stdio.h>

#pragma import(__use_no_semihosting_swi)

//to define @ bsp.h
#ifndef STDOUT_UART
#error "STDOUT_UART not defined"
#define STDOUT_UART uart1
#endif

struct __FILE{
	int id;
	uart_t *uart;
};

FILE __stdout={0,(uart_t*)&STDOUT_UART};
FILE __stdin={1,0};
FILE __stderr={2,0};

int globalStdoutCounter=0;

int fputc(int c, FILE *f){
	if(f->uart!=0){
		uart_putc(f->uart,c);
		globalStdoutCounter++;
	}
	return c;
}
int fgetc(FILE *f){
	int c;
	if(f->uart!=0){
		c=uart_getc(f->uart);
	}
	return c;
}

void assert_failed(uint8_t *filename,int line,uint8_t *funcName){
	printf("assert_failed:%s:%d:%s\r\n",filename,line,funcName);
	while(1){
		__ASM("nop");
	}
}
void _sys_exit(int return_code){
	while(1){
		printf("_sys_exit@%s,%d",__FILE__,__LINE__);
	}
}


