#ifndef ___MOUSE_H___
#define ___MOUSE_H___

#include "menu.h"


typedef enum {INITIAL,UP,RIGHT,COMP_UP, COMP_RIGHT} state_t;
extern state_t state;

/**
 * @brief Subscreve as interrupcoes do rato
 * @param bit_no - numero do bit correspondente ao hook ID
 * @return 0(zero) se a operacao for bem sucedida, senao retorna 1
 */
int (mouse_subscribe)(uint8_t *bit_no);

/**
 * @brief Poe fim a subscricao das interrupcoes do rato
 * @return 0(zero) se a operacao for bem sucedida, senao retorna 1
 */
int (mouse_unsubscribe)();

/**
 * @brief Retorna o conteudo em OUT_BUF
 * @return Conteudo no endereco OUT_BUF, retorna 1 se a operacao nao for bem sucedida
 */
uint8_t (mouse_scan_byte)();

/**
 * @brief Retira os bytes retirados do endereço OUT_BUF e os coloca no array packet, durante o jogo
 */
void (mouse_ih)();

/**
 *
 * @param mouse
 */
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
