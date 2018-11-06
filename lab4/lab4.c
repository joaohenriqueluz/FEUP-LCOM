#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"
#include "mouse.h"

int byteCounter = 0;
uint8_t byte;

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
  uint8_t bit_no, count = 0, packet[3];
  struct packet *pp = malloc(sizeof(struct packet));

  if(mouse_subscribe(&bit_no) != 0)
    printf("Erro na funcao mouse_subscribe\n");

  if(write_comand_mouse() != 0){
    printf("Problema ao enviar o 1º comando\n");
  }
  if(mouse_enable_stream() != 0){
    printf("Problema ao enviar o comando ENABLE STREAM MODE\n");
  }

  //mouse_enable_data_reporting();  // tem de ser feita por nós para ganhar pontos

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
                packet[byteCounter++] = byte;
              }
         }
     } else { /* received a standard message, not a notification */
         /* no standard messages expected: do nothing */}

    if (byteCounter >= 2)
    {
      for (int i = 0; i < 3; i++)
      {
        pp->bytes[i] = packet[i];
      }
      pp->rb = (pp->bytes[0] & BIT(1) >> 1);
      pp->mb = (pp->bytes[0] & BIT(2) >> 2);
      pp->lb = (pp->bytes[0] & BIT(0));
      pp->x_ov = (pp->bytes[0] & BIT(6) >> 6);
      pp->y_ov = (pp->bytes[0] & BIT(7) >> 7);
      pp->delta_x = pp->bytes[1] | (pp->bytes[0] & BIT(4) << 12);
      pp->delta_y = pp->bytes[2] | (pp->bytes[0] & BIT(5) << 11);

      mouse_print_packet(pp);
      byteCounter = 0;
      count++;
    }
 }

disable_mouse();


  if(mouse_unsubscribe() != 0)
    printf("Erro na funcao kb_unsubscribe\n");

  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
 
  struct packet *pp = malloc(sizeof(struct packet));
  uint8_t count = 0, packet[3];


if(write_comand_mouse() != 0)
    {
      printf("Problema ao enviar o 1º comando\n");
    }


disable_mouse();     

if(mouse_enable_remote() != 0)
    {
      printf("Problema ao enviar o comando ENABLE REMOTE MODE\n");
    }

while(count < cnt)
  {
    mouse_remote(period);
    packet[byteCounter++]= byte;
    if (byteCounter >= 2)
    {
      for (int i = 0; i < 3; i++)
      {
        pp->bytes[i] = packet[i];
      }
      pp->rb = (pp->bytes[0] & BIT(1) >> 1);
      pp->mb = (pp->bytes[0] & BIT(2) >> 2);
      pp->lb = (pp->bytes[0] & BIT(0));
      pp->x_ov = (pp->bytes[0] & BIT(6) >> 6);
      pp->y_ov = (pp->bytes[0] & BIT(7) >> 7);
      pp->delta_x = pp->bytes[1] | (pp->bytes[0] & BIT(4) << 12);
      pp->delta_y = pp->bytes[2] | (pp->bytes[0] & BIT(5) << 11);

      mouse_print_packet(pp);
      byteCounter = 0;
      count++;
    }
  }
uint8_t default_cmd= minix_get_dflt_kbc_cmd_byte();
  sys_outb(KB_STATUS_REG,WRITE_TO_MOUSE);
  sys_outb(KB_STATUS_REG, default_cmd);
return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}

// int (mouse_test_gesture)(UNUSED(uint8_t x_len), UNUSED(uint8_t tolerance) {
//     To be completed 
//     printf("%s: under construction\n", __func__);
//     return 1;
// }
