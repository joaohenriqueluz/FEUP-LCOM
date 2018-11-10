#include <lcom/lcf.h>

#include <stdint.h>

#include "i8042.h"
#include "mouse.h"

int globalHookId;
extern uint8_t byte, packet[3];
extern int byteCounter;
extern uint32_t count;
extern bool gest;
struct mouse_ev gesture;
uint8_t Xdelta=0;
struct packet *pp = malloc(sizeof(struct packet));
struct packet *old_pp=malloc(sizeof(struct packet));



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
        sys_inb(OUT_BUF, &data);
        if( stat & OBF ) 
        	{/* assuming it returns OK */		
            	if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
                	return data;
            	else
                	return -1;
        	}
      //  tickdelay(micros_to_ticks(DELAY_US));
	}
}

uint8_t (mouse_scan_byte_remote)(){

	uint32_t stat, data;
	
	while( 1 ){
	 sys_inb(KB_STATUS_REG, &stat); 
        if( stat & OBF ) 
        	{/* assuming it returns OK */
        		sys_inb(OUT_BUF, &data);
            	if ( (stat &(PAR_ERR | TO_ERR| AUX)>> 5) == 1 )
                	return data;
            	else
                	return -1;
        	}
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


void (mouse_remote)(){

	write_command(READ_DATA);

	for(unsigned int i=0;i<3;)
	{
		byte = mouse_scan_byte();
	
		packet[byteCounter] = byte;
	
		if ((packet[0] & BIT(3)) == 0)
		{
			byteCounter = 0;
			i=0;
			continue;
		}
		
		byteCounter++;
		i++;
	}

	printPacket();
}



void write_command(uint32_t cmd)
{
	uint32_t stat, data=0;

	while(data != ACK){
		sys_outb(KB_STATUS_REG, WRITE_TO_MOUSE);
		sys_inb(KB_STATUS_REG, &stat);
		if ((stat & IBF) == 0)
		{
			sys_outb(OUT_BUF,cmd);
			sys_inb(OUT_BUF,&data);
			return;
		}

		//tickdelay(micros_to_ticks(DELAY_US));
	}
}

void enable_cmd_int()
{
	//write_command(WRITE_TO_MOUSE);
	write_command(STREAM_MODE);
	//write_command(WRITE_TO_MOUSE);
	write_command(ENABLE_DR);

}

void disable_cmd_int()
{
	//write_command(WRITE_TO_MOUSE);
	write_command(DISABLE_MOUSE);
}

// int check_command(uint32_t cmd){
// 	uint32_t byte;
// 	write_command(cmd);
// 	byte = mouse_scan_byte();
// 	return 0;
// }
void disable_cmd_remote()
{
	write_command(STREAM_MODE);
	write_command(DISABLE_MOUSE);

}

void printPacket(){
	
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
      if(gest)
      {
      old_pp = pp;
      set_mouse_events();
      void check_line(gesture); 

      }

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


// void disable_mouse (){
// 	uint32_t status;
// 	sys_inb(KB_STATUS_REG,&status);
// 	sys_outb(KB_STATUS_REG,WRITE_TO_MOUSE);
// 	sys_inb(KB_STATUS_REG,&status);
// 	sys_outb(KB_STATUS_REG, DISABLE_MOUSE);
// }


int (kbc_write_cmd)()
{
  uint32_t cmd=0;
  sys_outb(KB_STATUS_REG,KBC_READ);
  sys_inb(OUT_BUF,&cmd);

  cmd = cmd| KB_ENABLE;

sys_outb(KB_STATUS_REG,WRITE_BYTE);

sys_outb(OUT_BUF,minix_get_dflt_kbc_cmd_byte());

  return 0;
}
typedef enum {INIT, LUP, PAUSE, RDOWN , COMP} state_t;

void check_line(struct mouse_ev evt) 
{
	static state_t state = INIT; // initial state; keep state

	switch (state) 
	{
	case INIT:
		if( evt.type == LB_PRESSED)
		{
			state = LUP;
			break;
		}
	case LUP:
		if( evt.type == LB_RELEASED ) 
		{
				state = PAUSE;
			break;
		} 

	case PAUSE:
	
		if( evt.type == RB_PRESSED) 
		{
			state = RDOWN;
			
		}
		else if( evt.type == LB_PRESSED) 
		{
				state = LUP;
		}
		else
		{
			state =INIT;
		}

		break;
		
	case RDOWN:
	
		if( evt.type == RB_RELEASED) 
		{
			state = COMP;
		}
		else if(evt.type == LB_PRESSED)
		{
			state = LUP;
		}
		else
		{
			state =INIT;
		}
		break;
	
	default:
		break;
	}	
}

void set_mouse_events()
{
	if(pp->lb == 1) //left button pressed
		gesture.type = LB_PRESSED;

	else if(pp->rb == 1) // right button pressed
		gesture.type = RB_PRESSED;

	else if(pp->lb == 0 && pp->rb == 0 && ((pp->delta_y != 0) || (pp->delta_x != 0))) // no button pressed but mouse is moving
		gesture.type = MOUSE_MOV;
	else if(pp->lb == 0 && old_pp->lb == 1)
		gesture.type = LB_RELEASED;
	else if(pp->rb == 0 && old_pp->rb == 1)
		gesture.type = RB_RELEASED;
	else
		gesture.type = BUTTON_EV;
}
