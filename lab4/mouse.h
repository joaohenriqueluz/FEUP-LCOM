#ifndef ___MOUSE_H___
#define ___MOUSE_H___
int (mouse_subscribe)(uint8_t *bit_no);

int (mouse_unsubscribe)();

uint8_t (mouse_scan_byte)();

void (mouse_ih)();

#endif
