#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

extern int globalCounter;
int globalHookId;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  uint8_t st, lsb, msb;
  timer_get_conf(timer, &st);

  st = (st & 0x0F);

  uint8_t command = st | TIMER_LSB_MSB;

 uint16_t div = TIMER_FREQ/freq;
  util_get_LSB(div, &lsb);
  util_get_MSB(div, &msb);


switch(timer)
{
case 0:
 {
 command |= TIMER_SEL0;

  sys_outb(TIMER_CTRL, command);
  sys_outb(TIMER_0, lsb);
  sys_outb(TIMER_0, msb);
  break;
}

case 1:
 {
 command |= TIMER_SEL1;

  sys_outb(TIMER_CTRL, command);
  sys_outb(TIMER_1, lsb);
  sys_outb(TIMER_1, msb);
  break;
}

  case 2:
  {
   command |= TIMER_SEL2;

   sys_outb(TIMER_CTRL, command);
   sys_outb(TIMER_2, lsb);
   sys_outb(TIMER_2, msb);
   break;
  }
  default:
    break;
  }
  return 0;
  }

int (timer_subscribe_int)(uint8_t *bit_no) {

  int temp_hook = TEMP_HOOK;
  
  sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &temp_hook);

  globalHookId = temp_hook;

 *bit_no = TEMP_HOOK;

  return 0;
}

int (timer_unsubscribe_int)() {
  
  sys_irqrmpolicy(&globalHookId);

  return 0;
}

void (timer_int_handler)() {
  ++globalCounter;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  
  uint8_t RB_command = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);


  int check = sys_outb(TIMER_CTRL,RB_command);

  if(check == 1){
    perror("Error");
    return 1;
  }

  uint32_t adress;
  int check1=0;

  switch(timer){
    case 0:
    check1 = sys_inb((TIMER_0), &adress);
    break;

    case 1:
    check1 = sys_inb((TIMER_1), &adress);
    break;
    
    case 2:
    check1 = sys_inb((TIMER_2), &adress);
    break;
    
  }

  

  if(check1 == 1)
  {
    perror("Error");
    return 1;
  }

  *st=(uint8_t) adress;

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                       enum timer_status_field field) {

  union timer_status_field_val conf;



  switch(field)
  {

    case all:
    {
      conf.byte= st;
      break;
    }
    case initial:
    {
    //10100010  TIMER_LSB_MSB  00110000 00100000 00000010
      conf.in_mode = (st & TIMER_LSB_MSB) >> 4;
      break;
    }

    case mode:
    {
      conf.count_mode = (st & TIMER_SQR_WAVE) >> 1;
      break;
    }

    case base:
    {
      conf.bcd = (st & TIMER_BCD);
      break;
    }

  }

  timer_print_config(timer, field, conf);


  return 0;
}

