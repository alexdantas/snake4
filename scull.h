/* $Id: scull.h,v 1.1.1.1 1996/10/04 20:16:17 sverrehu Exp $ */
#ifndef SCULL_H
#define SCULL_H

void scullInit(void);
void scullFinish(void);
void scullInitGame(void);
void scullFinishGame(void);
void scullInitRound(void);
void scullFinishRound();
void scullHandleTick(void);
void scullAt(int x, int y);

#endif
