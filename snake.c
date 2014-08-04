/* $Id: snake.c,v 1.4 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            snake.c
 *  MODULE OF       snake4 - game of snake eating fruit
 *
 *  DESCRIPTION     
 *
 *  WRITTEN BY      Sverre H. Huseby <shh@thathost.com>
 *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <shhmsg.h>

#include "gameobject.h"
#include "game.h"
#include "win.h"
#include "board.h"
#include "snake.h"

/**************************************************************************
 *                                                                        *
 *                       P R I V A T E    D A T A                         *
 *                                                                        *
 **************************************************************************/

static int tickFreq;
static int currTick;

static int maxIdleTick;
static int idleTick;

#define MAX_SNAKE_LEN ((X_MAX - X_MIN + 1) * (Y_MAX - Y_MIN + 1))
static GameObject *snake[MAX_SNAKE_LEN];
static int snakeHead, snakeTail, snakeLen;
static int moving;
static int xInc, yInc;
static int xIncNew, yIncNew, newInc;
static int digest;

#define KEY_BUF_SIZE 10
static int keybuf[KEY_BUF_SIZE];
static int keyIn, keyOut, keyLen;
enum {
    KEY_NORTH = 1,
    KEY_EAST,
    KEY_SOUTH,
    KEY_WEST
};



/**************************************************************************
 *                                                                        *
 *                   P R I V A T E    F U N C T I O N S                   *
 *                                                                        *
 **************************************************************************/

static void
keyPut(int key)
{
    if (keyLen == KEY_BUF_SIZE)
	return;
    /* don't buffer multiple identical keys */
    if (keyLen && keybuf[keyOut] == key)
	return;
    keybuf[keyIn] = key;
    if (++keyIn == KEY_BUF_SIZE)
	keyIn = 0;
    ++keyLen;
}

static int
keyGet(void)
{
    int ret;

    if (!keyLen)
	return -1;
    ret = keybuf[keyOut];
    if (++keyOut == KEY_BUF_SIZE)
	keyOut = 0;
    --keyLen;
    return ret;
}

static void
snakeHandleTurn(void)
{
    int key;

    if (newInc || (key = keyGet()) < 0)
	return;
    switch (key) {
      case KEY_NORTH:
	if (yInc == 1)
	    return;
	xIncNew = 0;
	yIncNew = -1;
	break;
      case KEY_EAST:
	if (xInc == -1)
	    return;
	xIncNew = 1;
	yIncNew = 0;
	break;
      case KEY_SOUTH:
	if (yInc == -1)
	    return;
	xIncNew = 0;
	yIncNew = 1;
	break;
      case KEY_WEST:
	if (xInc == 1)
	    return;
	xIncNew = -1;
	yIncNew = 0;
	break;
    }
    newInc = 1;
    moving = 1;
}

/* returns GameObject on head position, or NULL if none. */
static GameObject *
snakeMoveHead(void)
{
    int    old;
    Pixmap pix, oldHeadPix;
    GameObject *go;

    /* find next position, and determine what body part should
       overwrite the old head */
    if (newInc && xIncNew != xInc && yIncNew != yInc) {
	if (xInc == -1) {        /* yInc == 0 */
	    if (yIncNew == -1)   /* xIncNew == 0 */
		oldHeadPix = pixSnakeBodyNE;
	    else                 /* yIncNew == 1 */
		oldHeadPix = pixSnakeBodySE;
	} else if (xInc == 1) {  /* yInc == 0 */
	    if (yIncNew == -1)   /* xIncNew == 0 */
		oldHeadPix = pixSnakeBodyNW;
	    else                 /* yIncNew == 1 */
		oldHeadPix = pixSnakeBodySW;
	} else if (yInc == -1) { /* xInc == 0 */
	    if (xIncNew == -1)   /* yIncNew == 0 */
		oldHeadPix = pixSnakeBodySW;
	    else                 /* xIncNew == 1 */
		oldHeadPix = pixSnakeBodySE;
	} else {                 /* yInc == 1, xInc == 0 */
	    if (xIncNew == -1)   /* yIncNew == 0 */
		oldHeadPix = pixSnakeBodyNW;
	    else                 /* xIncNew == 1 */
		oldHeadPix = pixSnakeBodyNE;
	}
	xInc = xIncNew;
	yInc = yIncNew;
    } else {
	if (xInc)
	    oldHeadPix = pixSnakeBodyWE;
	else
	    oldHeadPix = pixSnakeBodyNS;
    }
    newInc = 0;

    old = snakeHead;

    /* new snake head */
    if (++snakeHead >= MAX_SNAKE_LEN)
	snakeHead = 0;
    snake[snakeHead]->x = snake[old]->x + xInc;
    snake[snakeHead]->y = snake[old]->y + yInc;
    go = goGetObjectAt(snake[snakeHead]->x, snake[snakeHead]->y);
    if (xInc == 1)
	pix = pixSnakeHeadE;
    else if (xInc == -1)
	pix = pixSnakeHeadW;
    else if (yInc == 1)
	pix = pixSnakeHeadS;
    else
	pix = pixSnakeHeadN;
    snake[snakeHead]->pixmaps[0] = pix;
    goActivate(snake[snakeHead]);

    /* overwrite old snakehead */
    snake[old]->pixmaps[0] = oldHeadPix;
    goDraw(snake[old]);

    return go;
}

static void
snakeMoveTail(void)
{
    int dx, dy, old, next;
    Pixmap pix;

    old = snakeTail;
    if (++snakeTail >= MAX_SNAKE_LEN)
	snakeTail = 0;
    if ((next = snakeTail + 1) >= MAX_SNAKE_LEN)
	next = 0;
    dx = snake[next]->x - snake[snakeTail]->x;
    dy = snake[next]->y - snake[snakeTail]->y;
    if (dx == -1)
	pix = pixSnakeTailW;
    else if (dx == 1)
	pix = pixSnakeTailE;
    else if (dy == -1)
	pix = pixSnakeTailN;
    else /* dy == 1 */
	pix = pixSnakeTailS;
    goDeactivate(snake[old]);
    snake[snakeTail]->pixmaps[0] = pix;
    goDraw(snake[snakeTail]);
}

static void
snakeShrinkToLen(int len)
{
    while (snakeLen > len) {
	if (--snakeLen > 0)
	    snakeMoveTail();
	winFlush();
	gameAddScore(SCORE_BONUS_PER_LENGTH);
    }
}



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

void
snakeInit(void)
{
    int q;

    for (q = 0; q < MAX_SNAKE_LEN; q++) {
	snake[q] = goNew(GO_SNAKE);
	goAddPixmap(snake[q], 0, 0);
    }
}

void
snakeFinish(void)
{
    int q;

    for (q = 0; q < MAX_SNAKE_LEN; q++)
	goFree(snake[q]);
}

void
snakeInitGame(void)
{
    tickFreq = MAX_LEVEL - gameLevel + 1;
    currTick = 0;
    maxIdleTick = (120 * 33) / tickFreq;
}

void
snakeFinishGame(void)
{
}

void
snakeInitRound(void)
{
    GameObject *go;

    idleTick = 0;

    snakeTail = 0;
    snake[0]->x = X_MIN + (X_MAX - X_MIN) / 2;
    snake[0]->y = Y_MIN + (Y_MAX - Y_MIN) / 2;
    snake[0]->pixmaps[0] = pixSnakeTailE;
    if ((go = goGetObjectAt(snake[0]->x, snake[0]->y)) != NULL)
	goDeactivate(go);
    snake[1]->x = snake[0]->x + 1;
    snake[1]->y = snake[0]->y;
    snake[1]->pixmaps[0] = pixSnakeBodyWE;
    if ((go = goGetObjectAt(snake[1]->x, snake[1]->y)) != NULL)
	goDeactivate(go);
    snakeHead = 2;
    snake[2]->x = snake[1]->x + 1;
    snake[2]->y = snake[1]->y;
    snake[2]->pixmaps[0] = pixSnakeHeadE;
    if ((go = goGetObjectAt(snake[2]->x, snake[2]->y)) != NULL)
	goDeactivate(go);
    snakeLen = 3;
    xInc = 1;
    yInc = 0;
    digest = 0;
    moving = 0;
    newInc = 0;
    goActivate(snake[0]);
    goActivate(snake[1]);
    goActivate(snake[2]);
    snakeFlushKeys();
}

void
snakeFinishRound(void)
{
    int q;

    for (q = 0; q < MAX_SNAKE_LEN; q++)
	goDeactivate(snake[q]);
}

void
snakeHandleTick(void)
{
    int x, y, len;
    GameObject *go;

    if (++currTick < tickFreq)
	return;
    currTick = 0;

    snakeHandleTurn();
    if (!moving) {
	if (++idleTick >= maxIdleTick) {
	    printf("idling too long, ending game\n");
	    gameEndOfRound = 2;
	}
	return;
    }

    go = snakeMoveHead();
    if (digest) {
	--digest;
	++snakeLen;
    } else
	snakeMoveTail();

    /* check if the snake hit something */
    x = snake[snakeHead]->x;
    y = snake[snakeHead]->y;
    if (x < X_MIN || x > X_MAX || y < Y_MIN || y > Y_MAX)
	snakeDie();
    else if (go) {
	switch (go->type) {
	  case GO_MUSHROOM:
	  case GO_SCULL:
	  case GO_HEADBANGER:
	    goDeactivate(go);
	    /* fall through */
	  case GO_SNAKE:
	    snakeDie();
	    break;
	  case GO_SLIMPILL:
	    gameAddScore(SCORE_SLIMPILL);
	    len = (snakeLen * (100 - SLIMPILL_SHRINK_PST)) / 100;
	    if (len < 3)
		len = 3;
	    snakeShrinkToLen(len);
	    goDeactivate(go);
	    break;
	  case GO_LEMON:
	    gameAddScore(SCORE_LEMON);
	    goDeactivate(go);
	    digest += 3;
	    break;
	  case GO_BANANA:
	    gameAddScore(SCORE_BANANA);
	    goDeactivate(go);
	    digest += 6;
	    break;
	  case GO_PEAR:
	    gameAddScore(SCORE_PEAR);
	    goDeactivate(go);
	    digest += 9;
	    break;
	  case GO_STRAWBERRY:
	    gameAddScore(SCORE_STRAWBERRY);
	    goDeactivate(go);
	    digest += 12;
	    break;
	}
    }
}

void
snakeTurnNorth(void)
{
    keyPut(KEY_NORTH);
}

void
snakeTurnEast(void)
{
    keyPut(KEY_EAST);
}

void
snakeTurnSouth(void)
{
    keyPut(KEY_SOUTH);
}

void
snakeTurnWest(void)
{
    keyPut(KEY_WEST);
}

void
snakeGetHeadPos(int *x, int *y)
{
    *x = snake[snakeHead]->x;
    *y = snake[snakeHead]->y;
}

void
snakeFlushKeys(void)
{
    keyIn = keyOut = keyLen = 0;
}

void
snakeDie(void)
{
    boardDrawBlock(pixSplat, snake[snakeHead]->x, snake[snakeHead]->y);
    winFlush();
    snakeShrinkToLen(0);
    boardDrawBlock(pixSplat, snake[snakeHead]->x, snake[snakeHead]->y);
    winFlush();
    gameEndOfRound = 1;
}
