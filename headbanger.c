/* $Id: headbanger.c,v 1.4 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            headbanger.c
 *  MODULE OF       snake4 - game of snake eating fruit
 *
 *  DESCRIPTION     
 *
 *  WRITTEN BY      Sverre H. Huseby <shh@thathost.com>
 *
 **************************************************************************/

#include <stdlib.h>

#include <shhmsg.h>

#include "gameobject.h"
#include "game.h"
#include "board.h"
#include "snake.h"
#include "headbanger.h"

/**************************************************************************
 *                                                                        *
 *                       P R I V A T E    D A T A                         *
 *                                                                        *
 **************************************************************************/

static int tickFreq;
static int currTick;
static int ticksToGo;

static GameObject *headbanger;



/**************************************************************************
 *                                                                        *
 *                   P R I V A T E    F U N C T I O N S                   *
 *                                                                        *
 **************************************************************************/

static void
headbangerActivate(void)
{
    boardEmptyRandomPosFarFromSnake(&headbanger->x, &headbanger->y);
    goActivate(headbanger);
}

static int
headbangerPosOk(int x, int y)
{
    GameObject *go;
    int sx, sy;

    if (x < X_MIN || x > X_MAX || y < Y_MIN || y > Y_MAX)
	return 0;
    snakeGetHeadPos(&sx, &sy);
    if (x == sx && y == sy)
	return 1;
    if ((go = goGetObjectAt(x, y)) == NULL)
	return 1;
    if (go->type != GO_HEADBANGER)
	return 0;
    return 1;
}

static void
headbangerMove(void)
{
    int x, y, ox, oy, px, py, sx, sy;

    x = ox = headbanger->x;
    y = oy = headbanger->y;
    snakeGetHeadPos(&sx, &sy);
    if (sx < x)
	px = -1;
    else if (sx == x)
	px = 0;
    else
	px = 1;
    if (sy < y)
	py = -1;
    else if (sy == y)
	py = 0;
    else
	py = 1;
    x += px;
    y += py;
    if (!headbangerPosOk(x, y)) {
	if (px && py) {
	    /* check if moving in x-direction only fixes it */
	    x = ox + px;
	    y = oy;
	    if (headbangerPosOk(x, y))
		goto posfound;
	    /* check if moving in y-direction only fixes it */
	    x = ox;
	    y = oy + py;
	    if (headbangerPosOk(x, y))
		goto posfound;
	}
	/* well, nothing worked, so we just keep the old position. */
	x = ox;
	y = oy;
    }
  posfound:
    goMove(headbanger, x, y);
    if (x == sx && y == sy)
	snakeDie();
    else
	gameAddScore(SCORE_HEADBANGER_MOVE);
}



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

void
headbangerInit(void)
{
    int q;

    headbanger = goNew(GO_HEADBANGER);
    for (q = 0; q < NUM_HEADBANGER_PIXMAPS; q++)
	goAddPixmap(headbanger, pixHeadbanger[q], 5);
}

void
headbangerFinish(void)
{
    goFree(headbanger);
}

void
headbangerInitGame(void)
{
    tickFreq = MAX_LEVEL - gameLevel + 3 + RANDOM(3);
    currTick = 0;
}

void
headbangerFinishGame(void)
{
}

void
headbangerInitRound(void)
{
    ticksToGo = 200 + RANDOM(500);
}

void
headbangerFinishRound(void)
{
    goDeactivate(headbanger);
}

void
headbangerHandleTick(void)
{
    if (++currTick < tickFreq)
	return;
    currTick = 0;

    if (headbanger->active) {
	if (--ticksToGo <= 0) {
	    goDeactivate(headbanger);
	    gameAddScore(SCORE_HEADBANGER_LEAVE);
	    ticksToGo = 200 + RANDOM(500);
	} else
	    headbangerMove();
    } else {
	if (--ticksToGo <= 0) {
	    ticksToGo = 50 + RANDOM(200);
	    headbangerActivate();
	}
    }
}
