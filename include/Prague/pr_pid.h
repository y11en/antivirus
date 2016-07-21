/*-----------------02.05.99 09:31-------------------
 * Project Prague                                 *
 * Subproject Plugin IDentifications              *
 * Author Andrew Andy Petrovitch                  *
 * Copyright (c) Kaspersky Lab                    *
 * Purpose Prague API                             *
 *                                                *
--------------------------------------------------*/

#ifndef __PR_PID_H
#define __PR_PID_H

#define PID_ANY     ((tPID)(0))
#define PID_SYSTEM  PID_ANY
#define PID_NONE    ((tPID)(0xffffffff))

#define PID_KERNEL        ((tPID)(1))
#define PID_APPLICATION   ((tPID)(2))
#define PID_NATIVE_FIO    ((tPID)(3))
#define PID_LOADER        ((tPID)(5))
#define PID_TMPFILE       ((tPID)(63002))
#define PID_STRING        ((tPID)(38))

//#define PID_DMAP          ((tPID)(63004))
//#define PID_MMIO				  ((tPID)(2000))
//#define SUBPID_MMIO_PROC	((tPID)(2001))
//#define PID_HOOK          ((tPID)(3000))


#endif
