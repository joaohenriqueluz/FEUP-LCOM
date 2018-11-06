#include <lcom/lcf.h>

#include <stdint.h>

#include "i8042.h"
#include "mouse.h"

int globalHookId;
extern uint8_t byte;
extern int byteCounter;

int (mouse_subscribe)(uint8_t *bit_no){
	int temp_hook = TEMP_HOOK_MOUSE;

  	if(sys_irqsetpolicy(MOUSE_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &temp_hook) != OK)
      return 1;

  	globalHookId = temp_hook;
 
 	*bit_no = TEMP_HOOK_MOUSE;

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

uint8_t (mouse_scan_byte_remote)(uint16_t period){

	uint32_t stat, data;
	while( 1 ){
	 sys_inb(KB_STATUS_REG, &stat); /*cd2 assuming it returns OK */
        /* loop while 8042 output buffer is empty */
        if( stat & OBF ) 
        	{/* assuming it returns OK */
        		sys_inb(OUT_BUF, &data);
            	if ( ((stat &(PAR_ERR | TO_ERR| AUX)) >> 5) == 1 )
                	return data;
            	else
                	return -1;
        	}
        tickdelay(micros_to_ticks(period));
	}
}

void (mouse_ih)(){
	byte = mouse_scan_byte();
	if (byte & BIT(3))
	{
		byteCounter = 0;
	}
}


void (mouse_remote)(uint16_t period){
	byte = mouse_scan_byte_remote(period);
	if (byte & BIT(3))
	{
		byteCounter = 0;
	}
}

int write_comand_mouse(){
	uint32_t stat;

	while(1){
		sys_inb(KB_STATUS_REG, &stat);
		if ((stat & IBF) == 0)
		{
			sys_outb(KBC_CM_REG,WRITE_TO_MOUSE);
			return 0;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

int mouse_enable_stream(){
	uint32_t stat, data;
	
	while(1){
		sys_inb(KB_STATUS_REG, &stat);
		if ((stat & IBF) == 0)
		{
			sys_inb(OUT_BUF, &data);
			
			if (data == ACK)
			{
				sys_outb(OUT_BUF,STREAM_MODE);
				return 0;
			}
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
	return 1;
}


int mouse_default(){
	uint32_t stat, data;
	
	while(1){
		sys_inb(KB_STATUS_REG, &stat);
		if ((stat & IBF) == 0)
		{
			sys_inb(OUT_BUF, &data);
			
			if (data == ACK)
			{
				sys_outb(OUT_BUF,MOUSE_DEFAULT);
				return 0;
			}
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
	return 1;
}

int mouse_reset(){
	uint32_t stat, data;
	
	while(1){
		sys_inb(KB_STATUS_REG, &stat);
		if ((stat & IBF) == 0)
		{
			sys_inb(OUT_BUF, &data);
			
			if (data == ACK)
			{
				sys_outb(OUT_BUF,MOUSE_RESET);
				return 0;
			}
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
	return 1;
}

int mouse_enable_remote(){
	uint32_t stat, data;
	
	while(1){
		sys_inb(KB_STATUS_REG, &stat);
		if ((stat & IBF) == 0)
		{
			sys_inb(OUT_BUF, &data);
			
			if (data == ACK)
			{
				sys_outb(OUT_BUF,STREAM_MODE);
				return 0;
			}
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
	return 1;
}

void disable_mouse (){
	sys_outb(KB_STATUS_REG,WRITE_TO_MOUSE);
	sys_outb(KB_STATUS_REG, DISABLE_MOUSE);
}





