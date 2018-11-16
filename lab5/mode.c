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
bool is_over = false, make;
uint8_t byte;
extern uint16_t mode_global;

static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */

static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
//static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

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

  	vbe_mode_info_t *vmi_p = malloc(sizeof(vbe_mode_info_t));

  if(vbe_get_mode_info(mode, vmi_p) != 0){
    printf("Erro na função vbe_get_mode_info\n");
    return NULL;
  }

  if(map_vram(mode,vmi_p) != 0){
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

  if(kb_subscribe(&bit_no) != 0)
    printf("Erro na funcao kb_subscribe\n");
	return 1;

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

  if(kb_unsubscribe() != 0)
    printf("Erro na funcao kb_unsubscribe\n");
	return 1;

  return 0;
}

int map_vram(uint16_t mode, vbe_mode_info_t *vmi_p){

	h_res = vmi_p->XResolution;
	v_res = vmi_p->YResolution;

	int r;
	struct minix_mem_range mr;
	unsigned int vram_base = vmi_p->PhysBasePtr;  /* VRAM's physical addresss */
	unsigned int vram_size = (vmi_p->XResolution)*v_res*num_bytes_mode(mode);  /* VRAM's size, but you can use
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

int vg_draw_rect(uint16_t x, uint16_t y, uint16_t len, uint16_t height, uint32_t color){

	for (uint16_t i = 0; i < height; ++i)
	{
		video_mem += num_bytes_mode(mode_global)*h_res*(y++) + x*num_bytes_mode(mode_global);
		
		if (vg_draw_hline(len,color) != 0)
		{
			printf("Erro a imprimir linha %d\n", i);
			return 1;
		}
	}
	return 0;
}

int vg_draw_hline(uint16_t len, uint32_t color){
	for (uint16_t i = 0; i < len; ++i)
	{
		*video_mem = color;
		video_mem += num_bytes_mode(mode_global);
	}
	return 0;
}

int num_bytes_mode(uint16_t mode){
	int size;
	switch(mode){
		case INDEXED:
			size = 1;
			break;
		case DIRECT_COLOR_16:
			size = 2;
			break;
		case DIRECT_COLOR_15:
			size = 2;
			break;
		case DIRECT_COLOR_24:
			size = 3;
			break;
		case DIRECT_COLOR_32:
			size = 4;
			break;
		default:
			size = 1;
			break;
	}
	return size;
}



