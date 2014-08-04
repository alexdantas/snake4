/* $Id: gameobject.c,v 1.3 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            gameobject.c
 *  MODULE OF       snake4 - game of snake eating fruit
 *
 *  DESCRIPTION     
 *
 *  WRITTEN BY      Sverre H. Huseby <shh@thathost.com>
 *
 **************************************************************************/

#include <stdlib.h>

#include <X11/Xlib.h>

#include <shhmsg.h>

#include "board.h"
#include "gameobject.h"

/**************************************************************************
 *                                                                        *
 *                       P R I V A T E    D A T A                         *
 *                                                                        *
 **************************************************************************/

static GameObject *list = NULL;



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

GameObject *
goNew(int type)
{
    GameObject *go;

    if ((go = malloc(sizeof(GameObject))) == NULL)
	msgFatal("out of memory\n");
    go->type = type;
    go->active = 0;
    go->x = go->y = 0;
    go->pixmaps = NULL;
    go->numPixmaps = 0;
    go->currPixmap = 0;
    go->ticksPixmap = NULL;
    go->tickCount = 0;
    go->allPixmapsCallback = NULL;
    go->deactivateCallback = NULL;
    /* include in list of handeled objects */
    go->next = list;
    if (list)
	list->prev = go;
    go->prev = NULL;
    list = go;
    return go;
}

void
goFree(GameObject *go)
{
    if (go->prev == NULL) {
	/* first element in list */
	list = go->next;
	if (go->next)
	    go->next->prev = NULL;
    } else {
	go->prev->next = go->next;
	if (go->next)
	    go->next->prev = go->prev;
    }
    free(go->pixmaps);
    free(go->ticksPixmap);
    free(go);
}

void
goAddPixmap(GameObject *go, Pixmap pix, int ticks)
{
    if (!go->numPixmaps) {
	go->pixmaps = malloc(sizeof(Pixmap));
	go->ticksPixmap = malloc(sizeof(int));
    } else {
	go->pixmaps
	    = realloc(go->pixmaps, (go->numPixmaps + 1) * sizeof(Pixmap));
	go->ticksPixmap
	    = realloc(go->ticksPixmap, (go->numPixmaps + 1) * sizeof(int));
    }
    if (go->pixmaps == NULL || go->ticksPixmap == NULL)
	msgFatal("out of memory\n");
    go->pixmaps[go->numPixmaps] = pix;
    go->ticksPixmap[go->numPixmaps] = ticks;
    ++go->numPixmaps;
}

void
goHandleTick(void)
{
    GameObject *go;

    go = list;
    while (go) {
	if (go->active && go->numPixmaps > 1) {
	    if (++go->tickCount >= go->ticksPixmap[go->currPixmap]) {
		go->tickCount = 0;
		if (++go->currPixmap >= go->numPixmaps) {
		    go->currPixmap = 0;
		    if (go->allPixmapsCallback)
			go->allPixmapsCallback(go);
		}
		if (go->active) { /* may have been changed by callback */
		    boardClearBlock(go->x, go->y);
		    boardDrawBlock(go->pixmaps[go->currPixmap], go->x, go->y);
		}
	    }
	}
	go = go->next;
    }
}

void
goMove(GameObject *go, int x, int y)
{
    int oldx, oldy;
    GameObject *go2;

    oldx = go->x;
    oldy = go->y;
    go->x = x;
    go->y = y;
    if (go->active) {
	if ((go2 = goGetObjectAt(oldx, oldy)) != NULL)
	    boardDrawBlock(go2->pixmaps[go2->currPixmap], go2->x, go2->y);
	else
	    boardClearBlock(oldx, oldy);
	boardDrawBlock(go->pixmaps[go->currPixmap], go->x, go->y);
    }
}

void
goActivate(GameObject *go)
{
    if (go->active)
	return;
    go->active = 1;
    goDraw(go);
}

void
goDeactivate(GameObject *go)
{
    if (!go->active)
	return;
    go->active = 0;
    if (go->deactivateCallback)
	go->deactivateCallback(go);
    goDraw(go);
}

void
goDraw(GameObject *go)
{
    GameObject *go2;

    if (go->active)
	boardDrawBlock(go->pixmaps[go->currPixmap], go->x, go->y);
    else {
	if ((go2 = goGetObjectAt(go->x, go->y)) != NULL)
	    boardDrawBlock(go2->pixmaps[go2->currPixmap], go2->x, go2->y);
	else
	    boardClearBlock(go->x, go->y);
    }
}

void
goDrawAll(void)
{
    GameObject *go;

    go = list;
    while (go) {
	if (go->active)
	    boardDrawBlock(go->pixmaps[go->currPixmap], go->x, go->y);
	go = go->next;
    }
}

GameObject *
goGetObjectAt(int x, int y)
{
    GameObject *go;

    go = list;
    while (go) {
	if (go->active && go->x == x && go->y == y)
	    return go;
	go = go->next;
    }
    return NULL;
}
