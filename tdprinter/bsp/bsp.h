#pragma once

#include "stm32f4_drivers/drivers.h"
#include "bsp_config.h"
#include "rtuart.h"
#include "rtspi.h"
#include "max6675.h"
#include "sddev.h"
#include "msd.h"
#include "usbmsc.h"

void bsp_core_init(void);
void bsp_init(void);

extern const char *RTUART_NAME[];
#define FINSH_DEVICE RTUART_NAME[FINSH_DEVICE_ID]




