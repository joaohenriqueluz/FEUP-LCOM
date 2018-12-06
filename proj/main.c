// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include "spacedef.h"
#include "keyboard.h"
#include <stdint.h>
#include <stdio.h>
#include "i8254.h"
#include "i8042.h"
#include "vbe_macros.h"
// Any header files included below this line should have been created by you





int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (proj_main_loop)() {
  vg_init(0x144);
  Bitmap* background = loadBitmap("/home/lcom/labs/proj/imagens/spacecomdiogo.bmp");
  Bitmap* ship = loadBitmap("/home/lcom/labs/proj/imagens/blue1.bmp");
  Bitmap* shot = loadBitmap("/home/lcom/labs/proj/imagens/bola.bmp");

int ipc_status, ind = 0;
  message msg;
  unsigned int r;
  uint8_t bit_no_timer, bit_no_kb, scancode1[1], scancode2[2];
  //uint8_t seconds = 0;

  if(kb_subscribe(&bit_no_kb) != 0)
    printf("Erro na funcao kb_subscribe\n");

  if(timer_subscribe_int(&bit_no_timer) != 0)
    printf("Erro na funcao timer_subscribe_int\n");

  uint32_t irq_set_kb = BIT(bit_no_kb);
  uint32_t irq_set_timer = BIT(bit_no_timer);

  drawBitmap(background,0,0,ALIGN_LEFT);

  drawBitmap(ship, 487, 690, ALIGN_LEFT);

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
              //printf("sec = %d\n", seconds);
            }

            if (msg.m_notify.interrupts & irq_set_kb)
              {
                  globalCounter = 0;

                  kbd_read();
                  move_ship(ship,  background);

                  if (size == 2)
                  {
                    scancode2[ind] = byte;
                  }
                  else{
                    scancode1[ind] = byte;
                  }
                  ind++;
              }
            
         }
     } else { /* received a standard message, not a notification */
         /* no standard messages expected: do nothing */}

     if (ind >= size)
      {
        make = true;
        size = 1;
        ind = 0;
      }

      if (globalCounter % (sys_hz() / 20) == 0)
		{
 		 shoot( ship, background, shot);
		}

 
  
    //drawBitmap(ship,100,100,ALIGN_LEFT);

      
 }
  if(timer_unsubscribe_int() != 0)
    printf("Erro na funcao timer_unsubscribe_int\n");
  if(kb_unsubscribe() != 0)
    printf("Erro na funcao kb_unsubscribe\n");

  vg_exit();

  return 0;
}
