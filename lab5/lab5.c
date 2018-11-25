// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <machine/int86.h> // /usr/src/include/arch/i386
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "i8254.h"
#include "mode.h"
#include "vbe_macros.h"

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
    printf("Erro na função vg_init\n");
    return 1;
  }

  if(sleep(delay) != 0){
    printf("Erro na função sleep\n");
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

  mode_global = mode;

  if (vg_init(mode) == NULL){
    printf("Erro na função vg_enter\n");
    return 1;
  }

  if (vg_draw_rectangle(x,y,width,height,color) != 0)
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

int (video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if (vg_init(mode) == NULL){
    printf("Erro na função vg_enter\n");
    return 1;
  }

  mode_global = mode;

  if (draw_matrix(no_rectangles,first,step) != 0)
  {
    printf("Erro na função draw_matrix\n");
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

int (video_test_xpm)(const char *xpm[], uint16_t x, uint16_t y) {
  
  int height;
  int width;

   if (vg_init(INDEXED) == NULL){
      printf("Erro na função vg_enter\n");
      return 1;
    }

  if(vg_draw_xpm(xpm,x,y,&width,&height) != 0)
  {
    printf("Erro na função vg_draw_xpm\n");
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

int (video_test_move)(const char *xpm[], uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  
  if (vg_init(INDEXED) == NULL){
      printf("Erro na função vg_enter\n");
      return 1;
    }

  if(move_pixemap(xpm, xi, yi,  xf,  yf, speed, fr_rate) != 0){
    printf("Erro na funcao move_pixemap\n");
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

int (video_test_controller)() {
  struct reg86u r;
  mmap_t m;

  lm_init(true);

  vg_vbe_contr_info_t *info_p = malloc(sizeof(vg_vbe_contr_info_t));
  lm_alloc(sizeof(mmap_t), &m);                /* use liblm.a to initialize buf */
  phys_bytes buf = m.phys;

  memset(&r, 0, sizeof(r)); /* zero the structure */

  info_p->VBESignature[0] = 'V';
  info_p->VBESignature[1] = 'B';
  info_p->VBESignature[2] = 'E';
  info_p->VBESignature[3] = '2';

  r.u.w.ax = VBE_INFO;          /* VBE get mode info */
  /* translate the buffer linear address to a far pointer */
  r.u.w.es = PB2BASE(buf);    /* set a segment base */
  r.u.w.di = PB2OFF(buf);      /*set the offset accordingly */
  r.u.w.cx = mode;
  r.u.b.intno = 0x10;

  if( sys_int86(&r) != OK ){
    printf("vg_get_info(): sys_int86() failed \n");
    return 1;
  }

  info_p = (vg_vbe_contr_info_t *) m.virt;

  lm_free(&m);

  if (vg_display_vbe_contr_info(info_p) != 0)
  {
    printf("Erro a imprimeir na função video_test_controller\n");
    return 1;
  }

  lm_free(&m);


  return 0;
} 
