#include <lcom/lcf.h>
#include <machine/int86.h> // /usr/src/include/arch/i386
#include "spacedef.h"
#include "menu.h"

extern int h_res, v_res;

Mouse* mouseInit(){
	Mouse* mouse = (Mouse*) malloc(sizeof(Mouse));

	mouse->x = h_res/2;
	mouse->y = v_res/2;

	mouse->click = false;
	mouse->exit = false;

	return mouse;
}

void updatePosition(Mouse* mouse, int delta_x, int delta_y){
	mouse->x += delta_x;
	mouse->y -= delta_y;

	if (mouse->x + 50 > h_res)
	{
		mouse->x = h_res - 50;
	}
	if (mouse->x < 0)
	{
		mouse->x = 0;
	}
	if (mouse->y + 50 > v_res)
	{
		mouse->y = v_res - 50;
	}
	if (mouse->y < 0)
	{
		mouse->y = 0;
	}
	if (mouse->y >= 442)
	{
		mouse->exit = true;
	}
	else{
		mouse->exit = false;
	}
}

void deleteMouse(Mouse* mouse){
	free(mouse);
}

