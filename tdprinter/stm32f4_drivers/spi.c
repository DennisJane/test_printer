#include "spi.h"

const static GPIO_InitTypeDef afio_att={
	0,
	GPIO_Mode_AF,
	GPIO_Speed_50MHz,
	GPIO_OType_PP,
	GPIO_PuPd_UP,
};

void spi_init(spi_t *s){
	(*s->rcc_init)(s->rcc,ENABLE);
	
	gpio_init_custom(&s->sck,afio_att);
	gpio_init_custom(&s->miso,afio_att);
	gpio_init_custom(&s->mosi,afio_att);
	uint8_t pmi=0,pmo=0,psck=0;
	for(int i=0;i<16;i++){
		if(s->sck.pin==(1<<i)){
			psck=i;
		}
		if(s->miso.pin==(1<<i)){
			pmi=i;
		}
		if(s->mosi.pin==(1<<i)){
			pmo=i;
		}
	}
	GPIO_PinAFConfig(s->sck.port ,psck,s->af);
    GPIO_PinAFConfig(s->miso.port,pmi ,s->af);
	GPIO_PinAFConfig(s->mosi.port,pmo ,s->af);
}

void spi_configure(spi_t *s,SPI_InitTypeDef *conf){
	while(s->base->SR & SPI_I2S_FLAG_BSY){__ASM("nop");}
	SPI_Cmd(s->base, DISABLE);
	SPI_Init(s->base,conf);
	SPI_Cmd(s->base, ENABLE);
}

void spi_setBaudRatePrescaler(spi_t *s,uint16_t SPI_BaudRatePrescaler){
	while(s->base->SR & SPI_I2S_FLAG_BSY){__ASM("nop");}
	s->base->CR1&=~SPI_CR1_BR;
	s->base->CR1|=SPI_BaudRatePrescaler;
}

void spi_setBaudRate(spi_t *s,uint32_t max_hz){
	uint32_t periphClock=0;
	uint16_t SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_256;
	if(s->rcc_init==RCC_APB1PeriphClockCmd){
		periphClock=RTE_PCLK1;
	}else{
		periphClock=RTE_PCLK2;
	}
	if(      max_hz >= periphClock/2){
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    }else if(max_hz >= periphClock/4){
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    }else if(max_hz >= periphClock/8){
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    }else if(max_hz >= periphClock/16){
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    }else if(max_hz >= periphClock/32){
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    }else if(max_hz >= periphClock/64){
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    }else if(max_hz >= periphClock/128){
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    }else{
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    }
	spi_setBaudRatePrescaler(s,SPI_BaudRatePrescaler);
}

uint16_t spi_swap(spi_t *s,uint16_t byte)
{
	while((s->base->SR &SPI_I2S_FLAG_TXE)==RESET);
	s->base->DR = byte;
	while((s->base->SR &SPI_I2S_FLAG_RXNE)==RESET);
	return(s->base->DR);
}

void spi_swaps8(spi_t *s,uint8_t *tx,uint8_t *rx,uint16_t size){
	if(tx && rx){
		for(int i=0;i<size;i++){
			rx[i]=spi_swap(s,tx[i]);
		}
	}else if(tx){
		for(int i=0;i<size;i++){
			spi_swap(s,tx[i]);
		}
	}else if(rx){
		for(int i=0;i<size;i++){
			rx[i]=spi_swap(s,0xFF);		//接收时TX必须要发0xFF才能正常使用SD卡???why
		}
	}
}
void spi_swaps16(spi_t *s,uint16_t *tx,uint16_t *rx,uint16_t size){
	if(tx && rx){
		for(int i=0;i<size;i++){
			rx[i]=spi_swap(s,tx[i]);
		}
	}else if(tx){
		for(int i=0;i<size;i++){
			spi_swap(s,tx[i]);
		}
	}else if(rx){
		for(int i=0;i<size;i++){
			rx[i]=spi_swap(s,0xFFFF);
		}
	}
}







