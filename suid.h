/* $Id: suid.h,v 1.1 1996/10/08 16:49:00 sverrehu Exp $ */
#ifndef SUID_H
#define SUID_H

#ifdef __cplusplus
  extern "C" {
#endif

void suidInit(void);
void suidStartPrivilegedAction(void);
void suidEndPrivilegedAction(void);


#ifdef __cplusplus
  }
#endif

#endif
