#if !defined (_GUARD_H_)
#define _GUARD_H_

#if defined (_WIN32)
#include "winguard.h"
namespace KLSysNS {
  typedef WinGuard Guard;
};
#elif defined (__unix__)
#include "posixguard.h"
namespace KLSysNS {
  typedef PosixGuard Guard;
}
#elif
#warning "Attention! Undefined operating system! Dummy guard will be used!"
#include "dummyguard.h"
namespace KLSysNS {
  typedef DummyGuard Guard;
}
#endif


#endif //_GUARD_H_
