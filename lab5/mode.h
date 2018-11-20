#ifndef ___MODE_H___
#define ___MODE_H___

void* (vg_init)(uint16_t mode);

int wait(uint8_t time);

int program_exit();

int map_vram(vbe_mode_info_t *vmi_p);

//int num_bytes_mode(uint16_t mode);

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t len, uint16_t height, uint32_t color);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

int draw_matrix(uint8_t no_rectangles, uint32_t first, uint8_t step);

int draw_indexed(uint8_t no_rectangles, uint32_t first, uint8_t step);

#endif
