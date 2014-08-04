/* $Id: score.c,v 1.8 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            score.c
 *  MODULE OF       snake4 - game of snake eating fruit
 *
 *  DESCRIPTION     
 *
 *  WRITTEN BY      Sverre H. Huseby <shh@thathost.com>
 *
 **************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>

#include <shhmsg.h>

#include "game.h"
#include "win.h"
#include "board.h"
#include "suid.h"
#include "score.h"

/**************************************************************************
 *                                                                        *
 *                       P R I V A T E    D A T A                         *
 *                                                                        *
 **************************************************************************/

#define TICK_FREQ 1000

static int currTick;

typedef struct {
    char *userName;
    char *realName;
    long score;
    int  level;
    char *dateTime;
} Highscore;

static Highscore *highscore[MAX_HIGHSCORES];
static int numHighscores;

static time_t lastUpdate = 0;



/**************************************************************************
 *                                                                        *
 *                   P R I V A T E    F U N C T I O N S                   *
 *                                                                        *
 **************************************************************************/

static char *
xstrdup(const char *s)
{
    char *ret;

    if ((ret = malloc(strlen(s) + 1)) == NULL)
	msgFatal("out of memory\n");
    strcpy(ret, s);
    return ret;
}

static Highscore *
scoreNew(void)
{
    Highscore *hs;

    if ((hs = malloc(sizeof(Highscore))) == NULL)
	msgFatal("out of memory\n");
    hs->userName = NULL;
    hs->realName = NULL;
    hs->score = 0L;
    hs->level = 0;
    hs->dateTime = NULL;
    return hs;
}

static Highscore *
scoreNewInit(char *uname, char *rname, long score, int level, char *dt)
{
    Highscore *hs;

    if ((hs = malloc(sizeof(Highscore))) == NULL)
	msgFatal("out of memory\n");
    hs->userName = xstrdup(uname);
    hs->realName = xstrdup(rname);
    hs->score = score;
    hs->level = level;
    hs->dateTime = xstrdup(dt);
    return hs;
}

static Highscore *
scoreNewCurrent(void)
{
    Highscore *hs;
    struct passwd *pw;
    char dateTime[20], *rname, *s;
    struct tm *tm;
    time_t tt;

    if ((pw = getpwuid(getuid())) == NULL) {
	msgError("unable to get password info\n");
	return NULL;
    }
    rname = xstrdup(pw->pw_gecos);
    if ((s = strchr(rname, ',')) != NULL)
	*s = '\0';
    time(&tt);
    tm = localtime(&tt);
    strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", tm);
    hs = scoreNewInit(pw->pw_name, rname, gameScore, gameLevel, dateTime);
    free(rname);
    return hs;
}

void
scoreFree(Highscore *hs)
{
    if (!hs)
	return;
    free(hs->userName);
    free(hs->realName);
    free(hs->dateTime);
    free(hs);
}

static time_t
scoreGetUpdateTime(void)
{
    struct stat st;

    if (stat(SCOREFILE, &st) < 0)
	return 0;
    return st.st_mtime;
}

static void
scoreDelete(int n)
{
    int q;

    if (n >= MAX_HIGHSCORES)
	return;
    scoreFree(highscore[n]);
    for (q = n; q < numHighscores - 1; q++)
	highscore[q] = highscore[q + 1];
    highscore[numHighscores - 1] = scoreNew();
    if (n < numHighscores)
	--numHighscores;
}

static void
scoreInsert(int n, Highscore *hs)
{
    int q;

    if (numHighscores == MAX_HIGHSCORES) {
	scoreFree(highscore[numHighscores - 1]);
	--numHighscores;
    }
    for (q = numHighscores; q > n; q--)
	highscore[q] = highscore[q - 1];
    highscore[n] = hs;
    ++numHighscores;
}

static int
scoreFindUser(const char *user)
{
    int q;

    for (q = 0; q < numHighscores; q++)
	if (strcmp(highscore[q]->userName, user) == 0)
	    return q;
    return -1;
}

static void
scoreLockHelp(FILE *f, short type)
{
    struct flock fl;

    fl.l_type = type;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0; /* lock to EOF */
    if (fcntl(fileno(f), F_SETLKW, &fl) < 0)
	msgFatal("error locking highscore file\n");
}

static void
scoreReadLockFile(FILE *f)
{
    scoreLockHelp(f, F_RDLCK);
}

static void
scoreWriteLockFile(FILE *f)
{
    scoreLockHelp(f, F_WRLCK);
}

static void
scoreUnlockFile(FILE *f)
{
    fflush(f);
    scoreLockHelp(f, F_UNLCK);
}

static Highscore *
scoreReadScoreLine(FILE *f)
{
    char line[100], *uname, *rname, *score, *level, *dateTime;
    Highscore *hs = NULL;

    if (fgets(line, sizeof(line), f)) {
        if (line[strlen(line) - 1] == '\n')
	    line[strlen(line) - 1] = '\0';
	if ((uname = strtok(line, ",")) == NULL)
	    msgFatal("error in highscore file\n");
	if ((rname = strtok(NULL, ",")) == NULL)
	    msgFatal("error in highscore file\n");
	if ((score = strtok(NULL, ",")) == NULL)
	    msgFatal("error in highscore file\n");
	if ((level = strtok(NULL, ",")) == NULL)
	    msgFatal("error in highscore file\n");
	if ((dateTime = strtok(NULL, ",")) == NULL)
	    msgFatal("error in highscore file\n");
	hs = scoreNewInit(uname, rname,	atol(score), atoi(level), dateTime);
    }
    return hs;
}

static void
scoreReadScoreLines(FILE *f, int winOk)
{
    int  n;
    char line[81];
    Highscore *hs;

    numHighscores = 0;
    if (fgets(line, sizeof(line), f) == NULL)
	return;
    n = atoi(line);
    while (numHighscores < n && (hs = scoreReadScoreLine(f)) != NULL) {
	scoreFree(highscore[numHighscores]);
	highscore[numHighscores] = hs;
	if (++numHighscores == MAX_HIGHSCORES)
	    break;
    }
    if (highscore[0]->score > gameHighscore) {
	gameHighscore = highscore[0]->score;
	if (winOk)
	    winShowHighscore();
    }
}

static void
scoreWriteScoreLines(FILE *f)
{
    Highscore *hs;
    int q;

    fprintf(f, "%d\n", numHighscores);
    for (q = 0; q < numHighscores; q++) {
	hs = highscore[q];
	/* strtok (used when reading score lines) doesn't like empty
	 * tokens, so we make sure no empty tokens are written out. */
	fprintf(f, "%s,%s,%ld,%d,%s\n",
		strlen(hs->userName) ? hs->userName : "nobody",
		strlen(hs->realName) ? hs->realName : "An Anonymous Gamer",
		hs->score, hs->level, hs->dateTime);
    }
}

static void
scoreReadScoreFile(int winOk)
{
    FILE *f;

    suidStartPrivilegedAction();
    numHighscores = 0;
    if ((f = fopen(SCOREFILE, "r")) == NULL)
	goto finish;
    scoreReadLockFile(f);
    scoreReadScoreLines(f, winOk);
    scoreUnlockFile(f);
    fclose(f);
  finish:
    suidEndPrivilegedAction();
}

static int
scorePossiblyAddEntry(Highscore *hs)
{
    int ret = 0, q, n;

    if ((n = scoreFindUser(hs->userName)) >= 0) {
	if (highscore[n]->score >= hs->score)
	    goto finish;
	scoreDelete(n);
    }
    for (q = 0; q < numHighscores; q++)
	if (highscore[q]->score < hs->score)
	    break;
    if (q < MAX_HIGHSCORES) {
	scoreInsert(q, hs);
	ret = 1;
    }
  finish:
    return ret;
}

static void
scorePossiblyUpdateScores(void)
{
    FILE *f;
    Highscore *hs;

    suidStartPrivilegedAction();
    if ((hs = scoreNewCurrent()) == NULL)
	goto finish;
    numHighscores = 0;
    if ((f = fopen(SCOREFILE, "r+")) == NULL) {
	msgPerror("unable to write `%s'", SCOREFILE);
	goto finish;
    }
    scoreWriteLockFile(f);
    scoreReadScoreLines(f, 1);
    if (scorePossiblyAddEntry(hs)) {
	rewind(f);
	scoreWriteScoreLines(f);
    } else
	scoreFree(hs);
    scoreUnlockFile(f);
    fclose(f);
  finish:
    suidEndPrivilegedAction();
}



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

void
scoreInit(void)
{
    int q;

    for (q = 0; q < MAX_HIGHSCORES; q++)
	highscore[q] = scoreNew();
    numHighscores = 0;
    lastUpdate = scoreGetUpdateTime();
    scoreReadScoreFile(0);
}

void
scoreFinish(void)
{
    int q;

    for (q = 0; q < MAX_HIGHSCORES; q++)
	scoreFree(highscore[q]);
}

void
scoreInitGame(void)
{
    currTick = 0;
}

void
scoreFinishGame(void)
{
    scorePossiblyUpdateScores();
}

void
scoreInitRound(void)
{
}

void
scoreFinishRound(void)
{
}

void
scoreHandleTick(void)
{
    time_t ut;

    if (++currTick < TICK_FREQ)
	return;
    currTick = 0;

    if ((ut = scoreGetUpdateTime()) > lastUpdate) {
	lastUpdate = ut;
	scoreReadScoreFile(1);
    }
}

int
scoreGetThisPlayerIndex(void)
{
    struct passwd *pw;

    if ((pw = getpwuid(getuid())) == NULL) {
	msgError("unable to get password info\n");
	return -1;
    }
    return scoreFindUser(pw->pw_name);
}

char *
scoreGetHeadStr(void)
{
    static char line[81];

    sprintf(line, "  #  %-10.10s %-25.25s %6.6s %3.3s  %s",
	    "user", "name", "score", "lvl", "date       time");
    return line;
}

char *
scoreGetHeadSepStr(void)
{
    static char line[81];
    int q;

    for (q = 0; q < 73; q++)
	line[q] = '-';
    line[q] = '\0';
    return line;
}

char *
scoreGetEntryStr(int n)
{
    static char line[81];
    Highscore *hs;

    if (n < 0 || n >= MAX_HIGHSCORES)
	return NULL;
    hs = highscore[n];
    if (n >= numHighscores)
	sprintf(line, "%3d.", n + 1);
    else
	sprintf(line, "%3d. %-10s %-25.25s %6ld %3d  %s", n + 1,
		hs->userName, hs->realName,
		hs->score, hs->level, hs->dateTime);
    return line;
}

void
scoreDumpHighscores(void)
{
    int q;

    if (!numHighscores)
	scoreReadScoreFile(0);
    printf("%s\n", scoreGetHeadStr());
    printf("%s\n", scoreGetHeadSepStr());
    for (q = 0; q < numHighscores; q++)
	printf("%s\n", scoreGetEntryStr(q));
    printf("\n");
}

void
scoreMergeScoreFile(char *file)
{
    int  q, n, changed = 0;
    FILE *orig, *merge;
    char line[81];
    Highscore *hs;

    suidStartPrivilegedAction();
    if (getuid() != 0 && getuid() != geteuid())
	msgFatal("scorefile merging only allowed for game owner or root\n");

    if ((merge = fopen(file, "r")) == NULL)
	msgFatalPerror("unable to open `%s'", file);
    scoreReadLockFile(merge);

    /* read current highscores */
    numHighscores = 0;
    if ((orig = fopen(SCOREFILE, "r+")) == NULL) {
	msgFatalPerror("unable to write `%s'", SCOREFILE);
	goto finish;
    }
    scoreWriteLockFile(orig);
    scoreReadScoreLines(orig, 0);

    /* merge new entries */
    if (fgets(line, sizeof(line), merge) == NULL)
	goto unlock;
    n = atoi(line);
    for (q = 0; q < n; q++)
        if ((hs = scoreReadScoreLine(merge)) != NULL)
	    if (scorePossiblyAddEntry(hs))
		changed = 1;
	    else
		scoreFree(hs);

    /* possibly save new highscore table */
    if (changed) {
	rewind(orig);
	scoreWriteScoreLines(orig);
    }

  unlock:
    scoreUnlockFile(orig);
    fclose(orig);
    scoreUnlockFile(merge);
    fclose(merge);
  finish:
    suidEndPrivilegedAction();
}
