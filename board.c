/* $Id: board.c,v 1.7 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            board.c
 *  MODULE OF       snake4 - game of snake eating fruit
 *
 *  DESCRIPTION
 *
 *  WRITTEN BY      Sverre H. Huseby <shh@thathost.com>
 *
 **************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/keysymdef.h>

#include <shhmsg.h>

#include "win.h"
#include "gameobject.h"
#include "game.h"
#include "snake.h"
#include "mushroom.h"
#include "scull.h"
#include "headbanger.h"
#include "slimpill.h"
#include "fruit.h"
#include "score.h"
#include "board.h"

/* must be last to avoid redefinition of Pixel */
#include <X11/xpm.h>



/**************************************************************************
 *                                                                        *
 *                       P R I V A T E    D A T A                         *
 *                                                                        *
 **************************************************************************/

#define BLOCK_WIDTH 12
#define BLOCK_HEIGHT 12

/* all pixmaps are static */
#include "pixmaps/instructions"
#include "pixmaps/snakeheadN"
#include "pixmaps/snakeheadE"
#include "pixmaps/snakeheadS"
#include "pixmaps/snakeheadW"
#include "pixmaps/snakebodyNW"
#include "pixmaps/snakebodyNS"
#include "pixmaps/snakebodyNE"
#include "pixmaps/snakebodyWE"
#include "pixmaps/snakebodySE"
#include "pixmaps/snakebodySW"
#include "pixmaps/snaketailN"
#include "pixmaps/snaketailE"
#include "pixmaps/snaketailS"
#include "pixmaps/snaketailW"
#include "pixmaps/splat"
#include "pixmaps/borderNW"
#include "pixmaps/borderN"
#include "pixmaps/borderNE"
#include "pixmaps/borderE"
#include "pixmaps/borderSE"
#include "pixmaps/borderS"
#include "pixmaps/borderSW"
#include "pixmaps/borderW"
#include "pixmaps/mushroom01"
#include "pixmaps/mushroom02"
#include "pixmaps/mushroom03"
#include "pixmaps/mushroom04"
#include "pixmaps/mushroom05"
#include "pixmaps/mushroom06"
#include "pixmaps/mushroom07"
#include "pixmaps/mushroom08"
#include "pixmaps/mushroom09"
#include "pixmaps/mushroom10"
#include "pixmaps/mushroom11"
#include "pixmaps/scull01"
#include "pixmaps/scull02"
#include "pixmaps/scull03"
#include "pixmaps/scull04"
#include "pixmaps/scull05"
#include "pixmaps/scull06"
#include "pixmaps/headbanger01"
#include "pixmaps/headbanger02"
#include "pixmaps/headbanger03"
#include "pixmaps/headbanger04"
#include "pixmaps/slimpill"
#include "pixmaps/lemon01"
#include "pixmaps/lemon02"
#include "pixmaps/banana01"
#include "pixmaps/banana02"
#include "pixmaps/pear01"
#include "pixmaps/pear02"
#include "pixmaps/strawberry01"
#include "pixmaps/strawberry02"

static GC boardGC;
static Pixmap pixInstructions;
static XpmAttributes attrInstructions;
static Pixmap pixBorderNW, pixBorderN, pixBorderNE, pixBorderE;
static Pixmap pixBorderSE, pixBorderS, pixBorderSW, pixBorderW;



/**************************************************************************
 *                                                                        *
 *                        P U B L I C    D A T A                          *
 *                                                                        *
 **************************************************************************/

Window  boardWin;
int     boardWidth = (BLOCK_WIDTH * X_BLOCKS);
int     boardHeight = (BLOCK_HEIGHT * Y_BLOCKS);

Pixmap pixSnakeHeadN, pixSnakeHeadE, pixSnakeHeadS, pixSnakeHeadW;
Pixmap pixSnakeBodyNW, pixSnakeBodyNS, pixSnakeBodyNE;
Pixmap pixSnakeBodyWE, pixSnakeBodySE, pixSnakeBodySW;
Pixmap pixSnakeTailN, pixSnakeTailE, pixSnakeTailS, pixSnakeTailW;
Pixmap pixSplat;
Pixmap pixMushroom[NUM_MUSHROOM_PIXMAPS];
Pixmap pixScull[NUM_SCULL_PIXMAPS];
Pixmap pixHeadbanger[NUM_HEADBANGER_PIXMAPS];
Pixmap pixSlimPill;
Pixmap pixLemon[NUM_FRUIT_PIXMAPS], pixBanana[NUM_FRUIT_PIXMAPS];
Pixmap pixPear[NUM_FRUIT_PIXMAPS], pixStrawberry[NUM_FRUIT_PIXMAPS];



/**************************************************************************
 *                                                                        *
 *                   P R I V A T E    F U N C T I O N S                   *
 *                                                                        *
 **************************************************************************/

static void
createPixmap(char **data, Pixmap *pix, XpmAttributes *attr)
{
    int err;
    XpmAttributes xa;

    if (attr == NULL)
	attr = &xa;
    xa.valuemask = 0;
    attr->closeness = 40000;
    attr->valuemask |= XpmCloseness;
    err = XpmCreatePixmapFromData(winDisplay, boardWin, data, pix, NULL, attr);
    if (err == XpmSuccess)
	return;
    else if (err > 0)
	msgError("warning: %s\n", XpmGetErrorString(err));
    msgFatal("Xpm: %s\n", XpmGetErrorString(err));
}

static void
setupPixmaps(void)
{
    XGCValues values;
    XColor xc, xc2;

    /* make sure we don't get NoExpose for each XCopyArea */
    values.graphics_exposures = False;
    XAllocNamedColor(winDisplay,
		     DefaultColormap(winDisplay, DefaultScreen(winDisplay)),
		     "light gray", &xc, &xc2);
    values.foreground = xc.pixel;
    XAllocNamedColor(winDisplay,
		     DefaultColormap(winDisplay, DefaultScreen(winDisplay)),
		     "black", &xc, &xc2);
    values.background = xc.pixel;
    values.font = XLoadFont(winDisplay, "6x10");
    boardGC = XCreateGC(winDisplay, boardWin,
			GCGraphicsExposures | GCForeground
			| GCBackground | GCFont, &values);

    attrInstructions.valuemask = 0;
    createPixmap(instructions, &pixInstructions, &attrInstructions);
    createPixmap(snakeheadN, &pixSnakeHeadN, NULL);
    createPixmap(snakeheadE, &pixSnakeHeadE, NULL);
    createPixmap(snakeheadS, &pixSnakeHeadS, NULL);
    createPixmap(snakeheadW, &pixSnakeHeadW, NULL);
    createPixmap(snakebodyNW, &pixSnakeBodyNW, NULL);
    createPixmap(snakebodyNS, &pixSnakeBodyNS, NULL);
    createPixmap(snakebodyNE, &pixSnakeBodyNE, NULL);
    createPixmap(snakebodyWE, &pixSnakeBodyWE, NULL);
    createPixmap(snakebodySE, &pixSnakeBodySE, NULL);
    createPixmap(snakebodySW, &pixSnakeBodySW, NULL);
    createPixmap(snaketailN, &pixSnakeTailN, NULL);
    createPixmap(snaketailE, &pixSnakeTailE, NULL);
    createPixmap(snaketailS, &pixSnakeTailS, NULL);
    createPixmap(snaketailW, &pixSnakeTailW, NULL);
    createPixmap(splat, &pixSplat, NULL);
    createPixmap(borderNW, &pixBorderNW, NULL);
    createPixmap(borderN, &pixBorderN, NULL);
    createPixmap(borderNE, &pixBorderNE, NULL);
    createPixmap(borderE, &pixBorderE, NULL);
    createPixmap(borderSE, &pixBorderSE, NULL);
    createPixmap(borderS, &pixBorderS, NULL);
    createPixmap(borderSW, &pixBorderSW, NULL);
    createPixmap(borderW, &pixBorderW, NULL);
    createPixmap(borderW, &pixBorderW, NULL);
    createPixmap(mushroom01, &pixMushroom[0], NULL);
    createPixmap(mushroom02, &pixMushroom[1], NULL);
    createPixmap(mushroom03, &pixMushroom[2], NULL);
    createPixmap(mushroom04, &pixMushroom[3], NULL);
    createPixmap(mushroom05, &pixMushroom[4], NULL);
    createPixmap(mushroom06, &pixMushroom[5], NULL);
    createPixmap(mushroom07, &pixMushroom[6], NULL);
    createPixmap(mushroom08, &pixMushroom[7], NULL);
    createPixmap(mushroom09, &pixMushroom[8], NULL);
    createPixmap(mushroom10, &pixMushroom[9], NULL);
    createPixmap(mushroom11, &pixMushroom[10], NULL);
    createPixmap(scull01, &pixScull[0], NULL);
    createPixmap(scull02, &pixScull[1], NULL);
    createPixmap(scull03, &pixScull[2], NULL);
    createPixmap(scull04, &pixScull[3], NULL);
    createPixmap(scull05, &pixScull[4], NULL);
    createPixmap(scull06, &pixScull[5], NULL);
    createPixmap(headbanger01, &pixHeadbanger[0], NULL);
    createPixmap(headbanger02, &pixHeadbanger[1], NULL);
    createPixmap(headbanger03, &pixHeadbanger[2], NULL);
    createPixmap(headbanger04, &pixHeadbanger[3], NULL);
    createPixmap(slimpill, &pixSlimPill, NULL);
    createPixmap(lemon01, &pixLemon[0], NULL);
    createPixmap(lemon02, &pixLemon[1], NULL);
    createPixmap(banana01, &pixBanana[0], NULL);
    createPixmap(banana02, &pixBanana[1], NULL);
    createPixmap(pear01, &pixPear[0], NULL);
    createPixmap(pear02, &pixPear[1], NULL);
    createPixmap(strawberry01, &pixStrawberry[0], NULL);
    createPixmap(strawberry02, &pixStrawberry[1], NULL);
}

static void
freePixmaps(void)
{
    int q;

    XFreePixmap(winDisplay, pixInstructions);
    XpmFreeAttributes(&attrInstructions);
    XFreePixmap(winDisplay, pixSnakeHeadN);
    XFreePixmap(winDisplay, pixSnakeHeadE);
    XFreePixmap(winDisplay, pixSnakeHeadS);
    XFreePixmap(winDisplay, pixSnakeHeadW);
    XFreePixmap(winDisplay, pixSnakeBodyNW);
    XFreePixmap(winDisplay, pixSnakeBodyNS);
    XFreePixmap(winDisplay, pixSnakeBodyNE);
    XFreePixmap(winDisplay, pixSnakeBodyWE);
    XFreePixmap(winDisplay, pixSnakeBodySE);
    XFreePixmap(winDisplay, pixSnakeBodySW);
    XFreePixmap(winDisplay, pixSnakeTailN);
    XFreePixmap(winDisplay, pixSnakeTailE);
    XFreePixmap(winDisplay, pixSnakeTailS);
    XFreePixmap(winDisplay, pixSnakeTailW);
    XFreePixmap(winDisplay, pixSplat);
    XFreePixmap(winDisplay, pixBorderNW);
    XFreePixmap(winDisplay, pixBorderN);
    XFreePixmap(winDisplay, pixBorderNE);
    XFreePixmap(winDisplay, pixBorderE);
    XFreePixmap(winDisplay, pixBorderSW);
    XFreePixmap(winDisplay, pixBorderS);
    XFreePixmap(winDisplay, pixBorderSW);
    XFreePixmap(winDisplay, pixBorderW);
    for (q = 0; q < NUM_MUSHROOM_PIXMAPS; q++)
	XFreePixmap(winDisplay, pixMushroom[q]);
    for (q = 0; q < NUM_SCULL_PIXMAPS; q++)
	XFreePixmap(winDisplay, pixScull[q]);
    for (q = 0; q < NUM_HEADBANGER_PIXMAPS; q++)
	XFreePixmap(winDisplay, pixHeadbanger[q]);
    XFreePixmap(winDisplay, pixSlimPill);
    for (q = 0; q < NUM_FRUIT_PIXMAPS; q++) {
	XFreePixmap(winDisplay, pixLemon[q]);
	XFreePixmap(winDisplay, pixBanana[q]);
	XFreePixmap(winDisplay, pixPear[q]);
	XFreePixmap(winDisplay, pixStrawberry[q]);
    }
    XFreeGC(winDisplay, boardGC);
}

static void
boardDraw(void)
{
    int q;

    boardDrawBlock(pixBorderNW, 0, 0);
    boardDrawBlock(pixBorderNE, X_BLOCKS - 1, 0);
    boardDrawBlock(pixBorderSE, X_BLOCKS - 1, Y_BLOCKS - 1);
    boardDrawBlock(pixBorderSW, 0, Y_BLOCKS - 1);
    for (q = 1; q < X_BLOCKS - 1; q++) {
	boardDrawBlock(pixBorderN, q, 0);
	boardDrawBlock(pixBorderS, q, Y_BLOCKS - 1);
    }
    for (q = 1; q < Y_BLOCKS - 1; q++) {
	boardDrawBlock(pixBorderW, 0, q);
	boardDrawBlock(pixBorderE, X_BLOCKS - 1, q);
    }
}

static Bool
boardIsKeyPressEvent(Display *display, XEvent *xev, char *data)
{
    return xev->type == KeyPress;
}

static void
boardFlushKeyboard(void)
{
    XEvent xev;

    winFlush();
    while (XCheckIfEvent(winDisplay, &xev, boardIsKeyPressEvent, NULL))
        ;
    snakeFlushKeys();
}

static void
boardShowInstructions(void)
{
    int w, h, bw;

    w = attrInstructions.width;
    h = attrInstructions.height;
    bw = (X_BLOCKS - 2) * BLOCK_WIDTH;

    XCopyArea(winDisplay, pixInstructions, boardWin, boardGC,
	      0, 0, w, h, BLOCK_WIDTH + (bw - w) / 2, BLOCK_HEIGHT);
}

static void
boardDrawString(int x, int y, char *s)
{
    XDrawString(winDisplay, boardWin, boardGC, x, y, s, strlen(s));
}

static void
boardShowHighscores(void)
{
    int q, y, y0, x0, ch, cw, len, thisPlayer, from, to, eachSide;
    char *s;

    XClearArea(winDisplay, boardWin,
	       BLOCK_WIDTH, BLOCK_HEIGHT + attrInstructions.height,
	       (X_BLOCKS - 2) * BLOCK_WIDTH,
	       boardHeight - 2 * BLOCK_HEIGHT - attrInstructions.height,
	       False);

    len = strlen(scoreGetHeadSepStr()) + 3;
    cw = 6;
    ch = 12;
    x0 = (boardWidth - cw * len) / 2;
    y0 = BLOCK_HEIGHT + attrInstructions.height;
    thisPlayer = scoreGetThisPlayerIndex();
    if (thisPlayer < BOARD_MAX_HIGHSCORES)
	to = BOARD_MAX_HIGHSCORES;
    else
	to = BOARD_MAX_HIGHSCORES / 2;
    y = y0 + ch;
    boardDrawString(x0 + cw * 3, y, scoreGetHeadStr());
    y += ch;
    boardDrawString(x0 + cw * 3, y, scoreGetHeadSepStr());
    y += ch;
    for (q = 0; q < to; q++) {
	if (q == thisPlayer)
	    boardDrawString(x0, y, "->");
	boardDrawString(x0 + cw * 3, y, scoreGetEntryStr(q));
	y += ch;
    }
    if (thisPlayer >= BOARD_MAX_HIGHSCORES) {
	boardDrawString(x0 + cw * 3, y, "  :");
	y += ch;
	eachSide = (BOARD_MAX_HIGHSCORES / 2 - 2) / 2;
	from = thisPlayer - eachSide;
	while (from < to) /* to is from the above loop */
	    ++from;
	to = from + 2 * eachSide + 1;
	for (q = from; q < to; q++) {
	    if ((s = scoreGetEntryStr(q)) == NULL)
		break;
	    if (q == thisPlayer)
		boardDrawString(x0, y, "->");
	    boardDrawString(x0 + cw * 3, y, s);
	    y += ch;
	}
    }
}



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

void
boardInit(void)
{
    XSelectInput(winDisplay, boardWin, ExposureMask | KeyPressMask);
}

void
boardFinish(void)
{
    freePixmaps();
}

void
boardInitGame(void)
{
    XClearArea(winDisplay, boardWin,
	       BLOCK_WIDTH, BLOCK_HEIGHT,
	       (X_BLOCKS - 2) * BLOCK_WIDTH, (Y_BLOCKS - 2) * BLOCK_HEIGHT,
	       False);
}

void
boardFinishGame(void)
{
    boardDraw();
    boardShowInstructions();
    boardShowHighscores();
}

void
boardInitRound(void)
{
    boardDraw();
    boardFlushKeyboard();
}

void
boardFinishRound(void)
{
}

void
boardHandleEvent(XEvent *evt)
{
    static int setupDone = 0;
    KeySym* ks;

    switch (evt->type) {
      case Expose:
	if (evt->xexpose.count)
	    break;
	if (!setupDone) {
	    setupPixmaps();
	    snakeInit();
	    mushInit();
	    scullInit();
	    headbangerInit();
	    pillInit();
	    fruitInit();
	    winShowHighscore();
	    setupDone = 1;
	}
	boardDraw();
	if (gameRunning)
	    goDrawAll();
	else {
	    boardShowInstructions();
	    boardShowHighscores();
	}
	break;
      case KeyPress:
      {
	int keysymsPerKeycode;
	ks = XGetKeyboardMapping(winDisplay, evt->xkey.keycode, 1, &keysymsPerKeycode);
	switch (*ks) {
	  case XK_Up:
	  case XK_A:
	  case XK_a:
	  case XK_T:
	  case XK_t:
	  case XK_K:
	  case XK_k:
	    snakeTurnNorth();
	    break;
	  case XK_Right:
	  case XK_M:
	  case XK_m:
	  case XK_G:
	  case XK_g:
	  case XK_L:
	  case XK_l:
	    snakeTurnEast();
	    break;
	  case XK_Down:
	  case XK_Z:
	  case XK_z:
	  case XK_V:
	  case XK_v:
	  case XK_J:
	  case XK_j:
	    snakeTurnSouth();
	    break;
	  case XK_Left:
	  case XK_N:
	  case XK_n:
	  case XK_F:
	  case XK_f:
	  case XK_H:
	  case XK_h:
	    snakeTurnWest();
	    break;
	  case XK_space:
	  case XK_Pause:
	  case XK_P:
	  case XK_p:
	    winTogglePause();
	    break;
	  case XK_Escape:
	  case XK_Q:
	  case XK_q:
	    gameExit = 1;
	    break;
          /* choosing level */
	  case XK_1:
	  case XK_KP_1:
	    winChooseLevel(1);
	    break;
	  case XK_2:
	  case XK_KP_2:
	    winChooseLevel(2);
	    break;
	  case XK_3:
	  case XK_KP_3:
	    winChooseLevel(3);
	    break;
	  case XK_4:
	  case XK_KP_4:
	    winChooseLevel(4);
	    break;
	  case XK_5:
	  case XK_KP_5:
	    winChooseLevel(5);
	    break;
	}
	XFree(ks);
	break;
	}
    }
}

void
boardHandleTick(void)
{
}

void
boardDrawBlock(Pixmap pix, int x, int y)
{
    XCopyArea(winDisplay, pix, boardWin, boardGC,
	      0, 0, BLOCK_WIDTH, BLOCK_HEIGHT,
	      x * BLOCK_WIDTH, y * BLOCK_HEIGHT);
}

void
boardClearBlock(int x, int y)
{
    XClearArea(winDisplay, boardWin,
	       x * BLOCK_WIDTH, y * BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT,
	       False);

}

void
boardRandomPos(int *x, int *y)
{
    *x = RANDOM(1 + X_MAX - X_MIN) + X_MIN;
    *y = RANDOM(1 + Y_MAX - Y_MIN) + Y_MIN;
}

void
boardEmptyRandomPos(int *x, int *y)
{
    do {
	boardRandomPos(x, y);
    } while (goGetObjectAt(*x, *y) != NULL);
}

void
boardEmptyRandomPosFarFromSnake(int *x, int *y)
{
    int sx, sy, dx, dy, distsquared;

    snakeGetHeadPos(&sx, &sy);
    do {
	boardRandomPos(x, y);
	dx = *x - sx;
	dy = *y - sy;
	distsquared = dx * dx + dy * dy;
    } while (goGetObjectAt(*x, *y) != NULL || distsquared < 150);
}
