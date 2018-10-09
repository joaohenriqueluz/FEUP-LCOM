#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int globalCounter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  uint8_t st, lsb, msb;
  timer_get_conf(timer, &st);
  util_get_LSB(st, &lsb);
  util_get_MSB(st, &msb);

  uint8_t command = lsb | TIMER_LSB_MSB | (TIMER_SEL0 + timer);

  int timerFrq = 1193181;

  int div = timerFrq/freq;

  sys_outb(TIMER_CTRL, command);
  sys_outb((timer + TIMER_0), div);

  return 0;
}

int (timer_subscribe_int)(uint8_t *mUNUSED(bit_no)) {
  
  //sys_irqsetpolicy(TIMER_IRQ, IRQ_REEANABLE, bit_no);

  return 0;
}

int (timer_unsubscribe_int)() {
  
  //sys_irqrmpolicy(*hook_id);

  return 1;
}

void (timer_int_handler)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  
  uint8_t RB_command = TIMER_RB_CMD | TIMER_RB_SEL(timer);

  timer = TIMER_0 + timer;

  int check = sys_outb(TIMER_CTRL,RB_command);

  if(check == 1){
    perror("Error");
    return 1;
  }

  uint32_t adress = (uint32_t) *st;
  int check1=0;

  check1 = sys_inb((TIMER_0 + timer), &adress);

  if(check1 == 1)
  {
    perror("Error");
    return 1;
  }

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

