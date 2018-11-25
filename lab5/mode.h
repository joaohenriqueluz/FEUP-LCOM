#ifndef ___MODE_H___
#define ___MODE_H___

void* (vg_init)(uint16_t mode);

int wait(uint8_t time);

int program_exit();

int map_vram(vbe_mode_info_t *vmi_p);

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t len, uint16_t height, uint32_t color);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

int draw_matrix(uint8_t no_rectangles, uint32_t first, uint8_t step);

int draw_indexed(uint8_t no_rectangles, uint32_t first, uint8_t step);

int vg_draw_xpm(const char *xpm[], uint16_t x, uint16_t y, int *width, int *height);

int move_pixemap(const char *xpm[], uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate);

int vbe_get_info(uint16_t mode,  vbe_mode_info_t *vmi_p);

#endif
