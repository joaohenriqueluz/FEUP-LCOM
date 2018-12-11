#pragma once

/** @defgroup Bitmap Bitmap
 * @{
 * Functions for manipulating bitmaps. 
 * These functions where developed by Henrique Ferrolho and con be found in the link http://difusal.blogspot.pt/2014/09/minixtutorial-8-loading-bmp-images.html
 */


//////////////////////////////////////////////////////////////////////////////////
static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */
static int bits_per_pixel; /* Number of VRAM bits per pixel */
static int num_bytes_mode;
//////////////////////////////////////////////////////////////////////////////////

static int playerX = 487;
//////////////////////////////////////////////////////////////////////////////////

int vbe_info(uint16_t mode,  vbe_mode_info_t *vmi_p);

int map_vram(vbe_mode_info_t *vmi_p);

int vg_draw_xpm(unsigned char* pic,  xpm_image_t* xpm, uint16_t x, uint16_t y);

