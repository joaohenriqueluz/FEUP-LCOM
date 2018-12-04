
#include <lcom/lcf.h>
#include <machine/int86.h> // /usr/src/include/arch/i386
#include <stdint.h>
#include <stdio.h>
#include <minix/sysutil.h>
#include "i8254.h"
#include "i8042.h"
#include "vbe_macros.h"
#include "spacedef.h"
#include "keyboard.h"

uint8_t GlobalRedScreeMask, GlobalGreenScreeMask, GlobalBlueScreeMask;

void* (vg_init)(uint16_t mode){

//mode_global= mode;
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

  if (r.u.b.ah != 0)
  {
    lm_free(&m);
    return 1;
  }

  *vmi_p = *(vbe_mode_info_t *) m.virt;

  lm_free(&m);

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



Bitmap* loadBitmap(const char* filename) {
    // allocating necessary size
    Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

    // open filename in read binary mode
    FILE *filePtr;
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
        return NULL;

    // read the bitmap file header
    BitmapFileHeader bitmapFileHeader;
    fread(&bitmapFileHeader, 2, 1, filePtr);

    // verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.type != 0x4D42) {
        fclose(filePtr);
        return NULL;
    }

    int rd;
    do {
        if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
            break;
    } while (0);

    if (rd !=1) {
        fprintf(stderr, "Error reading file\n");
        exit(-1);
    }

    // read the bitmap info header
    BitmapInfoHeader bitmapInfoHeader;
    fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

    // move file pointer to the begining of bitmap data
    fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

    // allocate enough memory for the bitmap image data
    unsigned char* bitmapImage = (unsigned char*) malloc(
            bitmapInfoHeader.imageSize);

    // verify memory allocation
    if (!bitmapImage) {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

    // make sure bitmap image data was read
    if (bitmapImage == NULL) {
        fclose(filePtr);
        return NULL;
    }

    // close file and return bitmap image data
    fclose(filePtr);

    bmp->bitmapData = bitmapImage;
    bmp->bitmapInfoHeader = bitmapInfoHeader;

    return bmp;
}

void drawBitmap(Bitmap* bmp, int x, int y, Alignment alignment) {
    if (bmp == NULL)
        return;

    int width = bmp->bitmapInfoHeader.width;
    int drawWidth = width;
    int height = bmp->bitmapInfoHeader.height;

    if (alignment == ALIGN_CENTER)
        x -= width / 2;
    else if (alignment == ALIGN_RIGHT)
        x -= width;

    if (x + width < 0 || x > h_res || y + height < 0
            || y > v_res)
        return;

    int xCorrection = 0;
    if (x < 0) {
        xCorrection = -x;
        drawWidth -= xCorrection;
        x = 0;

        if (drawWidth > h_res)
            drawWidth = h_res;
    } else if (x + drawWidth >= h_res) {
        drawWidth = h_res - x;
    }

    char* bufferStartPos;
    char* imgStartPos;

    int i;
    for (i = 0; i < height; i++) {
        int pos = y + height - 1 - i;

        if (pos < 0 || pos >= v_res)
            continue;

        bufferStartPos = video_mem;
        bufferStartPos += x * 4 + pos * h_res * 4;

        imgStartPos = (char*) bmp->bitmapData + xCorrection * 4 + i * width * 4;

        memcpy(bufferStartPos, imgStartPos, drawWidth * 4);
    }
}

void deleteBitmap(Bitmap* bmp) {
    if (bmp == NULL)
        return;

    free(bmp->bitmapData);
    free(bmp);
}

