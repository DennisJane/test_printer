#include "bsp.h"
#include "sddev.h"
#include <rtthread.h>

static struct rt_spi_device sddev;
#ifdef TDPRINTER_BRAIN
static gpio_t cs={GPIOA,GPIO_Pin_4,1};
#else
static gpio_t cs={GPIOA,GPIO_Pin_15,1};
#endif
static struct rt_spi_configuration config={
	RT_SPI_MODE_0 | RT_SPI_MSB,
	8,
	0,
	50000,	//sd spi init 50k
};

void sddev_init(char *spi_bus){
	rt_spi_device_init(&sddev,&cs,"sddev",spi_bus);
	rt_spi_configure(&sddev,&config);
}




