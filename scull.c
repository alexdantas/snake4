/* $Id: scull.c,v 1.3 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            scull.c
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
#include "board.h"
#include "scull.h"

/**************************************************************************
 *                                                                        *
 *                       P R I V A T E    D A T A                         *
 *                                                                        *
 **************************************************************************/

#define MAX_SCULLS ((X_MAX - X_MIN + 1) * (Y_MAX - Y_MIN + 1))
static GameObject *scull[MAX_SCULLS];



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

void
scullInit(void)
{
    int q, w;

    for (q = 0; q < MAX_SCULLS; q++) {
	scull[q] = goNew(GO_SCULL);
	for (w = 0; w < NUM_SCULL_PIXMAPS; w++)
	    goAddPixmap(scull[q], pixScull[w], (w == 0) ? 25 : 4);
    }
}

void
scullFinish(void)
{
    int q;

    for (q = 0; q < MAX_SCULLS; q++)
	goFree(scull[q]);
}

void
scullInitGame(void)
{
}

void
scullFinishGame(void)
{
    int q;

    for (q = 0; q < MAX_SCULLS; q++)
	goDeactivate(scull[q]);
}

void
scullInitRound(void)
{
}

void
scullFinishRound(void)
{
}

void
scullHandleTick(void)
{
}

void
scullAt(int x, int y)
{
    int q;

    for (q = 0; q < MAX_SCULLS; q++)
	if (!scull[q]->active)
	    break;
    if (q >= MAX_SCULLS)
	return;
    scull[q]->x = x;
    scull[q]->y = y;
    scull[q]->currPixmap = 0;
    scull[q]->tickCount = 0;
    goActivate(scull[q]);
}
