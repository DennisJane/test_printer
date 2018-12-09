/*
 * File      : stm32f20x_40x_spi.c
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
 * 20012-07-27    aozima       fixed variable uninitialized.
 */

#include "rtspi.h"

static rt_err_t configure(struct rt_spi_device* device,
                          struct rt_spi_configuration* configuration)
{
	spi_t *spi=device->bus->parent.user_data;
    SPI_InitTypeDef SPI_InitStructure;
    SPI_StructInit(&SPI_InitStructure);
	
    if(configuration->data_width <= 8){
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    }else if(configuration->data_width <= 16){
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    }else{
        return RT_EIO;
    }

    if(configuration->mode & RT_SPI_CPOL){
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    }else{
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    }
    if(configuration->mode & RT_SPI_CPHA){
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    }else{
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    }
    if(configuration->mode & RT_SPI_MSB){
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    }else{
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    }
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS  = SPI_NSS_Soft;

	spi_configure(spi,&SPI_InitStructure);
	spi_setBaudRate(spi,configuration->max_hz);
    return RT_EOK;
};

static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message)
{
	gpio_t *cs=device->parent.user_data;
    spi_t *spi=device->bus->parent.user_data;

    if(message->cs_take){
		gpio_set(cs,false);
		__ASM("nop");__ASM("nop");__ASM("nop");
		__ASM("nop");__ASM("nop");__ASM("nop");
    }
	if(device->config.data_width<=8){
		spi_swaps8(spi,(uint8_t*)message->send_buf,(uint8_t*)message->recv_buf,message->length);
	}else{
		spi_swaps16(spi,(uint16_t*)message->send_buf,(uint16_t*)message->recv_buf,message->length);
	}
    if(message->cs_release){
        gpio_set(cs,true);
		__ASM("nop");__ASM("nop");__ASM("nop");
		__ASM("nop");__ASM("nop");__ASM("nop");
    }
    return message->length;
};
static const struct rt_spi_ops stm32_spi_ops ={configure,xfer};

void rt_spi_bus_init(struct rt_spi_bus *bus,spi_t *spi,const char * bus_name){
	bus->parent.user_data=spi;
	rt_spi_bus_register(bus,bus_name,&stm32_spi_ops);
}
void rt_spi_device_init(struct rt_spi_device *device,gpio_t *cs,const char *device_name,const char *bus_name){
	gpio_init(cs,GPIO_Mode_OUT);
	gpio_set(cs,true);
	rt_spi_bus_attach_device(device,device_name,bus_name,cs);
}



