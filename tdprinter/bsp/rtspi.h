/*
 * File      : stm32f20x_40x_spi.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 20012-01-01    aozima       first implementation.
 */

#ifndef STM32F20X_40X_SPI_H_INCLUDED
#define STM32F20X_40X_SPI_H_INCLUDED

#include <stdint.h>
#include <rtthread.h>
#include <drivers/spi.h>
#include "stm32f4_drivers/drivers.h"

/* public function */
void rt_spi_bus_init(struct rt_spi_bus *bus,spi_t *spi,const char * bus_name);
void rt_spi_device_init(struct rt_spi_device *device,gpio_t *cs,const char *device_name,const char *bus_name);






#endif // STM32F20X_40X_SPI_H_INCLUDED
