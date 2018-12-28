#ifndef _MENU_H_
#define _MENU_H_

typedef struct {
     int x, y;
} Mouse;

Mouse* mouseInit();

void updatePosition(Mouse* mouse, int delta_x, int delta_y);

void deleteMouse(Mouse* mouse);

typedef enum {
	MAIN_MENU, GAME, COMP
} game_st;

// typedef enum{
// 	PLAY, HOME, END
// } event_st;


#endif
