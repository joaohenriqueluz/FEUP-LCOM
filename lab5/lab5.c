// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <machine/int86.h> // /usr/src/include/arch/i386
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "i8254.h"
#include "mode.h"

uint16_t mode_global;


// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (video_test_init)(uint16_t mode, uint8_t delay) {
  if(vg_init(mode) == NULL){
    printf("Erro na função vg_enter\n");
    return 1;
  }

  if(wait(delay) != 0){
    printf("Erro na função delay\n");
    return 1;
  }

  if(vg_exit() != 0){
    printf("Erro na função vg_exit\n");
    return 1;
  }

  return 0;
}

int (video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                       uint16_t width, uint16_t height, uint32_t color) {

  if (vg_init(mode) == NULL){
    printf("Erro na função vg_enter\n");
    return 1;
  }

  mode_global = mode;

  if (vg_draw_rect(x,y,width,height,color) != 0)
  {
    printf("Erro a imprimir o retangulo!\n");
    return 1;
  }

  if(program_exit() != 0){
    printf("Erro na função program_exit\n");
    return 1;
  }

  if(vg_exit() != 0){
    printf("Erro na função vb_exit\n");
    return 1;
  }
  

  return 0;
}

int (video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first) {
  /* To be completed */
  printf("%s(0x%03x, %u, 0x%08x): under construction\n", __func__, mode, no_rectangles, first);

  return 1;
}
