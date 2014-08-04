/* $Id: win.c,v 1.6 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            win.c
 *  MODULE OF       snake4 - game of snake eating fruit
 *
 *  DESCRIPTION     
 *
 *  WRITTEN BY      Sverre H. Huseby <shh@thathost.com>
 *
 **************************************************************************/

#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Core.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>

#include <shhmsg.h>

#include "game.h"
#include "board.h"
#include "snake.h"
#include "score.h"
#include "win.h"

/**************************************************************************
 *                                                                        *
 *                       P R I V A T E    D A T A                         *
 *                                                                        *
 **************************************************************************/

#define TICK_FREQ 30

static int winSleepDone;
static XtIntervalId timerId;
static Widget chooseLevelWidget, startWidget[MAX_LEVEL - MIN_LEVEL + 1];
static Widget levelWidget, roundWidget, scoreWidget, highscoreWidget;



/**************************************************************************
 *                                                                        *
 *                        P U B L I C    D A T A                          *
 *                                                                        *
 **************************************************************************/

Display *winDisplay;

XtAppContext winAppContext;
Widget winTopLevel;
Widget winMainWindow;


/**************************************************************************
 *                                                                        *
 *                   P R I V A T E    F U N C T I O N S                   *
 *                                                                        *
 **************************************************************************/

static void
winGetAndHandleEvent(void)
{
    XEvent e;

    XtAppNextEvent(winAppContext, &e);
    if (e.xany.window == boardWin || e.type == KeyPress)
	boardHandleEvent(&e);
    else if (e.type == FocusIn) {
	XAutoRepeatOff(winDisplay);  /* for users holding the keys down */
    } else if (e.type == FocusOut) {
	XAutoRepeatOn(winDisplay);
    } else
	XtDispatchEvent(&e);
}

static void
winSleepTimerProc(XtPointer foo, XtIntervalId *bar)
{
    XEvent e;

    winSleepDone = 1;
    /* wake up the waiting loop by sending a ClientMessage */
    e.type = ClientMessage;
    e.xclient.message_type = 0;
    e.xclient.format = 8;
    XSendEvent(winDisplay, XtWindowOfObject(winMainWindow), 0, 0, &e);
}

static void
winSleep(unsigned long ms)
{
    winSleepDone = 0;
    XtAppAddTimeOut(winAppContext, ms, winSleepTimerProc, 0);
    while (!winSleepDone && !gameExit)
	winGetAndHandleEvent();
}

static void timerProc(XtPointer, XtIntervalId *);

static void
installTimer(void)
{
    timerId = XtAppAddTimeOut(winAppContext, TICK_FREQ, timerProc, 0);
}

static void
removeTimer(void)
{
    XtRemoveTimeOut(timerId);
}

static void
timerProc(XtPointer foo, XtIntervalId *bar)
{
    installTimer();
    gameHandleTick();
    if (gameEndOfRound) {
	removeTimer();
	if (gameEndOfRound == 1)
	    winSleep(1000);
	gameFinishRound();
	if (gameRound > MAX_ROUNDS || gameEndOfRound > 1) {
	    int q;

	    gameFinishGame();

	    XtSetSensitive(chooseLevelWidget, 1);
	    for (q = 0; q < MAX_LEVEL - MIN_LEVEL + 1; q++)
		XtSetSensitive(startWidget[q], 1);
	    XtSetSensitive(levelWidget, 0);
	    XtSetSensitive(roundWidget, 0);
	    XtSetSensitive(scoreWidget, 0);
	    XtSetSensitive(highscoreWidget, 0);
	} else {
	    installTimer();
	    gameInitRound();
	}
    }
}

static void
callbackQuit(Widget w, XtPointer clientData, XtPointer callData)
{
    gameExit = 1;
}

static void
callbackStart(Widget w, XtPointer clientData, XtPointer callData)
{
    winChooseLevel((int) clientData);
}



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

void
winParseOptions(int *argc, char *argv[])
{
    static String resources[] = {
	"*board.accelerators: <Key>:",
	"*background: black",
	"*foreground: white",
	"*borderColor: dim gray",
	"*Command.background: gray",
	"*Command.foreground: black",
	"*score.foreground: yellow",
	"*font: 9x15bold",
	NULL
    };

    winTopLevel = XtVaAppInitialize(&winAppContext, "Snake4",
                                    NULL, 0, argc, argv, resources, NULL);
    winDisplay = XtDisplay(winTopLevel);
}

void
winInit(void)
{
    int q;
    char num[10];
    Widget boardWidget;
    Widget quitWidget;

    winMainWindow
	= XtVaCreateManagedWidget("mainWindow",
				  formWidgetClass, winTopLevel,
				  
				  NULL);
    boardWidget
	= XtVaCreateManagedWidget("board",
				  coreWidgetClass, winMainWindow,
				  XtNwidth, boardWidth,
				  XtNheight, boardHeight,
				  XtNborderWidth, 0,
				  NULL);

    chooseLevelWidget
	= XtVaCreateManagedWidget("chooseLevel",
				  labelWidgetClass, winMainWindow,
				  XtNlabel, "Choose level:",
				  XtNfromVert, boardWidget,
				  NULL);
    for (q = 0; q < MAX_LEVEL - MIN_LEVEL + 1; q++) {
	sprintf(num, "%d", q + MIN_LEVEL);
	startWidget[q]
	    = XtVaCreateManagedWidget(num,
				      commandWidgetClass, winMainWindow,
				      XtNfromHoriz, 
				        q ? startWidget[q - 1]
				          : chooseLevelWidget,
				      XtNfromVert, boardWidget,
				      NULL);
	XtAddCallback(startWidget[q], XtNcallback, callbackStart,
		      (XtPointer) (q + MIN_LEVEL));
    }

    levelWidget
	= XtVaCreateManagedWidget("level",
				  labelWidgetClass, winMainWindow,
				  XtNlabel, "Level: 0",
				  XtNfromVert, boardWidget,
				  XtNfromHoriz,
				    startWidget[MAX_LEVEL - MIN_LEVEL],
				  XtNsensitive, 0,
				  NULL);
    roundWidget
	= XtVaCreateManagedWidget("round",
				  labelWidgetClass, winMainWindow,
				  XtNlabel, "Lives: 0",
				  XtNfromVert, boardWidget,
				  XtNfromHoriz, levelWidget,
				  XtNsensitive, 0,
				  NULL);
    scoreWidget
	= XtVaCreateManagedWidget("score",
				  labelWidgetClass, winMainWindow,
				  XtNlabel, "Score:      0",
				  XtNfromVert, boardWidget,
				  XtNfromHoriz, roundWidget,
				  XtNsensitive, 0,
				  NULL);
    highscoreWidget
	= XtVaCreateManagedWidget("highscore",
				  labelWidgetClass, winMainWindow,
				  XtNlabel, "High:      0",
				  XtNfromVert, boardWidget,
				  XtNfromHoriz, scoreWidget,
				  XtNsensitive, 0,
				  NULL);
    quitWidget
	= XtVaCreateManagedWidget("quit",
				  commandWidgetClass, winMainWindow,
				  XtNlabel, "Quit",
				  XtNfromVert, boardWidget,
				  XtNfromHoriz, highscoreWidget,
				  NULL);
    XtAddCallback(quitWidget, XtNcallback, callbackQuit, 0);

    XtRealizeWidget(winTopLevel);
    XtInstallAccelerators(winTopLevel, boardWidget);

    boardWin = XtWindowOfObject(boardWidget);
}

void
winFinish(void)
{
}

void
winMainLoop(void)
{
    while (!gameExit)
	winGetAndHandleEvent();
    if (gameRunning)
	scoreFinishGame();
    XAutoRepeatOn(winDisplay);
    winFlush();
}

void
winFlush(void)
{
    XFlush(winDisplay);
}

void
winTogglePause(void)
{
    if (!gameRunning || gameEndOfRound)
	return;
    gamePause ^= 1;
    if (!gamePause)
	snakeFlushKeys();
    if (gamePause)
	removeTimer();
    else
	installTimer();
}

void
winChooseLevel(int level)
{
    int q;

    if (gameRunning || level < MIN_LEVEL || level > MAX_LEVEL)
	return;
    XtSetSensitive(chooseLevelWidget, 0);
    for (q = 0; q < MAX_LEVEL - MIN_LEVEL + 1; q++)
	XtSetSensitive(startWidget[q], 0);
    XtSetSensitive(levelWidget, 1);
    XtSetSensitive(roundWidget, 1);
    XtSetSensitive(scoreWidget, 1);
    XtSetSensitive(highscoreWidget, 1);

    gameSetLevel(level);
    gameInitGame();
    gameInitRound();
    installTimer();
}

void
winShowLevel(void)
{
    char text[40];

    sprintf(text, "Level: %d", gameLevel);
    XtVaSetValues(levelWidget, XtNlabel, text, NULL);
}

void
winShowRound(void)
{
    char text[40];

    sprintf(text, "Lives: %d", MAX_ROUNDS - gameRound + 1);
    XtVaSetValues(roundWidget, XtNlabel, text, NULL);
}

void
winShowScore(void)
{
    char text[40];

    sprintf(text, "Score:%7ld", gameScore);
    XtVaSetValues(scoreWidget, XtNlabel, text, NULL);
}

void
winShowHighscore(void)
{
    char text[40];

    sprintf(text, "High:%7ld", gameHighscore);
    XtVaSetValues(highscoreWidget, XtNlabel, text, NULL);
}
