/* $Id: game.h,v 1.2 1996/10/06 13:36:50 sverrehu Exp $ */
#ifndef GAME_H
#define GAME_H

#define RANDOM(x) (random() % (x))
#define RANDOMIZE(x) (srandom((unsigned) (x)))

/* keyboard routines in board.c needs to be changed if the following
   two are altered */
#define MIN_LEVEL 1
#define MAX_LEVEL 5

#define MAX_ROUNDS 3

#define SCORE_BONUS_PER_LENGTH   ( 10 * gameLevel)
#define SCORE_LEMON              (  5 * gameLevel)
#define SCORE_BANANA             ( 10 * gameLevel)
#define SCORE_PEAR               ( 15 * gameLevel)
#define SCORE_STRAWBERRY         ( 20 * gameLevel)
#define SCORE_SLIMPILL           (100 * gameLevel)
#define SCORE_HEADBANGER_MOVE    (  2)
#define SCORE_HEADBANGER_LEAVE   ( 50 * gameLevel)

#define SLIMPILL_SHRINK_PST      50

void gameInit(void);
void gameFinish(void);
void gameInitGame(void);
void gameFinishGame(void);
void gameInitRound(void);
void gameFinishRound(void);
void gameHandleTick(void);
void gameSetLevel(int level);
void gameSetRound(int round);
void gameSetScore(long score);
void gameAddScore(long add);

extern int gameLevel;
extern long gameScore;
extern long gameHighscore;
extern int gameRound;
extern int gameEndOfRound;
extern int gameRunning;
extern int gamePause;
extern int gameExit;

#endif
