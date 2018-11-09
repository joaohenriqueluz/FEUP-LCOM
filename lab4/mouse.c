#include <lcom/lcf.h>

#include <stdint.h>

#include "i8042.h"
#include "mouse.h"

int globalHookId;
extern uint8_t byte, packet[3];
extern int byteCounter;
extern uint32_t count;

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
	printf("check read\n");
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

	packet[byteCounter] = byte;

	if ((packet[0] & BIT(3)) == 0)
	{
		byteCounter = 0;
	}
	else{
		if (byteCounter >= 2) printPacket();
		else{
			byteCounter++;
		}
	}
}


void (mouse_remote)(uint16_t period){
	byte = mouse_scan_byte_remote(period);
	if (byte & BIT(3))
	{
		byteCounter = 0;
	}
}

int fst_command_mouse(){
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

int write_command(uint32_t cmd){
	uint32_t stat;

	while(1){
		sys_inb(KB_STATUS_REG, &stat);
		if ((stat & IBF) == 0)
		{
			sys_outb(OUT_BUF,cmd);
			return 0;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

int check_command(uint32_t cmd){
	uint32_t byte;
	write_command(cmd);
	byte = mouse_scan_byte();
	return 0;
}

void printPacket(){
	struct packet *pp = malloc(sizeof(struct packet));
	for (int i = 0; i < 3; ++i)
	{
		pp->bytes[i] = packet[i];
	}
	pp->rb = (pp->bytes[0] & BIT(1)) >> 1;
     pp->mb = (pp->bytes[0] & BIT(2)) >> 2;
      pp->lb = (pp->bytes[0] & BIT(0));
      pp->x_ov = (pp->bytes[0] & BIT(6)) >> 6;
      pp->y_ov = (pp->bytes[0] & BIT(7)) >> 7;
      
      if(((packet[0] & BIT(4)) >> 4)==0)
        pp->delta_x = (packet[1] & 0x00ff);
      else
         pp->delta_x = (packet[1] | 0xff00);

      if(((packet[0] & BIT(5)) >> 5)==0)
         pp->delta_y = (packet[2] & 0x00ff);
      else
        pp->delta_y = (packet[2] | 0xff00);

      mouse_print_packet(pp);
      clearPacket();
      byteCounter = 0;
      count++;
}

void clearPacket(){
	for (int i = 0; i < 3; ++i)
	{
		packet[i] = 0;
	}
}

void disable_mouse (){
	sys_outb(KB_STATUS_REG,WRITE_TO_MOUSE);
	sys_outb(KB_STATUS_REG, DISABLE_MOUSE);
}





