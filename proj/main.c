// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "i8254.h"
#include "i8042.h"
#include "vbe_macros.h"
#include "spacedef.h"
#include "keyboard.h"
#include "game.h"
#include "mouse.h"
#include "menu.h"

// Any header files included below this line should have been created by you


bool explosion= false;

int counterExplosion =0;

uint8_t byte, packet[3];

int byteCounter = 0;

int ship_counterExplosion = 0;

bool ship_explosion= false;

extern game_st game_state;




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

int (interrupt_loop)(Jogo* mib, Player* willSmith, Alien* frank) {
  int ipc_status;
  message msg;
  unsigned int r;
  uint8_t bit_no_timer, bit_no_kb, bit_no_mouse;
  unsigned counter = 0, shots_fired=0;
  //uint8_t seconds = 0;

  enable_cmd_int();

  if(mouse_subscribe(&bit_no_mouse) != 0)
    printf("Erro na funcao mouse_subscribe\n");

  if(kb_subscribe(&bit_no_kb) != 0)
    printf("Erro na funcao kb_subscribe\n");

  if(timer_subscribe_int(&bit_no_timer) != 0)
    printf("Erro na funcao timer_subscribe_int\n");

  uint32_t irq_set_kb = BIT(bit_no_kb);
  uint32_t irq_set_timer = BIT(bit_no_timer);
  uint32_t irq_set_mouse = BIT(bit_no_mouse);


  drawJogo(mib,willSmith,frank);

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
              if (game_state == GAME)
              {
                check_player_fire(mib,willSmith);
                drawJogo(mib,willSmith,frank);
                double_buffering();
              }
              else if (game_state == PAUSE)
              {
                drawPause(mib);
                double_buffering();
              }
              else if (game_state == GAME_OVER)
              {
                drawGameOver(mib);
                double_buffering();
                sleep(3);
                is_over = true;
                game_state = MAIN_MENU;
              }
              else if (game_state == WON)
              {
                drawWon(mib);
                double_buffering();
                game_state = MAIN_MENU;
              }

            }

            if (msg.m_notify.interrupts & irq_set_kb)
              {
                if(game_state == GAME)
                  {
                    kbd_read();
                    move_ship(mib, willSmith);
                  }
              }
            
            if (msg.m_notify.interrupts & irq_set_mouse)
              {
                 if(game_state == GAME)
                 {
                  mouse_ih();
                 }
              }
            
         }
     } else { /* received a standard message, not a notification */
         /* no standard messages expected: do nothing */}

    if (globalCounter % sys_hz() == 0){
      counter++;
     // printf("Score = %d \n", willSmith->score);
      
      
      if(explosion)
        {counterExplosion++;}

      if(ship_explosion)
      {
        ship_counterExplosion++;
      }
    }
    if(counter == 2 && !frank->shot){
      frank->shot = 1;
      alien_shot_init(mib,frank);
      shots_fired++;
      counter = 0;
    }
    else if(counter == 2){
      counter=0;
      frank->shot = 0;
    }

    if(counterExplosion == 1)
    {

      counterExplosion =0;
      explosion = false;
    }

    if(ship_counterExplosion ==1)
    {
      ship_counterExplosion = 0;
      ship_explosion = false;
    }


      
 }
  if(timer_unsubscribe_int() != 0)
    printf("Erro na funcao timer_unsubscribe_int\n");
  if(kb_unsubscribe() != 0)
    printf("Erro na funcao kb_unsubscribe\n");

  if(mouse_unsubscribe() != 0)
    printf("Erro na funcao mouse_unsubscribe\n");

  disable_cmd_int();

  game_state = MAIN_MENU;
  is_over = false;

  return 0;
}


int menu_interrupt_loop(Jogo* jogo, Mouse* mouse){
  int ipc_status;
  message msg;
  unsigned int r;
  uint8_t bit_no, bit_no_timer, bit_no_mouse;

  enable_cmd_int();

  if(mouse_subscribe(&bit_no_mouse) != 0)
    printf("Erro na funcao mouse_subscribe\n");
  if(kb_subscribe(&bit_no) != 0)
    printf("Erro na funcao mouse_subscribe\n");
  if(timer_subscribe_int(&bit_no_timer) != 0)
    printf("Erro na funcao timer_subscribe_int\n");

 uint32_t irq_set_kb = BIT(bit_no);
 uint32_t irq_set_timer = BIT(bit_no_timer);
 uint32_t irq_set_mouse = BIT(bit_no_mouse);

  while(game_state == MAIN_MENU || game_state == INSTRUCTIONS)
   {

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
              if (game_state == MAIN_MENU)
              {
                drawMenu(jogo, mouse);
                double_buffering();
              }
              else if (game_state == INSTRUCTIONS)
              {
                drawInstructions(jogo);
                double_buffering();
              }

            } 
            if (msg.m_notify.interrupts & irq_set_kb)
              {
                  kbd_read();
                  menu_kb_ih();
              }
            if (msg.m_notify.interrupts & irq_set_mouse)
            {
              if (game_state == MAIN_MENU)
              {
                mouse_menu_ih(mouse);
              }
            }
         }
     }
 }

 

  if(kb_unsubscribe() != 0)
    printf("Erro na funcao mouse_unsubscribe\n");
  if(timer_unsubscribe_int() != 0)
    printf("Erro na funcao timer_unsubscribe_int\n");
  if(mouse_unsubscribe() != 0)
    printf("Erro na funcao mouse_unsubscribe\n");

  disable_cmd_int();


  return 0;
}



// int (proj_main_loop)(){

//   vg_init(0x11a);
//   Jogo* mib = (Jogo*) inicio();
//   Player* willSmith = (Player*) playerInit(mib);
//   Alien* frank = (Alien*) alienInit(mib);
//   interrupt_loop(mib, willSmith, frank);
//   vg_exit();

//   playerDelete(willSmith);
//   fim(mib);

//   return 0;
// }

int (proj_main_loop)(){

  vg_init(0x11a);

  Jogo* mib = (Jogo*) inicio();
  Player* willSmith = (Player*) playerInit(mib);
  Alien* frank = (Alien*) alienInit(mib);
  Mouse* mouse = (Mouse*) mouseInit();

  while(1){
    switch(game_state){
    case MAIN_MENU:
      printf("state menu\n");
      menu_interrupt_loop(mib,mouse);
      break;
    case GAME:
      printf("state game\n");
      interrupt_loop(mib, willSmith, frank);
      game_state = MAIN_MENU;
      break;
    case COMP:
      printf("state end\n");
      vg_exit();
      playerDelete(willSmith);
      fim(mib);
      return 0;

    default:
      break;
    }
  }

  vg_exit();
  playerDelete(willSmith);
  fim(mib);
  return 0;
}




