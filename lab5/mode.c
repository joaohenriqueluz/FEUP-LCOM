#include <lcom/lcf.h>
#include <machine/int86.h> // /usr/src/include/arch/i386
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "i8254.h"
#include "vbe_macros.h"
#include "mode.h"
#include "keyboard.h"

int globalCounter = 0, size = 0;
bool is_over = false, make = true;
uint8_t byte;
extern uint16_t mode_global;

static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */

static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned num_bytes_mode;

int h_num, v_num;

void* (vg_init)(uint16_t mode){
	struct reg86u reg86;

	memset(&reg86, 0, sizeof(reg86));	/* zero the structure */

	reg86.u.w.ax = VBE_MODE;
	reg86.u.w.bx = LINEAR | mode;
	reg86.u.b.intno = INIT;

	if(sys_int86(&reg86) != OK) {
  		printf("vg_enter(): sys_int86() failed \n");
  		return NULL;
  	}

    lm_init(true);

  	vbe_mode_info_t *vmi_p = malloc(sizeof(vbe_mode_info_t));

  if(vbe_get_mode_info(mode, vmi_p) != 0){
    printf("Erro na função vbe_get_mode_info\n");
    return NULL;
  }

  if(map_vram(vmi_p) != 0){
    printf("Erro na função map_vram\n");
    return NULL;
  }

  return video_mem;
}

int wait(uint8_t time){
	int ipc_status;
	message msg;
	unsigned int r;
	uint8_t bit_no;
	globalCounter = 0;

	if(timer_subscribe_int(&bit_no) != 0){
		printf("Erro na função timer_subscribe_int \n");
		return 1;
	}

	int freq = sys_hz();

	uint32_t irq_set = BIT(bit_no);

	while(globalCounter/freq < time) {

     if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
          printf("driver_receive failed with: %d", r);
         continue;
     } 
     if (is_ipc_notify(ipc_status)) { /* received notification */
         switch (_ENDPOINT_P(msg.m_source)) {
             case HARDWARE: /* hardware interrupt notification */       
                 if (msg.m_notify.interrupts & irq_set) {
                    timer_int_handler();
                  }
                 break;
              default:
                  break; /* no other notifications expected: do nothing */ 
         }
     } else { /* received a standard message, not a notification */
         /* no standard messages expected: do nothing */
     }
 }

  if(timer_unsubscribe_int() != 0){
  	printf("Erro na função timer_unsubscribe_int \n");
  	return 1;
  }

  return 0;
}

int program_exit() {

  int ipc_status, ind = 0;
  message msg;
  unsigned int r;
  uint8_t bit_no, scancode1[1], scancode2[2];

  if(kb_subscribe(&bit_no) != 0){
    printf("Erro na funcao kb_subscribe\n");
	   return 1;
  }

 uint32_t irq_set = BIT(bit_no);

 // printf("%d\n", irq_set );

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
            if (msg.m_notify.interrupts & irq_set)
              {
                  kbc_ih();
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
 }

  if(kb_unsubscribe() != 0){
      printf("Erro na funcao kb_unsubscribe\n");
	   return 1;
   }

  return 0;
}

int map_vram(vbe_mode_info_t *vmi_p){

	h_res = vmi_p->XResolution;
	v_res = vmi_p->YResolution;
  bits_per_pixel = vmi_p->BitsPerPixel;
  num_bytes_mode = bits_per_pixel/8;

	int r;
	struct minix_mem_range mr;
	unsigned int vram_base = vmi_p->PhysBasePtr;  /* VRAM's physical addresss */
	unsigned int vram_size = h_res*v_res*num_bytes_mode;  /* VRAM's size, but you can use
				    the frame-buffer size, instead */

	/* Allow memory mapping */

	mr.mr_base = (phys_bytes) vram_base;	
	mr.mr_limit = mr.mr_base + vram_size;  

	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
   		panic("sys_privctl (ADD_MEM) failed: %d\n", r);
   		return 1;
   	}

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

	if(video_mem == MAP_FAILED){
  	 	panic("couldn't map video memory");
  	 	return 1;
  	 }

  	return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t len, uint16_t height, uint32_t color){

  uint16_t x_old = x;

	for (int i = 0; i < height; ++i, y++)
  {
    x = x_old;
    vg_draw_hline(x,y,len,color);
  }
	return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){

  for (int i = 0; i < len; ++i, x++)
  {
    char *temp = video_mem;
    temp += (h_res*y+x)*num_bytes_mode;
    *temp = color;
  }
  return 0;
}

int draw_matrix(uint8_t no_rectangles, uint32_t first, uint8_t step){
  h_num = h_res/no_rectangles;
  v_num = v_res/no_rectangles;

  switch(mode_global){
    case 0x105:
      if(draw_indexed(no_rectangles, first,step) != 0){
        printf("Erro na função draw_indexed\n");
        return 1;
      }
      break;

    default:
      // if (draw_direct(no_rectangles,first,step) != 0)
      // {
      //   printf("Erro na função draw_direct\n");
      //   return 1;
      // }
    break;
  }
  return 0;
}

int draw_indexed(uint8_t no_rectangles, uint32_t first, uint8_t step){
  uint32_t color;

  for (int i = 0; i < no_rectangles; i += h_num)
  {
    for (int j = 0; j < no_rectangles; j += v_num)
    {
      color = (first + (j * no_rectangles + i) * step) % (1 << bits_per_pixel);
      if (vg_draw_rectangle(i,j,no_rectangles,no_rectangles,color) != 0)
      {
        printf("Erro na função vg_draw_rectangle\n");
        return 1;
      }
    }
  }
  return 0;
}

// int num_bytes_mode(uint16_t mode){
// 	int size;
// 	switch(mode){
// 		case INDEXED:
// 			size = 1;
// 			break;
// 		case DIRECT_COLOR_16:
// 			size = 2;
// 			break;
// 		case DIRECT_COLOR_15:
// 			size = 2;
// 			break;
// 		case DIRECT_COLOR_24:
// 			size = 3;
// 			break;
// 		case DIRECT_COLOR_32:
// 			size = 4;
// 			break;
// 		default:
// 			size = 1;
// 			break;
// 	}
// 	return size;
// }



