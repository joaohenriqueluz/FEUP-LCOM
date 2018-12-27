#include <lcom/lcf.h>
#include <machine/int86.h> // /usr/src/include/arch/i386
#include "i8042.h"
#include "game.h"
#include "spacedef.h"
#include "keyboard.h"
#include <stdio.h>

int shotX, shotY, alien_shotX, alien_shotY;
extern int h_res, v_res;



extern bool explosion;
extern bool ship_explosion;
extern int counterExplosion;
extern int ship_counterExplosion;
extern bool allowed_to_fire;
extern bool protected;


Jogo* inicio(){
	Jogo* jogo = (Jogo*) malloc(sizeof(Jogo));

	jogo->background_map =  space_xpm;
	jogo->background_pic = xpm_load(jogo->background_map, XPM_5_6_5, &jogo->background_info);

	jogo->ship_map =  alien_xpm;
	jogo->ship_pic = xpm_load(jogo->ship_map, XPM_5_6_5, &jogo->ship_info);

	jogo->shield_map =  shield_xpm;
	jogo->shield_pic = xpm_load(jogo->shield_map, XPM_5_6_5, &jogo->shield_info);

	jogo->SCORE_map =  SCORE_xpm;
	jogo->SCORE_pic = xpm_load(jogo->SCORE_map, XPM_5_6_5, &jogo->SCORE_info);

	jogo->shot_map =  shot_xpm;
	jogo->shot_pic = xpm_load(jogo->shot_map, XPM_5_6_5, &jogo->shot_info);

	jogo->alien_map =  alien_xpm;
	jogo->alien_pic = xpm_load(jogo->alien_map, XPM_5_6_5, &jogo->alien_info);

	jogo->bang_map =  explosion_xpm;
	jogo->bang_pic = xpm_load(jogo->bang_map, XPM_5_6_5, &jogo->bang_info);

	jogo->ship_explosion_map =  explosion100_xpm;
	jogo->ship_explosion_pic = xpm_load(jogo->ship_explosion_map, XPM_5_6_5, &jogo->ship_explosion_info);

	return jogo;
}

void fim(Jogo* jogo){
	free(jogo);
}

Player* playerInit(Jogo* jogo){
	Player* player = (Player*) malloc(sizeof(Player));

	player->x = h_res/2 - (jogo->ship_info.width)/2;
	player->y = v_res-(jogo->ship_info.height)*3.5;
	player->speed = 20;
	player->shot = 0;
	player->lives = 200;
	player->score = 0;
	player->alive=true;;

	return player;
}

void move_ship(Jogo* mib, Player* willsmith){

if(willsmith->alive ==false)
	return;

	if (byte == LEFT_ARROW)
	{
		if (willsmith->x + mib->ship_info.width > h_res-10)
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
			willsmith->x = 0;
		}
		else{
			willsmith->x -= willsmith->speed;
		}
	}
}

void playerDelete(Player* player){
	free(player);
	}

Alien* alienInit(Jogo* jogo){
	Alien* frank = (Alien*) malloc(sizeof(Alien));

	frank->x = h_res/2 - (jogo->alien_info.width)/2;
	frank->y = 700;
	frank->shot = 0;
	frank->speed = 0;
	frank->lives = 3;
	frank->alive = true;
	frank->right = true;
	frank->left = false;

	return frank;
}

void alienDelete(Alien* frank){
	free(frank);
}


/////////////////////////////////////////////////////////////////////////////////////
void drawJogo(Jogo* mib, Player* willsmith, Alien* frank){
	
draw_background(mib, willsmith);

move_alien(mib,frank);											//Altera posiçao do alien;

//vg_draw_xpm(mib->shield_pic, &mib->shield_info, mouseX, mouseY);


if (frank->alive)
	{
		vg_draw_xpm(mib->alien_pic, &mib->alien_info, frank->x, frank->y);
	}
	

if(willsmith->alive)
{ 
	if(protected)
	{
		printf("protected\n");
		vg_draw_xpm(mib->shield_pic, &mib->shield_info, willsmith->x, willsmith->y);
	}
	else
		vg_draw_xpm(mib->ship_pic, &mib->ship_info, willsmith->x, willsmith->y);
}


alien_fire(mib,frank,willsmith);								//Provoca disparo do alien



player_fire(mib,frank,willsmith);									//provoca disparo do Jogador





	if(explosion)
	{
		vg_draw_xpm(mib->bang_pic, &mib->bang_info, frank->x, frank->y);
	
	}

	if(ship_explosion)
	{
			vg_draw_xpm(mib->bang_pic, &mib->bang_info, willsmith->x, willsmith->y);
			frank->shot = 0;
			
	}
	
	
}



//////////////////////////////////////////////////////////////////////////////////////

void alien_shot_init(Jogo* mib, Alien* frank){
	if(frank->shot){
		alien_shotX = frank->x;
		alien_shotY = frank->y + (mib->alien_info.height);

	}
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



void check_player_fire(Jogo* jogo,Player* player)
{
	if (allowed_to_fire && !(player->shot))
	{
		player->shot = 1;
		shotX = player->x + (jogo->ship_info.width)/2 - (jogo->shot_info.width)/2;
		shotY = player->y;
	}

	allowed_to_fire = false;
}



void move_alien(Jogo* jogo, Alien* alien)
{
	if(alien->alive)
	{
		if(alien->right)
		{
			if (alien->x + jogo->alien_info.width > h_res-10)
			{
				alien->x = h_res - jogo->alien_info.width;
				alien->right = false;
				alien->left = true;
			}
			else{
				alien->x += alien->speed;
			}

		}

		else if(alien->left)
		{
			
			if (alien->x - alien->speed < 0)
			{
				alien->x = 0;
				alien->left = false;
				alien->right = true;
			}
			else{
				alien->x -= alien->speed;
			}


		}
	}

}





void alien_fire(Jogo* jogo, Alien* alien, Player* player)
{
	
	if(alien->shot && alien->alive){
		
		if ( check_colision(player->x,player->y,jogo->ship_info.width,jogo->ship_info.height) )
	{	
		printf("boom\n");
		
				if(!protected)
				{
					player->lives--;
					ship_explosion = true;
					if(player->score - 25 <= 0)
					{
						player-> score = 0;
					}
					else
					{
					player->score -= 25;
					}
				}

				protected = false;
				
		
		if(player->lives == 0)
			player->alive = false;

		
	}	
		
		
		if (alien_shotY + 70 <= v_res && !ship_explosion)
		{
			alien_shotY += 1;

			vg_draw_xpm(jogo->shot_pic, &jogo->shot_info, alien_shotX, alien_shotY);
			
			
		}

		if(alien_shotY +70 > v_res)
			alien->shot=0;




		
	}

}




void player_fire(Jogo* jogo, Alien* alien, Player* player)
{
	if (player->shot && player->alive)
	{
		int minY = alien->y;
		int minX = alien->x;
		int maxY = alien->y + (jogo->alien_info.height);
		int maxX = alien->x + (jogo->alien_info.width);
			
			if ((shotY >= minY && shotY <= maxY && shotX >= minX && shotX <= maxX && alien->alive))
		{
			alien->lives--;
			if(alien->lives ==0)
				{
					alien->alive = false;
					player->score+= 100;
				}

			player->score += 50;

			explosion = true;
		}
		
		
		if (shotY - 20 >= 0)
		{
				shotY -= 20;
				if(!explosion)
				{
					vg_draw_xpm(jogo->shot_pic, &jogo->shot_info, shotX, shotY);
				}
				if (alien->alive)
				{
					vg_draw_xpm(jogo->alien_pic, &jogo->alien_info, alien->x, alien->y);
				}
				
				if(player->alive)
				{
					if(protected)
					{
						vg_draw_xpm(jogo->shield_pic, &jogo->shield_info, player->x, player->y);
					}
					else
						vg_draw_xpm(jogo->ship_pic, &jogo->ship_info, player->x, player->y);
				}
				return;
			}
			else
				{
					player->shot = 0;
					if(player->score -10 <= 0)
					{
						player->score = 0;
					}
					else
					{
						player->score -=10;
					}
				}
		}
}


void draw_background(Jogo * jogo,Player* player)
{

vg_draw_xpm(jogo->background_pic, &jogo->background_info, 0, 0); // Desenha o background;
vg_draw_xpm(jogo->SCORE_pic, &jogo->SCORE_info, 10, 10); // Desenha o background;
show_score(jogo,player);
}



void show_score(Jogo* jogo, Player* player)
{
int unidades = (player->score % 10);
display_number(jogo,300, 0, unidades);
int dezenas = (player->score % 100);
display_number(jogo,200, 0, dezenas);
int centenas = (player->score % 1000);
display_number(jogo,100, 0, centenas);
}




void display_number(Jogo* jogo,int x, int y, int number)
{
	switch(number)
	{

	case 1:
		vg_draw_xpm(jogo->bang_pic, &jogo->bang_info, x,y);
		break;

	case 2:
		vg_draw_xpm(jogo->bang_pic, &jogo->bang_info, x,y);
		break;

	case 3:
vg_draw_xpm(jogo->bang_pic, &jogo->bang_info, x,y);
		break;

	case 4:
vg_draw_xpm(jogo->bang_pic, &jogo->bang_info, x,y);
		break;

	case 5:
vg_draw_xpm(jogo->bang_pic, &jogo->bang_info, x,y);
		break;

	case 6:
vg_draw_xpm(jogo->bang_pic, &jogo->bang_info, x,y);
		break;

	case 7:
vg_draw_xpm(jogo->bang_pic, &jogo->bang_info, x,y);
		break;

	case 8:
vg_draw_xpm(jogo->bang_pic, &jogo->bang_info, x,y);
		break;
	case 9:
vg_draw_xpm(jogo->bang_pic, &jogo->bang_info, x,y);
		break;

	case 0:
vg_draw_xpm(jogo->bang_pic, &jogo->bang_info, x,y);
		break;


	}
}


