/* $Id: score.h,v 1.5 1996/10/11 15:33:08 sverrehu Exp $ */
#ifndef SCORE_H
#define SCORE_H

#define MAX_HIGHSCORES 200

void scoreInit(void);
void scoreFinish(void);
void scoreInitGame(void);
void scoreFinishGame(void);
void scoreInitRound(void);
void scoreFinishRound();
void scoreHandleTick(void);
int  scoreGetThisPlayerIndex(void);
char *scoreGetHeadStr(void);
char *scoreGetHeadSepStr(void);
char *scoreGetEntryStr(int n);
void scoreDumpHighscores(void);
void scoreMergeScoreFile(char *file);

#endif
