#ifndef ___SIGNPORT_H
#define ___SIGNPORT_H

// place to put all compiller/OS dependent defines
#ifdef __MWERKS__
  #define _far 
  #define _pascal
  #define NO_DEBUG_KEY
#endif

#if defined (__unix__)
  #include "Avp_defs.h"
  #include <AVPPort.h>
#endif /* __unix__ */

#endif /* ___SIGNPORT_H */

