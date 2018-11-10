#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"
#include "mouse.h"

int globalCounter = 0;
int byteCounter = 0;
uint8_t byte, packet[3];
uint32_t count = 0;
bool gest=false, terminate = false;
uint8_t global_x_len, global_tolerence;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status;
  message msg;
  unsigned int r;
  uint8_t bit_no;


 enable_cmd_int();

  if(mouse_subscribe(&bit_no) != 0)
    printf("Erro na funcao mouse_subscribe\n");

 uint32_t irq_set = BIT(bit_no);

  while(count < cnt) {

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
                 mouse_ih();
              }
         }
     }
 }

 

  if(mouse_unsubscribe() != 0)
    printf("Erro na funcao mouse_unsubscribe\n");

 disable_cmd_int();

  return 0;
}


int (mouse_test_remote)(uint16_t period, uint8_t cnt) {

while(count < cnt)
  {
    mouse_remote();
    

  tickdelay(micros_to_ticks(period*1000));
  }

  disable_cmd_remote();
  kbc_write_cmd();

return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status;
  message msg;
  unsigned int r;
  bool is_over = false;
  uint8_t bit_no_mouse, bit_no_timer;
  int freq = sys_hz();


 enable_cmd_int();

  if(mouse_subscribe(&bit_no_mouse) != 0)
    printf("Erro na funcao mouse_subscribe\n");

  if(timer_subscribe_int(&bit_no_timer) != 0)
    printf("Erro na funcao timer_subscribe_int\n");

  uint32_t irq_set_mouse = BIT(bit_no_mouse);
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

            if (msg.m_notify.interrupts & irq_set_timer)
            {
              timer_int_handler();
              if(globalCounter >= (idle_time*freq))
              {
                is_over = true;
              }
            }

            if (msg.m_notify.interrupts & irq_set_mouse)
              {
                globalCounter = 0;
                mouse_ih();
              }
         }
     } 
 }

//disable_mouse();

  if(timer_unsubscribe_int() != 0)
    printf("Erro na funcao timer_unsubscribe_int\n");

  if(mouse_unsubscribe() != 0)
    printf("Erro na funcao mouse_unsubscribe\n");

  disable_cmd_int();

  return 0;
}


int (mouse_test_gesture)( uint8_t x_len, uint8_t tolerance) {
  int ipc_status;
   message msg;
   unsigned int r;
   uint8_t bit_no;
  global_x_len=x_len;
  global_tolerence=tolerance;

 enable_cmd_int();

  if(mouse_subscribe(&bit_no) != 0)
    printf("Erro na funcao mouse_subscribe\n");

 uint32_t irq_set = BIT(bit_no);

  while(!terminate) {
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
                gest = true;
                 mouse_ih();
              }
         }
     }
 }

 

  if(mouse_unsubscribe() != 0)
    printf("Erro na funcao mouse_unsubscribe\n");

 disable_cmd_int();
return 0;
 }
