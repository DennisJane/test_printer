#pragma once

#include "stm32f4_drivers/drivers.h"
#include <stdbool.h>

typedef struct{
	gpio_t trigger;
	bool inv_trigger;
	bool fakeOutput;
}trigger_t;

void trigger_construct(trigger_t *t,gpio_t gpio,bool inv,bool fake);
void trigger_init(trigger_t *t);

//ret true=pressed,false=released
__STATIC_INLINE bool trigger_read(trigger_t *t){
	if(t->fakeOutput){
		return false;
	}else{
		return gpio_get(&t->trigger)^t->inv_trigger;
	}
}



