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
  Bitmap* new_bmp = loadBitmap("/home/lcom/labs/proj/imagens/spacecomdiogo.bmp");
  drawBitmap(new_bmp,0,0,ALIGN_LEFT);
  sleep(25);
  vg_exit();

  return 0;
}