#include <lcom/lcf.h>

#include <stdint.h>

#include "i8042.h"
#include "mouse.h"

int globalHookId;
extern uint8_t byte;
extern int byteCounter;

int (mouse_subscribe)(uint8_t *bit_no){
	int temp_hook = TEMP_HOOK_MOUSE;

  	if(sys_irqsetpolicy(KB_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &temp_hook) != OK)
      return 1;

  	globalHookId = temp_hook;
 
 	*bit_no = TEMP_HOOK_KB;

	return 0;
}

int (mouse_unsubscribe)(){
  
  if(sys_irqrmpolicy(&globalHookId) != OK)
    return 1;

  return 0;
}

uint8_t (mouse_scan_byte)(){

	uint32_t stat, data;
	while( 1 ){
	 sys_inb(KB_STATUS_REG, &stat); /*cd2 assuming it returns OK */
        /* loop while 8042 output buffer is empty */
        if( stat & OBF ) 
        	{/* assuming it returns OK */
        		sys_inb(OUT_BUF, &data);
            	if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
                	return data;
            	else
                	return -1;
        	}
        tickdelay(micros_to_ticks(DELAY_US));
	}
}

void (mouse_ih)(){
	byte = mouse_scan_byte();
	if (byte & 0x80)
	{
		byteCounter = 0;
	}
}

int mouse_enable_stream(){
	uint32_t stat;
	
	while(1){
		sys_inb(KB_STATUS_REG, &stat);
		if (stat & OBF)
		{
			sys_outb(KBC_CM_REG,WRITE_TO_MOUSE);
			sys_inb(OUT_BUF, &stat);
			
			if (stat == ACK)
			{
				sys_outb(KBC_CM_REG,STREAM_MODE);
				return 0;
			}
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
	return 1;
}






