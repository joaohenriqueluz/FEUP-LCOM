#ifndef ___MOUSE_H___
#define ___MOUSE_H___
int (mouse_subscribe)(uint8_t *bit_no);

int (mouse_unsubscribe)();

uint8_t (mouse_scan_byte)();

void (mouse_ih)();

void (mouse_remote)(uint16_t period);

int fst_command_mouse();

int write_commad(uint32_t cmd);

int check_command(uint32_t cmd);

void printPacket();

void clearPacket();

void disable_mouse();

#endif
