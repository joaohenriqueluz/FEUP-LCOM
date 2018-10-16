#ifndef ___KEYBOARD_H___
#define ___KEYBOARD_H___
int (kb_subscribe)(uint8_t *bit_no);

int (kb_unsubscribe)();

uint8_t (kb_scan_byte)();

#endif
