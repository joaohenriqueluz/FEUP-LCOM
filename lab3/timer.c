#include <lcom/lcf.h>

#include <stdint.h>

#include "i8254.h"
#include "timer.h"

extern int globalCounter;
int globalHookId;

int (timer_subscribe_int)(uint8_t *bit_no) {

  int temp_hook = TEMP_HOOK;
  
  sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &temp_hook);

  globalHookId = temp_hook;

 *bit_no = TEMP_HOOK;

  return 0;
}

int (timer_unsubscribe_int)() {
  
  sys_irqrmpolicy(&globalHookId);

  return 0;
}

void (timer_int_handler)() {
  ++globalCounter;
}
