#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t UNUSED(freq)) {

  uint8_t st, lsb, msb;
  timer_get_conf(timer, &st);
  util_get_LSB(st, &lsb);
  util_get_MSB(st, &msb);

  //sys_outb(TIMER_CTRL, )


  return 0;
}

int (timer_subscribe_int)(uint8_t *UNUSED(bit_no)) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  
  uint8_t RB_command = TIMER_RB_CMD | TIMER_RB_SEL(timer);
  int check = sys_outb(TIMER_CTRL,RB_command);
  sys_outb(TIMER_CTRL,RB_command);

  if(check == 1){
    perror("Error");
    return 1;
  }
  uint32_t adress = (uint32_t) *st;

  check = sys_inb(timer + TIMER_0, &adress);

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                       enum timer_status_field field) {

  union timer_status_field_val conf;
  conf.byte = st;

  timer_print_config(timer, field, conf);


  return 0;
}

