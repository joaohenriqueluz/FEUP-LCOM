#include <lcom/lcf.h>
#include <machine/int86.h> // /usr/src/include/arch/i386
#include "i8042.h"
#include "game.h"
#include "spacedef.h"
#include "keyboard.h"

int shotX, shotY;
extern int h_res, v_res;

Jogo* inicio(){
	Jogo* jogo = (Jogo*) malloc(sizeof(Jogo));

	jogo->background_map =  space_xpm;
	jogo->background_pic = xpm_load(jogo->background_map, XPM_5_6_5, &jogo->background_info);

	jogo->ship_map =  rocket2_xpm;
	jogo->ship_pic = xpm_load(jogo->ship_map, XPM_5_6_5, &jogo->ship_info);

	jogo->shot_map =  redLaser_xpm;
	jogo->shot_pic = xpm_load(jogo->shot_map, XPM_5_6_5, &jogo->shot_info);

	return jogo;
}

void fim(Jogo* jogo){
	free(jogo);
}

Player* playerInit(Jogo* jogo){
	Player* player = (Player*) malloc(sizeof(Player));

	player->x = h_res/2 - (jogo->ship_info.width)/2;
	player->y = v_res-(jogo->ship_info.height)*3.5;
	player->speed = 10;
	player->shot = 0;
	player->lives = 5;
	player->score = 0;

	return player;
}

void move_ship(Jogo* mib, Player* willsmith){
	if (byte == LEFT_ARROW)
	{
		if (willsmith->x + mib->ship_info.width > h_res)
		{
			willsmith->x = h_res - mib->ship_info.width;
		}
		else{
			willsmith->x += willsmith->speed;
		}
	}
	else if (byte == RIGHT_ARROW)
	{
		if (willsmith->x - willsmith->speed < 0)
		{
			willsmith->speed = 0;
		}
		else{
			willsmith->x -= willsmith->speed;
		}
	}
	else if (byte == SPACE_BAR && !(willsmith->shot))
	{
		willsmith->shot = 1;
		shotX = willsmith->x + (mib->ship_info.width)/2 - (mib->shot_info.width)/2;
		shotY = willsmith->y;
	}
}

void playerDelete(Player* player){
	free(player);
}

Alien* alienInit(){
	Alien* frank = (Alien*) malloc(sizeof(Alien));

	frank->x = h_res/2;
	frank->y = 0;
	frank->speed = 0;

	return frank;
}

void alienDelete(Alien* frank){
	free(frank);
}

void drawJogo(Jogo* mib, Player* willsmith){
	vg_draw_xpm(mib->background_pic, &mib->background_info, 0, 0);
	vg_draw_xpm(mib->ship_pic, &mib->ship_info, willsmith->x, willsmith->y);
	if (willsmith->shot)
	{
		if (shotY - 10 >= 0)
		{
			shotY -= 10;
			vg_draw_xpm(mib->shot_pic, &mib->shot_info, shotX, shotY);
			return;
		}
	}
	willsmith->shot = 0;
}

void kbd_read(){
  byte = kb_scan_byte(false);

  if (byte == ESC_BREAK)
  {
      is_over = true;
      make = false;
      return;
  }
 if (byte == TWO_BYTE_SCAN)
 {
    size = 2;
    return;
  }
  else{
  if (byte & 0x80)
  {
      make = false;
      return;
   }
   else{
    make = true;
   }
 }
 
   return;
}
