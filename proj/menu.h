#ifndef _MENU_H_
#define _MENU_H_

typedef struct {
     int x, y;
     bool click, exit;
} Mouse;

Mouse* mouseInit();

void updatePosition(Mouse* mouse, int delta_x, int delta_y);

void deleteMouse(Mouse* mouse);

void reset_mouse(Mouse* mouse);

typedef enum {
	MAIN_MENU, GAME, PAUSE, OPTIONS, GAME_OVER, WON, INSTRUCTIONS, COMP
} game_st;

// typedef enum{
// 	PLAY, HOME, END
// } event_st;


#endif
