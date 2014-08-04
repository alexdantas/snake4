/* $Id: win.h,v 1.1.1.1 1996/10/04 20:16:16 sverrehu Exp $ */
#ifndef WIN_H
#define WIN_H

#include <X11/Xlib.h>       /* Display, Window */
#include <X11/Intrinsic.h>  /* Widget */

void winParseOptions(int *argc, char *argv[]);
void winInit(void);
void winFinish(void);
void winMainLoop(void);
void winFlush(void);
void winChooseLevel(int level);
void winTogglePause(void);
void winShowLevel(void);
void winShowRound(void);
void winShowScore(void);
void winShowHighscore(void);

extern Display *winDisplay;
extern XtAppContext winAppContext;
extern Widget  winTopLevel;
extern Widget  winMainWindow;

#endif
