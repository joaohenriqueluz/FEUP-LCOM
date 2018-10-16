#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8042.h"

int globalHookId;

int (kb_subscribe)(uint8_t *bit_no){
	int temp_hook = TEMP_HOOK;
  
  	sys_irqsetpolicy(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &temp_hook);

  	globalHookId = temp_hook;

 	*bit_no = TEMP_HOOK;

	return 0;
}

int (kb_unsubscribe)(){
  
  sys_irqrmpolicy(&globalHookId);

  return 0;
}