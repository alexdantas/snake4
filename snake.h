/* $Id: snake.h,v 1.1.1.1 1996/10/04 20:16:16 sverrehu Exp $ */
#ifndef SNAKE_H
#define SNAKE_H

void snakeInit(void);
void snakeFinish(void);
void snakeInitGame(void);
void snakeFinishGame(void);
void snakeInitRound(void);
void snakeFinishRound();
void snakeHandleTick(void);
void snakeTurnNorth(void);
void snakeTurnEast(void);
void snakeTurnSouth(void);
void snakeTurnWest(void);
void snakeGetHeadPos(int *x, int *y);
void snakeFlushKeys(void);
void snakeDie(void);

#endif
