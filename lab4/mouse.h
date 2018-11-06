#ifndef ___MOUSE_H___
#define ___MOUSE_H___
int (mouse_subscribe)(uint8_t *bit_no);

int (mouse_unsubscribe)();

uint8_t (mouse_scan_byte)();

void (mouse_ih)();
void (mouse_remote)(uint16_t period);

int write_comand_mouse();
void disable_mouse();

int mouse_enable_stream();
int mouse_enable_remote();

#endif
