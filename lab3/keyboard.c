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
        if( stat & OBF ) 
        	{
            sys_inb(OUT_BUF, &data); /* assuming it returns OK */
            if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
                return data;
            else
                return -1;
        	}
        tickdelay(micros_to_ticks(DELAY_US));
		}

}

// int (kbc_write_cmd)(uint8_t cmd){
//   uint32_t stat;

//   while( 1 ) {
//     sys_inb(STAT_REG, &stat); /* assuming it returns OK */
//      loop while 8042 input buffer is not empty 
//     if( (stat & IBF) == 0 ) {
//         sys_outb(KBC_CMD_REG, cmd); /* no args command */
//         return 0; 
//       }
//     tickdelay(micros_to_ticks(DELAY_US));
//   }
//   return 0;
// }

int (kb_handler)(uint8_t *byte){
  *byte = kb_scan_byte();
  bool make = true;
  uint8_t tam;

  if (*byte == ESC_BREAK)
  {
      make = false;
      uint8_t scancode[1];
      scancode [0] = *byte;
      tam = 1;
      //*size = 1;
      kbd_print_scancode(make, tam, scancode);
      return 1;
  }
 if (*byte == TWO_BYTE_SCAN)
 {
    uint8_t scancode[2];
    scancode [0] = *byte;
    scancode[1]  = kb_scan_byte();
    tam = 2;
    kbd_print_scancode(make, tam, scancode);
    return 0;
  }
  else{
  if (*byte & 0x80)
  {
      make = false;
      uint8_t scancode[1];
      scancode [0] = *byte;
      tam = 1;
      //*size = 1;
      kbd_print_scancode(make, tam, scancode);
      return 0;
   }
   else{
    uint8_t scancode[1];
    scancode[0] = *byte;
    tam = 1;
    kbd_print_scancode(make, tam, scancode);
   }
 }
   return 0;
}


int (kb_read_poll)(){
  uint8_t byte, tam;
  uint32_t stat;
  bool make = true;

  printf("Teste\n\n");

  while(make)
    sys_inb(KB_STATUS_REG, &stat);
    printf("OBF %d\n", stat & OBF);
    printf("AUX %d\n\n", stat & AUX);
    if (make )//(stat & OBF) /*&& !(stat & AUX)*/)

    {
      byte = kb_scan_byte();
      if (byte == ESC_BREAK)
      {
        make = false;
        uint8_t scancode[1];
        scancode [0] = byte;
        tam = 1;
        //*size = 1;
        kbd_print_scancode(make, tam, scancode);
        return 1;
      }
      if (byte == TWO_BYTE_SCAN)
      {
       uint8_t scancode[2];
       scancode [0] = byte;
       scancode[1]  = kb_scan_byte();
       tam = 2;
       kbd_print_scancode(make, tam, scancode);
       return 0;
      }
      else{
        uint8_t scancode[1];
        scancode[0] = byte;
        tam = 1;
        kbd_print_scancode(make, tam, scancode);
      }
    }
    return 0;
}


