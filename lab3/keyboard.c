#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8042.h"
#include "keyboard.h"

extern int byteCount;

int globalHookId;

int (kb_subscribe)(uint8_t *bit_no){
	int temp_hook = TEMP_HOOK;
  
  	sys_irqsetpolicy(KB_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &temp_hook);

  	globalHookId = temp_hook;

 	*bit_no = TEMP_HOOK;

	return 0;
}

int (kb_unsubscribe)(){
  
  sys_irqrmpolicy(&globalHookId);

  return 0;
}

uint8_t (kb_scan_byte)(){
	uint32_t stat, data;
	while( 1 ){
	 sys_inb(KB_STATUS_REG, &stat); /* assuming it returns OK */
        /* loop while 8042 output buffer is empty */
        if( stat & OBF ) {
            sys_inb(OUT_BUF, &data); /* assuming it returns OK */
            if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
                return data;
            else
                return -1;
        }
        tickdelay(micros_to_ticks(DELAY_US));
}







