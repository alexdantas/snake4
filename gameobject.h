/* $Id: gameobject.h,v 1.1.1.1 1996/10/04 20:16:16 sverrehu Exp $ */
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <X11/Xlib.h> /* Pixmap */

struct GameObject;
typedef void (*GameObjectCallbackFunc)(struct GameObject *);

typedef struct GameObject {
    int    type;          /* for classification */
    int    active;        /* member of the game? */
    int    x, y;          /* position */
    Pixmap *pixmaps;      /* one or more pixmaps */
    int    numPixmaps;
    int    currPixmap;    /* current pixmap */
    int    *ticksPixmap;  /* game ticks for each pixmap to last */
    int    tickCount;     /* number of ticks so far for current pixmap */

    GameObjectCallbackFunc allPixmapsCallback;
                          /* called when all pixmaps are circulated throug */
    GameObjectCallbackFunc deactivateCallback;
                          /* called when object is deactivated */

    int    extra[10];     /* used defined */
    struct GameObject *next;
    struct GameObject *prev;
} GameObject;

/* game object types */
enum {
    GO_SNAKE,
    GO_MUSHROOM,
    GO_SCULL,
    GO_HEADBANGER,
    GO_SLIMPILL,
    GO_LEMON,
    GO_BANANA,
    GO_PEAR,
    GO_STRAWBERRY
};

GameObject *goNew(int type);
void goFree(GameObject *go);

void goAddPixmap(GameObject *go, Pixmap pix, int ticks);
void goHandleTick(void);
void goMove(GameObject *go, int x, int y);
void goActivate(GameObject *go);
void goDeactivate(GameObject *go);
void goDraw(GameObject *go);
void goDrawAll(void);
GameObject *goGetObjectAt(int x, int y);

#endif
