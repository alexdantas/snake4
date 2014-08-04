/* $Id: board.h,v 1.2 1996/10/11 14:45:03 sverrehu Exp $ */
#ifndef BOARD_H
#define BOARD_H

#include <X11/Xlib.h>       /* Pixmap */

#define X_BLOCKS 61
#define Y_BLOCKS 47
#define X_MIN 1
#define Y_MIN 1
#define X_MAX (X_BLOCKS - 2)
#define Y_MAX (Y_BLOCKS - 2)

#define BOARD_MAX_HIGHSCORES 20

void boardInit(void);
void boardFinish(void);
void boardInitGame(void);
void boardFinishGame(void);
void boardInitRound(void);
void boardFinishRound(void);
void boardHandleEvent(XEvent *evt);
void boardHandleTick(void);
void boardDrawBlock(Pixmap pix, int x, int y);
void boardClearBlock(int x, int y);
void boardRandomPos(int *x, int *y);
void boardEmptyRandomPos(int *x, int *y);
void boardEmptyRandomPosFarFromSnake(int *x, int *y);

extern Window  boardWin;
extern int     boardWidth, boardHeight;

#define NUM_MUSHROOM_PIXMAPS 11
#define NUM_SCULL_PIXMAPS 6
#define NUM_HEADBANGER_PIXMAPS 4
#define NUM_FRUIT_PIXMAPS 2

extern Pixmap pixSnakeHeadN, pixSnakeHeadE, pixSnakeHeadS, pixSnakeHeadW;
extern Pixmap pixSnakeBodyNW, pixSnakeBodyNS, pixSnakeBodyNE;
extern Pixmap pixSnakeBodyWE, pixSnakeBodySE, pixSnakeBodySW;
extern Pixmap pixSnakeTailN, pixSnakeTailE, pixSnakeTailS, pixSnakeTailW;
extern Pixmap pixSplat;
extern Pixmap pixMushroom[NUM_MUSHROOM_PIXMAPS];
extern Pixmap pixScull[NUM_SCULL_PIXMAPS];
extern Pixmap pixHeadbanger[NUM_HEADBANGER_PIXMAPS];
extern Pixmap pixSlimPill;
extern Pixmap pixLemon[NUM_FRUIT_PIXMAPS], pixBanana[NUM_FRUIT_PIXMAPS];
extern Pixmap pixPear[NUM_FRUIT_PIXMAPS], pixStrawberry[NUM_FRUIT_PIXMAPS];

#endif
