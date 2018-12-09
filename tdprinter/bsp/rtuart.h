/*
 * File      : usart.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

//垃圾的RTTHREAD,只支持DMA_TX 和 INT_RX
#ifndef __USART_H__
#define __USART_H__

#include <rtthread.h>
#include "serial.h"
#include "bsp/bsp.h"

void rt_uart_init(rt_serial_t *rt_uart,char *name,uart_dma_t *uart_dma);
void stm32_rtuart_handler(rt_serial_t *rtuart);

#endif
