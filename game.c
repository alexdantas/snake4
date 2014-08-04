/* $Id: game.c,v 1.5 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            game.c
 *  MODULE OF       snake4 - game of snake eating fruit
 *
 *  DESCRIPTION     
 *
 *  WRITTEN BY      Sverre H. Huseby <shh@thathost.com>
 *
 **************************************************************************/

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/keysymdef.h>

#include <shhmsg.h>

#include "win.h"
#include "gameobject.h"
#include "snake.h"
#include "mushroom.h"
#include "scull.h"
#include "headbanger.h"
#include "slimpill.h"
#include "fruit.h"
#include "board.h"
#include "score.h"
#include "game.h"

/**************************************************************************
 *                                                                        *
 *                        P U B L I C    D A T A                          *
 *                                                                        *
 **************************************************************************/

int gameLevel = 0;
long gameScore = 0L, gameHighscore = 0L;
int gameRound;
int gameEndOfRound;  /* 1 = end of this round, >1 = end of all rounds */
int gameRunning;
int gamePause;
int gameExit;



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

void
gameInit(void)
{
    RANDOMIZE(time(NULL) + getpid());
    gameRunning = 0;
    gameExit = 0;

    winInit();
    scoreInit();
    boardInit();
    /* init-functions for other `modules' are called from board.c */
}

void
gameFinish(void)
{
    fruitFinish();
    pillFinish();
    headbangerFinish();
    scullFinish();
    mushFinish();
    snakeFinish();
    boardFinish();
    scoreFinish();
    winFinish();
}

void
gameInitGame(void)
{
    gameSetRound(1);
    gameSetScore(0);
    gameRunning = 1;

    scoreInitGame();
    boardInitGame();
    snakeInitGame();
    mushInitGame();
    scullInitGame();
    headbangerInitGame();
    pillInitGame();
    fruitInitGame();
}

void
gameFinishGame(void)
{
    gameRunning = 0;

    fruitFinishGame();
    pillFinishGame();
    headbangerFinishGame();
    scullFinishGame();
    mushFinishGame();
    snakeFinishGame();
    scoreFinishGame(); /* score must be updated before board is repainted */
    boardFinishGame();
}

void
gameInitRound(void)
{
    gameEndOfRound = 0;
    gamePause = 0;

    scoreInitRound();
    boardInitRound();
    snakeInitRound();
    mushInitRound();
    scullInitRound();
    headbangerInitRound();
    pillInitRound();
    fruitInitRound();
}

void
gameFinishRound(void)
{
    fruitFinishRound();
    pillFinishRound();
    headbangerFinishRound();
    scullFinishRound();
    mushFinishRound();
    snakeFinishRound();
    boardFinishRound();
    scoreFinishRound();

    gameSetRound(gameRound + 1);
}

void
gameHandleTick(void)
{
    /* let all `modules' handle the tick */
    goHandleTick();
    scoreHandleTick();
    boardHandleTick();
    snakeHandleTick();
    mushHandleTick();
    scullHandleTick();
    headbangerHandleTick();
    pillHandleTick();
    fruitHandleTick();

    winFlush();
}

void
gameSetLevel(int level)
{
    gameLevel = level;
    winShowLevel();
}

void
gameSetRound(int round)
{
    gameRound = round;
    winShowRound();
}

void
gameSetScore(long score)
{
    gameScore = score;
    if (gameScore > gameHighscore) {
	gameHighscore = gameScore;
	winShowHighscore();
    }
    winShowScore();
}

void
gameAddScore(long add)
{
    gameSetScore(gameScore + add);
}
