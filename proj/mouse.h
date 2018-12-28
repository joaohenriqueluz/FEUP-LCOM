#ifndef ___MOUSE_H___
#define ___MOUSE_H___

#include "menu.h"


typedef enum {INITIAL,UP,RIGHT,COMP_UP, COMP_RIGHT} state_t;
extern state_t state;

int (mouse_subscribe)(uint8_t *bit_no);

int (mouse_unsubscribe)();

uint8_t (mouse_scan_byte)();

void (mouse_ih)();

void mouse_menu_ih(Mouse* mouse);

int (kbc_write_cmd)();

void write_command(uint32_t cmd);

void printPacket();

void checkMenuPacket(Mouse* mouse);

void clearPacket();

void enable_cmd_int();

void disable_cmd_int();

bool fire_tolerance();

void check_line(struct mouse_ev evt);

void set_mouse_events();


bool protect_tolerance();






#endif
