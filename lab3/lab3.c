#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

int byteCount = 0;

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

  int ipc_status;
  message msg;
  unsigned int r;
  uint8_t bit_no;
  uint8_t size=1;
  bool is_over = false;
  uint8_t byte;
  uint8_t fst_byte;
  uint8_t sd_byte;
  bool make = true;

  kb_subscribe(&bit_no);

 uint32_t irq_set = BIT(bit_no);

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
            if (msg.m_notify.interrupts & irq_set)
              {
                  byte = kb_scan_byte();

                    switch(byte)
                    {
                      case ESC_BREAK:
                      {
                        is_over= true;

                        continue;
                      }
                      case TWO_BYTE_SCAN:
                      {
                        size=2;
                        fst_byte= byte;
                        break;
                      }
                      default:
                      {
                        if(byte > 0x7f)
                          make=false;

                        if(size == 2)
                        {
                          sd_byte= byte;

                          uint8_t *scancode[size];
                          scancode[0]=fst_byte;
                          scancode[1]=sd_byte;
                         continue;
                        }
                        else
                        {
                          fst_byte = byte;
                          uint8_t *scancode[size];
                          scancode[0]=fst_byte;
                          continue;
                        }
                      }
                    }

                }
         }
     } else { /* received a standard message, not a notification */
         /* no standard messages expected: do nothing */}
  
 

  kbd_print_scancode(make, size, scancode);

 }

  kb_unsubscribe();

  return 0;
}
int (kbd_test_poll)() {
    /* To be completed */
}
int (kbd_test_timed_scan)(uint8_t UNUSED(n)) {
    /* To be completed */
    /* When you use argument n for the first time, delete the UNUSED macro */
}