/* $Id: slimpill.c,v 1.4 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            slimpill.c
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
#include "slimpill.h"

/**************************************************************************
 *                                                                        *
 *                       P R I V A T E    D A T A                         *
 *                                                                        *
 **************************************************************************/

static int tickFreq;
static int currTick;
static int ticksToGo;

static GameObject *slimPill;



/**************************************************************************
 *                                                                        *
 *                   P R I V A T E    F U N C T I O N S                   *
 *                                                                        *
 **************************************************************************/

static void
pillActivate(void)
{
    boardEmptyRandomPos(&slimPill->x, &slimPill->y);
    slimPill->extra[0] = RANDOM(2) ? -1 : 1;
    slimPill->extra[1] = RANDOM(2) ? -1 : 1;
    goActivate(slimPill);
}

static int
pillPosOk(int x, int y)
{
    GameObject *go;

    if (x < X_MIN || x > X_MAX || y < Y_MIN || y > Y_MAX)
	return 0;
    if ((go = goGetObjectAt(x, y)) == NULL)
	return 1;
    if (go->type != GO_SLIMPILL)
	return 0;
    return 1;
}

static void
pillMove(void)
{
    int x, y, ox, oy, px, py;

    x = ox = slimPill->x;
    y = oy = slimPill->y;
    px = slimPill->extra[0];
    py = slimPill->extra[1];
    x += px;
    y += py;
    if (!pillPosOk(x, y)) {
	/* check if turning x-direction fixes it */
	x = ox - px;
	y = oy + py;
	if (pillPosOk(x, y)) {
	    px = -px;
	    goto posfound;
	}
	/* check if turning y-direction fixes it */
	x = ox + px;
	y = oy - py;
	if (pillPosOk(x, y)) {
	    py = -py;
	    goto posfound;
	}
	/* check if turning both directions fixes it */
	x = ox - px;
	y = oy - py;
	if (pillPosOk(x, y)) {
	    px = -px;
	    py = -py;
	    goto posfound;
	}
	/* well, nothing worked, so we just keep the old position. */
	x = ox;
	y = oy;
    }
  posfound:
    slimPill->extra[0] = px;
    slimPill->extra[1] = py;
    goMove(slimPill, x, y);
}



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

void
pillInit(void)
{
    slimPill = goNew(GO_SLIMPILL);
    goAddPixmap(slimPill, pixSlimPill, 0);
}

void
pillFinish(void)
{
    goFree(slimPill);
}

void
pillInitGame(void)
{
    tickFreq = MAX_LEVEL - gameLevel + 3 + RANDOM(3);
    currTick = 0;
}

void
pillFinishGame(void)
{
}

void
pillInitRound(void)
{
    ticksToGo = 250 + RANDOM(400);
}

void
pillFinishRound(void)
{
    goDeactivate(slimPill);
}

void
pillHandleTick(void)
{
    if (++currTick < tickFreq)
	return;
    currTick = 0;

    if (slimPill->active) {
	if (--ticksToGo <= 0) {
	    goDeactivate(slimPill);
	    ticksToGo = 250 + RANDOM(400);
	} else
	    pillMove();
    } else {
	if (--ticksToGo <= 0) {
	    ticksToGo = 50 + RANDOM(250);
	    pillActivate();
	}
    }
}
