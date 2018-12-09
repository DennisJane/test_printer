#include "bsp.h"
#include "max6675.h"
#include <rtthread.h>

static struct rt_spi_device max6675;
static gpio_t cs={GPIOA,GPIO_Pin_15,1};
static struct rt_spi_configuration config={
	RT_SPI_MODE_1 | RT_SPI_MSB,
	16,
	0,
	2000000,	//max6675 max clk =2M
};

void max6675_init(char *spi_bus){
	rt_spi_device_init(&max6675,&cs,"max6675",spi_bus);
	rt_spi_configure(&max6675,&config);
}
















