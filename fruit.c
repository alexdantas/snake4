/* $Id: fruit.c,v 1.4 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            fruit.c
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
#include "scull.h"
#include "fruit.h"

/**************************************************************************
 *                                                                        *
 *                       P R I V A T E    D A T A                         *
 *                                                                        *
 **************************************************************************/

#define TICK_FREQ 20
static int currTick;

#define MAX_FRUITS 10
static GameObject *fruit[MAX_FRUITS];

static int numFruits;



/**************************************************************************
 *                                                                        *
 *                   P R I V A T E    F U N C T I O N S                   *
 *                                                                        *
 **************************************************************************/

static void
fruitActivate(int n)
{
    int q;
    GameObject *go;

    go = fruit[n];
    boardEmptyRandomPos(&go->x, &go->y);
    switch (RANDOM(4)) {
      case 0:
	go->type = GO_LEMON;
	for (q = 0; q < NUM_FRUIT_PIXMAPS; q++)
	    go->pixmaps[q] = pixLemon[q];
	break;
      case 1:
	go->type = GO_BANANA;
	for (q = 0; q < NUM_FRUIT_PIXMAPS; q++)
	    go->pixmaps[q] = pixBanana[q];
	break;
      case 2:
	go->type = GO_PEAR;
	for (q = 0; q < NUM_FRUIT_PIXMAPS; q++)
	    go->pixmaps[q] = pixPear[q];
	break;
      case 3:
	go->type = GO_STRAWBERRY;
	for (q = 0; q < NUM_FRUIT_PIXMAPS; q++)
	    go->pixmaps[q] = pixStrawberry[q];
	break;
    }
    go->currPixmap = 0;
    go->tickCount = 0;
    go->ticksPixmap[0] = 300 + 70 * (MAX_LEVEL - gameLevel) + RANDOM(500);
    go->extra[0] = 0;
    goActivate(go);
    ++numFruits;
}

static void
fruitAllCallback(GameObject *go)
{
    if (go->extra[0] == 0)
	go->ticksPixmap[0] = 2 * go->ticksPixmap[1];
    if (++go->extra[0] == 7) {
	goDeactivate(go);
	scullAt(go->x, go->y);
    }
}

static void
fruitDeactivateCallback(GameObject *go)
{
    --numFruits;
}



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

void
fruitInit(void)
{
    int q, w;

    for (q = 0; q < MAX_FRUITS; q++) {
	fruit[q] = goNew(GO_LEMON);
	for (w = 0; w < NUM_FRUIT_PIXMAPS; w++)
	    goAddPixmap(fruit[q], pixLemon[w], 5);
	fruit[q]->deactivateCallback = fruitDeactivateCallback;
	fruit[q]->allPixmapsCallback = fruitAllCallback;
    }
}

void
fruitFinish(void)
{
    int q;

    for (q = 0; q < MAX_FRUITS; q++)
	goFree(fruit[q]);
}

void
fruitInitGame(void)
{
    currTick = 0;
    numFruits = 0;
}

void
fruitFinishGame(void)
{
}

void
fruitInitRound(void)
{
}

void
fruitFinishRound(void)
{
    int q;

    for (q = 0; q < MAX_FRUITS; q++)
	goDeactivate(fruit[q]);
}

void
fruitHandleTick(void)
{
    int q;

    if (++currTick < TICK_FREQ)
	return;
    currTick = 0;

    if (numFruits < 2 || RANDOM(3 + MAX_LEVEL - gameLevel) == 0) {
	for (q = 0; q < MAX_FRUITS; q++)
	    if (!fruit[q]->active)
		break;
	if (q < MAX_FRUITS)
	    fruitActivate(q);
    }
}
