#include "libTrigger.h"


void trigger_construct(trigger_t *t,gpio_t gpio,bool inv,bool fake){
	t->trigger=gpio;
	t->inv_trigger=inv;
	t->fakeOutput=fake;
}

void trigger_init(trigger_t *t){
	gpio_init(&t->trigger,GPIO_Mode_IN);
}

