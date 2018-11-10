#ifndef ___MOUSE_H___
#define ___MOUSE_H___
int (mouse_subscribe)(uint8_t *bit_no);

int (mouse_unsubscribe)();

uint8_t (mouse_scan_byte)();

void (mouse_ih)();

void (mouse_remote)();

int (kbc_write_cmd)();

//int fst_command_mouse();

void write_command(uint32_t cmd);

//int check_command(uint32_t cmd);

void printPacket();

void clearPacket();

//void disable_mouse();

void enable_cmd_int();

void disable_cmd_int();

void disable_cmd_remote();

void check_line(struct mouse_ev evt);

void set_mouse_events();
#endif
