#ifndef _GAME_H_
#define _GAME_H_

#include "spacedef.h"
#include "xpm.h"




typedef struct {
     int x, y, speed, shot, lives;
     bool alive,right, left, colision;
} Alien;


typedef struct {

  xpm_image_t background_info;
  xpm_string_t* background_map;
  unsigned char* background_pic;

  xpm_image_t ship_info;
  xpm_string_t* ship_map;
  unsigned char* ship_pic;


  xpm_image_t ship_explosion_info;
  xpm_string_t* ship_explosion_map;
  unsigned char* ship_explosion_pic;

  xpm_image_t shield_info;
  xpm_string_t* shield_map;
  unsigned char* shield_pic;

  xpm_image_t SCORE_info;
  xpm_string_t* SCORE_map;
  unsigned char* SCORE_pic;

  xpm_image_t alien_info;
  xpm_string_t* alien_map;
  unsigned char* alien_pic;

  xpm_image_t shot_info;
  xpm_string_t* shot_map;
  unsigned char* shot_pic;

  xpm_image_t bang_info;
  xpm_string_t* bang_map;
  unsigned char* bang_pic;



  //Alien* aliens[3];
} Jogo;

typedef struct {
    int x, y, speed, shot, lives, score;
    bool right,left, alive, colision;
} Player;



Jogo* inicio();

void drawJogo(Jogo* jogo, Player* player, Alien* alien);

void draw_background(Jogo * jogo, Player* player);

void fim(Jogo* jogo);

Player* playerInit();

void move_ship(Jogo* jogo, Player* player);

void move_alien(Jogo* jogo, Alien* alien);

void check_player_fire(Jogo* jogo,Player* player);

void playerDelete(Player* player);

Alien* alienInit(Jogo* jogo);

void alienDelete(Alien* alien);

void alien_shot_init(Jogo* jogo, Alien* alien);

void alien_fire(Jogo* jogo, Alien* alien, Player* player);

void player_fire(Jogo* jogo, Alien* alien, Player* player);

void kbd_read();

bool shot_player(int x, int y, Player* player);

void set_mouse_events();

void show_score(Jogo* jogo, Player* player);

void display_number(Jogo* jogo,int x, int y, int number);



extern int mouseX,mouseY;


#endif
