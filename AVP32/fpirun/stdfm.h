/****************************************************************************\
  MODULE........: STDFM.H | This is general header module.
  AUTHOR........: Alex Starkov (AS)
  COPYRIGHT.....: Copyright (c) 1997, Data Fellows Ltd.
  VERSION.......: 1.00
  HISTORY.......: DATE     COMMENT
                  -------- ------------------------------------------------
                  03-01-97 Created.
\****************************************************************************/

/*===========================================[ REDEFINITION DEFENCE ]=======*/
#ifndef STDFM_H
#define STDFM_H

/*===========================================[ SPECIAL ]====================*/
/*===========================================[ IMPORT DECLARATIONS ]========*/

//#include <io.h>  // moved to "archives.h" because of kernel mode
                   // problem; 980528, alk

#ifndef VTOOLSD

#ifndef NT_DRIVER
#include <string.h>
#endif  // NT_DRIVER

#if !defined _WIN32 && !defined NT_DRIVER && !defined UNIX
#include <dos.h>
#endif

#endif // !VTOOLSD

#include "fmtypes.h"
#include "fileio.h"

/*===========================================[ PUBLIC CONSTANTS ]===========*/
/*===========================================[ PUBLIC TYPES ]===============*/
/*===========================================[ PUBLIC VARIABLES ]===========*/
/*===========================================[ PUBLIC FUNCTIONS ]===========*/
/*===========================================[ PSEUDO/INLINE FUNCTIONS ]====*/
/*===========================================[ END REDEFINITION DEFENCE ]===*/
#endif/*STDFM_H*/

/** (END OF FILE  : stdfm.h) ************************************************/
