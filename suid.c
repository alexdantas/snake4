/* $Id: suid.c,v 1.3 2002/03/02 21:10:55 sverrehu Exp $ */
/**************************************************************************
 *
 *  FILE            suid.c
 *
 *  DESCRIPTION     Helper functions for programs running part time
 *                  with a different effective user ID.
 *
 *                  The executable file must have the set-UID bit set.
 *                  At startup, the function suidInit() is called to set
 *                  the effective UID to the user running the program,
 *                  to run securely.
 *
 *                  Before a privileged action is to be done, the function
 *                  suidStartPrivilegedAction() is called to set the
 *                  effective UID to the owner of the executable file.
 *
 *                  when the privileged action is done, call
 *                  suidEndPrivilegedAction() to reenter the secure state
 *                  where the effective UID matches the user running the
 *                  program.
 *
 *                  I have tested this on Linux, Irix, Solaris and SunOS.
 *                  If it doesn't work for you, and you fix it, please
 *                  mail me a patch.
 *
 *  WRITTEN BY      Sverre H. Huseby <shh@thathost.com>
 *
 **************************************************************************/

#include <sys/types.h>  /* uid_t */
#include <unistd.h>

#include <shhmsg.h>

#define HAVE_SETEUID    /* possibly move this to a Makefile or something */

#ifdef HAVE_SETEUID
#  define SETUID seteuid
#else
#  define SETUID setuid
#endif

/**************************************************************************
 *                                                                        *
 *                       P R I V A T E    D A T A                         *
 *                                                                        *
 **************************************************************************/

static uid_t uidOwner;  /* the effective uid at startup */
static uid_t uidUser;   /* the real uid at startup */



/**************************************************************************
 *                                                                        *
 *                    P U B L I C    F U N C T I O N S                    *
 *                                                                        *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *
 *  NAME          suidInit
 *
 *  FUNCTION      Do initialization at program startup.
 *
 *  SYNOPSIS      #include "suid.h"
 *                void suidInit(void);
 *
 *  RETURNS       Nothing. Aborts the program in case of an error.
 *
 *  DESCRIPTION   Sets the effective UID equal to the real UID, to run
 *                securely. Also sets up variables used by the other
 *                functions.
 */
void
suidInit(void)
{
    uidOwner = geteuid();
    uidUser = getuid();
    if (uidOwner == 0 && uidUser != 0)
	msgFatal("won't run as set-UID root\n");
    if (SETUID(uidUser) < 0)
	msgFatalPerror("suidInit");
}

/*-------------------------------------------------------------------------
 *
 *  NAME          suidStartPrivilegedAction
 *
 *  FUNCTION      Prepare a privileged action.
 *
 *  SYNOPSIS      #include "suid.h"
 *                void suidStartPrivilegedAction(void);
 *
 *  RETURNS       Nothing. Aborts the program in case of an error.
 *
 *  DESCRIPTION   Sets the effective UID to the program owner.
 */
void
suidStartPrivilegedAction(void)
{
    if (SETUID(uidOwner) < 0)
	msgFatalPerror("suidStartPrivilegedAction");
}

/*-------------------------------------------------------------------------
 *
 *  NAME          suidEndPrivilegedAction
 *
 *  FUNCTION      Reenter secure mode after a privileged action.
 *
 *  SYNOPSIS      #include "suid.h"
 *                void suidEndPrivilegedAction(void);
 *
 *  RETURNS       Nothing. Aborts the program in case of an error.
 *
 *  DESCRIPTION   Sets the effective UID to the program user.
 */
void
suidEndPrivilegedAction(void)
{
    if (SETUID(uidUser) < 0)
	msgFatalPerror("suidEndPrivilegedAction");
}
