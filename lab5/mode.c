#include <lcom/lcf.h>
#include <machine/int86.h> // /usr/src/include/arch/i386
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "i8254.h"
#include "vbe_macros.h"
#include "mode.h"

int globalCounter = 0;

int vg_enter(uint16_t mode){
	struct reg86u reg86;

	memset(&reg86, 0, sizeof(reg86));	/* zero the structure */

	reg86.u.w.ax = mode;
	reg86.u.w.bx = LINEAR;
	reg86.u.b.intno = INIT;

	if(sys_int86(&reg86) != OK) {
  		printf("vg_enter(): sys_int86() failed \n");
  		return 1;
  	}
  	return 0;
}

int wait(uint8_t time){
	int ipc_status;
	message msg;
	unsigned int r;
	uint8_t bit_no;
	globalCounter = 0;

	if(timer_subscribe_int(&bit_no) != 0){
		printf("Erro na função timer_subscribe_int \n");
		return 1;
	}

	int freq = sys_hz();

	uint32_t irq_set = BIT(bit_no);

	while(globalCounter/freq < time) {

     if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
          printf("driver_receive failed with: %d", r);
         continue;
     } 
     if (is_ipc_notify(ipc_status)) { /* received notification */
         switch (_ENDPOINT_P(msg.m_source)) {
             case HARDWARE: /* hardware interrupt notification */       
                 if (msg.m_notify.interrupts & irq_set) {
                    timer_int_handler();
                  }
                 break;
              default:
                  break; /* no other notifications expected: do nothing */ 
         }
     } else { /* received a standard message, not a notification */
         /* no standard messages expected: do nothing */
     }
 }

  if(timer_unsubscribe_int() != 0){
  	printf("Erro na função timer_unsubscribe_int \n");
  	return 1;
  }

  return 0;
}

