#include <lcom/lcf.h>

#include "keyboard.h"
#include "timer.h"
#include "i8042.h"
#include "i8254.h"

#include <stdbool.h>
#include <stdint.h>

int byteCount = 0;
int globalCounter = 0;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (kbd_test_scan)(bool UNUSED(assembly)) {

  int ipc_status; //ind = 1;
  message msg;
  unsigned int r;
  uint8_t bit_no;
  int is_over = 0;
  //bool make = true;
  uint8_t byte;// size = 1;
  //uint8_t scancode[2];

  kb_subscribe(&bit_no);

 uint32_t irq_set = BIT(bit_no);

 // printf("%d\n", irq_set );

  while(is_over == 0) {

    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
      { 
        printf("driver_receive failed with: %d", r);
        continue;
      } 
    if (is_ipc_notify(ipc_status))
      { /* received notification */
        switch (_ENDPOINT_P(msg.m_source))
        {
          case HARDWARE: /* hardware interrupt notification */       
            if (msg.m_notify.interrupts & irq_set)
              {
                  is_over = kb_handler(&byte);
                  // scancode[ind-1] = byte;
                  // ind++;

              }
         }
     } else { /* received a standard message, not a notification */
         /* no standard messages expected: do nothing */}

    // if (ind == size)
    //   {
    //     kbd_print_scancode(make, size, scancode);
    //     make=true;
    //     ind = 1;
    //   }





 }

  kb_unsubscribe();

  kbd_print_no_sysinb(counter);

  return 0;
}

int (kbd_test_poll)() {
  kb_read_poll();
  kbc_write_cmd(); //CHANGE NAME MAYBE?

  return 0;
}
int (kbd_test_timed_scan)(uint8_t n) {
  int ipc_status;
  message msg;
  unsigned int r;
  uint8_t bit_no_timer, bit_no_kb;
  int is_over = 0;
  int seconds = 0;
  uint8_t byte;

  kb_subscribe(&bit_no_kb);
  timer_subscribe_int(&bit_no_timer);

  int freq = sys_hz();

  uint32_t irq_set_kb = BIT(bit_no_kb);
  uint32_t irq_set_timer = BIT(bit_no_timer);

  while(!is_over) {

    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
      { 
        printf("driver_receive failed with: %d", r);
        continue;
      } 
    if (is_ipc_notify(ipc_status))
      { /* received notification */
        switch (_ENDPOINT_P(msg.m_source))
        {
          case HARDWARE: /* hardware interrupt notification */       
            if (msg.m_notify.interrupts & irq_set_kb)
              {
                  is_over = kb_handler(&byte);
                  seconds = 0;
              }
            if (msg.m_notify.interrupts & irq_set_timer)
            {
              timer_int_handler();
              if(globalCounter%freq == 0){
                  ++seconds;
              }
              if (seconds == n)
              {
                is_over = 1;
              }
            }
         }
     } else { /* received a standard message, not a notification */
         /* no standard messages expected: do nothing */}


 }
  timer_unsubscribe_int();
  kb_unsubscribe();
  
  return 0;

}


