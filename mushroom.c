/* $Id: mushroom.c,v 1.3 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            mushroom.c
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
#include "mushroom.h"

/**************************************************************************
 *                                                                        *
 *                       P R I V A T E    D A T A                         *
 *                                                                        *
 **************************************************************************/

#define TICK_FREQ 20
static int currTick;

#define MAX_MUSHROOMS 30
static GameObject *mushroom[MAX_MUSHROOMS];



/**************************************************************************
 *                                                                        *
 *                   P R I V A T E    F U N C T I O N S                   *
 *                                                                        *
 **************************************************************************/

static void
finishCallback(GameObject *ob)
{
    goDeactivate(ob);
}

static void
mushActivate(int n)
{
    GameObject *go;

    go = mushroom[n];
    boardEmptyRandomPosFarFromSnake(&go->x, &go->y);
    go->ticksPixmap[NUM_MUSHROOM_PIXMAPS / 2] = 100 + RANDOM(4000);
    go->currPixmap = 0;
    go->tickCount = 0;
    goActivate(go);
}



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

void
mushInit(void)
{
    int q, w;

    for (q = 0; q < MAX_MUSHROOMS; q++) {
	mushroom[q] = goNew(GO_MUSHROOM);
	for (w = 0; w < NUM_MUSHROOM_PIXMAPS; w++)
	    goAddPixmap(mushroom[q], pixMushroom[w], 2);
	mushroom[q]->allPixmapsCallback = finishCallback;
    }
}

void
mushFinish(void)
{
    int q;

    for (q = 0; q < MAX_MUSHROOMS; q++)
	goFree(mushroom[q]);
}

void
mushInitGame(void)
{
    currTick = 0;
}

void
mushFinishGame(void)
{
    int q;

    for (q = 0; q < MAX_MUSHROOMS; q++)
	goDeactivate(mushroom[q]);
}

void
mushInitRound(void)
{
}

void
mushFinishRound(void)
{
}

void
mushHandleTick(void)
{
    int q;

    if (++currTick < TICK_FREQ)
	return;
    currTick = 0;

    if (RANDOM(15) == 0) {
	for (q = 0; q < MAX_MUSHROOMS; q++)
	    if (!mushroom[q]->active)
		break;
	if (q < MAX_MUSHROOMS)
	    mushActivate(q);
    }
}
