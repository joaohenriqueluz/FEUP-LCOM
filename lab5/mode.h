#ifndef ___MODE_H___
#define ___MODE_H___

extern void *video_mem;

int vg_enter(uint16_t mode);

int vg_start(uint16_t mode);

int wait(uint8_t time);

int program_exit();

int map_vram(uint16_t mode, vbe_mode_info_t *vmi_p);

int num_bytes_mode(uint16_t mode);

#endif
