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

uint8_t GlobalRedScreeMask, GlobalGreenScreeMask, GlobalBlueScreeMask;

unsigned int h_num, v_num;

void* (vg_init)(uint16_t mode){

	struct reg86u reg86;

    lm_init(true);

  	vbe_mode_info_t *vmi_p = malloc(sizeof(vbe_mode_info_t));

  if(vbe_info(mode, vmi_p) != 0){
    printf("Erro na função vbe_get_mode_info\n");
    return NULL;
  }

  if(map_vram(vmi_p) != 0){
    printf("Erro na função map_vram\n");
    return NULL;
  }

  memset(&reg86, 0, sizeof(reg86)); /* zero the structure */

  reg86.u.w.ax = VBE_MODE;
  reg86.u.w.bx = LINEAR | mode;
  reg86.u.b.intno = INIT;

  if(sys_int86(&reg86) != OK) {
      printf("vg_init(): sys_int86() failed \n");
      return NULL;
    }

  return video_mem;
}

int vbe_info(uint16_t mode,  vbe_mode_info_t *vmi_p){
  struct reg86u r;
  mmap_t m;
  lm_alloc(sizeof(mmap_t), &m);                /* use liblm.a to initialize buf */
  phys_bytes buf = m.phys;

  memset(&r, 0, sizeof(r)); /* zero the structure */

  r.u.w.ax = MODE_INFO;          /* VBE get mode info */
  /* translate the buffer linear address to a far pointer */
  r.u.w.es = PB2BASE(buf);    /* set a segment base */
  r.u.w.di = PB2OFF(buf);     /* set the offset accordingly */
  r.u.w.cx = mode;
  r.u.b.intno = 0x10;


  if( sys_int86(&r) != OK ){
    printf("vg_get_info(): sys_int86() failed \n");
    return 1;
  }

  *vmi_p = *(vbe_mode_info_t *) m.virt;

  lm_free(&m);

  return 0;
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
  if (bits_per_pixel == 15)
  {
    num_bytes_mode = 2;
  }
  GlobalRedScreeMask = vmi_p->RedMaskSize;
  GlobalGreenScreeMask = vmi_p->GreenMaskSize;
  GlobalBlueScreeMask = vmi_p->BlueMaskSize;


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

  //uint16_t x_old = x;

	for (int i = 0; i < height; ++i, y++)
  {
    //x = x_old;
    vg_draw_hline(x,y,len,color);
  }
	return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){

  for (int i = 0; i < len; ++i, x++)
  {
    char *temp = video_mem;
    temp += (h_res*y+x)*num_bytes_mode;
    uint32_t tempColor = color;
    for (unsigned int j = 0; j < num_bytes_mode; ++j, temp++)
    {
      *temp = tempColor;
      tempColor = tempColor >> 8;
    }
    //printf("X = %d\n", x);
  }
  return 0;
}

int vg_draw_xpm(const char *xpm[], uint16_t x, uint16_t y, int *width, int *height)
{
  char *sprite = read_xpm(xpm, width, height);
  uint16_t old_x = x;
  for (int i = 0; i < *height; ++i,y++)
  {
   x=old_x;
   for (int j = 0; j < *width; ++j,x++)
  {
    char *temp = video_mem;
    temp += (h_res*y+x)*num_bytes_mode;
    *temp = *(sprite + i * (*width) + j);
  }
}

return 0;
}

int draw_matrix(uint8_t no_rectangles, uint32_t first, uint8_t step){
  h_num = h_res/no_rectangles;
  v_num = v_res/no_rectangles;

  if(draw_indexed(no_rectangles, first,step) != 0){
      printf("Erro na função draw_indexed\n");
      return 1;
    }
   
  return 0;
}



int draw_indexed(uint8_t no_rectangles, uint32_t first, uint8_t step){
  uint32_t color;
  uint32_t R; 
  uint32_t G; 
  uint32_t B;
  unsigned int i=0, j=0;
  uint32_t old_first = first;
  for (unsigned int column = 0; column < no_rectangles; column++)
  {
    j=0;
    for (unsigned int row = 0; row < no_rectangles; row++)
    {
      switch(mode_global){
       case INDEXED:
          color = (first + (row * no_rectangles + column) * step) % (1 << bits_per_pixel);
          break;

        case DIRECT_COLOR_24:
          R = (((first >> 16) & 0x00ff) + column * step) % (1 << GlobalRedScreeMask);
          first = old_first;
          G = (((first >> 8) & 0x00ff) + row * step) % (1 << GlobalGreenScreeMask);
          first = old_first;
          B = ((first & 0x00ff) + (column + row) * step) % (1 << GlobalBlueScreeMask);
          first = old_first;
          color = ((R << 16) | (G << 8) | B);
          break;

        case DIRECT_COLOR_15:
          R = (((first >> 10) & 0x001f) + column * step) % (1 << GlobalRedScreeMask);
          first = old_first;
          G = (((first >> 5) & 0x001f) + row * step) % (1 << GlobalGreenScreeMask);
          first = old_first;
          B = ((first & 0x001f) + (column + row) * step) % (1 << GlobalBlueScreeMask);
          first = old_first;
          color = ((R << 10) | (G << 5) | B);
          break;

        case DIRECT_COLOR_16:
          R = (((first >> 11) & 0x001f) + column * step) % (1 << GlobalRedScreeMask);
          first = old_first;
          G = (((first >> 5) & 0x003f) + row * step) % (1 << GlobalGreenScreeMask);
          first = old_first;
          B = ((first & 0x001f) + (column + row) * step) % (1 << GlobalBlueScreeMask);
          first = old_first;
          color = ((R << 11) | (G << 5) | B);
          break;

        case DIRECT_COLOR_32:
          R = (((first >> 16) & 0x00ff) + column * step) % (1 << GlobalRedScreeMask);
          first = old_first;
          G = (((first >> 8) & 0x00ff) + row * step) % (1 << GlobalGreenScreeMask);
          first = old_first;
          B = ((first & 0x00ff) + (column + row) * step) % (1 << GlobalBlueScreeMask);
          first = old_first;
          color = ((R << 16) | (G << 8) | B);
          break;


        default:
          break;

      }
      //color = (first + (row * no_rectangles + column) * step) % (1 << bits_per_pixel);
      printf("ROW: %d  Column %d  cor: %d \n", row, column, color);

      if (vg_draw_rectangle(i,j,h_num,v_num,color) != 0)
      {
        printf("Erro na função vg_draw_rectangle\n");
        return 1;

      }
      j += v_num;

    }
    i += h_num;
  }
  return 0;
}


int move_pixemap(const char *xpm[], uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate){
  int ipc_status, ind = 0;
  message msg;
  unsigned int r;
  uint16_t globalXi = xi;
  uint16_t globalYi = yi;
  uint16_t fr_couter = 0;
  int width = 0, height = 0;
  int old_x = 0;
  int old_y = 0;
  uint8_t bit_no_timer, bit_no_kb, scancode1[1], scancode2[2];

  if(kb_subscribe(&bit_no_kb) != 0){
    printf("Erro na funcao kb_subscribe\n");
  }

  if(timer_subscribe_int(&bit_no_timer) != 0){
    printf("Erro na funcao timer_subscribe_int\n");
  }

  int freq = sys_hz();

  int frame_counter = freq/fr_rate;
  //float distance = (speed*fr_rate)/freq;


  uint32_t irq_set_kb = BIT(bit_no_kb);
  uint32_t irq_set_timer = BIT(bit_no_timer);

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
              if((globalCounter % frame_counter) == 0)
              {
               
                if (globalXi == xf && globalYi == yf)
                {
                  continue;
                }

                if(speed < 0)
                {
                   fr_couter++;
                    if (abs(speed) == fr_couter)
                  {
                    fr_couter = 0;
                  if (globalXi <= xf && globalYi == yf)
                  {
                      vg_draw_rectangle(old_x,old_y,1,height,0);
                      vg_draw_xpm(xpm,globalXi,globalYi, &width, &height);
                      old_x = globalXi;
                      old_y = globalYi;
                      globalXi += 1;
                 }
                else if (globalXi >= xf && globalYi == yf)
                 {
                    vg_draw_rectangle(globalXi+width,old_y,1,height,0);
                    vg_draw_xpm(xpm,globalXi,globalYi, &width, &height);
                    old_x = globalXi;
                    old_y = globalYi;
                    globalXi -= 1;
                  }
                else if (globalXi == xf && globalYi <= yf)
                  {
                    vg_draw_rectangle(old_x,old_y,width,1,0);
                    vg_draw_xpm(xpm,globalXi,globalYi, &width, &height);
                    old_x = globalXi;
                    old_y = globalYi;
                   globalYi += 1;
                  }
                 else if (globalXi == xf && globalYi >= yf)
                 {
                    vg_draw_rectangle(old_x,globalYi+height,width,1,0);
                    vg_draw_xpm(xpm,globalXi,globalYi, &width, &height);
                    old_x = globalXi;
                    old_y = globalYi;
                    globalYi -= 1;
                  }
                }
              }
               
               else 
              {
                if (globalXi <= xf && globalYi == yf)
                  {
                  vg_draw_rectangle(old_x,old_y,speed,height,0);
                  vg_draw_xpm(xpm,globalXi,globalYi, &width, &height);
                  old_x = globalXi;
                  old_y = globalYi;
                  globalXi += speed;
                }
                else if (globalXi >= xf && globalYi == yf)
                {
                  vg_draw_rectangle(globalXi+width,old_y,speed,height,0);
                  vg_draw_xpm(xpm,globalXi,globalYi, &width, &height);
                  old_x = globalXi;
                  old_y = globalYi;
                  globalXi -= speed;
                }
                else if (globalXi == xf && globalYi <= yf)
                {
                  vg_draw_rectangle(old_x,old_y,width,speed,0);
                  vg_draw_xpm(xpm,globalXi,globalYi, &width, &height);
                  old_x = globalXi;
                  old_y = globalYi;
                  globalYi += speed;
                }
                else if (globalXi == xf && globalYi >= yf)
                {
                  vg_draw_rectangle(old_x,globalYi+height,width,speed,0);
                  vg_draw_xpm(xpm,globalXi,globalYi, &width, &height);
                  old_x = globalXi;
                  old_y = globalYi;
                  globalYi -= speed;
                }
              }
            }

            }

            if (msg.m_notify.interrupts & irq_set_kb)
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

  if(timer_unsubscribe_int() != 0){
    printf("Erro na funcao timer_unsubscribe_int\n");
  }
  if(kb_unsubscribe() != 0){
    printf("Erro na funcao kb_unsubscribe\n");
  }

  return 0;
}

int get_controller(){
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



