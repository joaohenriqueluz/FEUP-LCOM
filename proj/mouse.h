#ifndef ___MOUSE_H___
#define ___MOUSE_H___

typedef enum {INITIAL,MOVE, COMP} state_t;
extern state_t state;

int (mouse_subscribe)(uint8_t *bit_no);

int (mouse_unsubscribe)();

uint8_t (mouse_scan_byte)();

void (mouse_ih)();

int (kbc_write_cmd)();

void write_command(uint32_t cmd);

void printPacket();

void clearPacket();

void enable_cmd_int();

void disable_cmd_int();

bool tolerance();

void check_line(struct mouse_ev evt);

void set_mouse_events();








#endif
