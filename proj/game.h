#ifndef _GAME_H_
#define _GAME_H_

#include "spacedef.h"
#include "xpm.h"

typedef struct {

	xpm_image_t background_info;
  xpm_string_t* background_map;
  unsigned char* background_pic;

  xpm_image_t ship_info;
  xpm_string_t* ship_map;
  unsigned char* ship_pic;

  xpm_image_t shot_info;
  xpm_string_t* shot_map;
  unsigned char* shot_pic;
} Jogo;

typedef struct {
    int x, y, speed, shot, lives, score;
} Player;

typedef struct {
    unsigned int x, y, speed;
} Alien;

Jogo* inicio();

void drawJogo(Jogo* jogo, Player* player);

void fim(Jogo* jogo);

Player* playerInit();

void move_ship(Jogo* jogo, Player* player);

void playerDelete(Player* player);

Alien* alienInit();

void alienDelete(Alien* alien);

void kbd_read();

#endif
