#if !defined (_WINGUARD_H_)
#define _WINGUARD_H_

#include <windows.h>

namespace KLSysNS {

  class WinGuard {
  public:
    WinGuard () { InitializeCriticalSection ( &theCriticalSection ); }
    ~WinGuard () { DeleteCriticalSection ( &theCriticalSection ); }
    
    class Lock {
    public:
      Lock ( WinGuard& aWinGuard ) : theCriticalSection ( &aWinGuard.theCriticalSection ) { EnterCriticalSection ( theCriticalSection ); }
      ~Lock () { LeaveCriticalSection ( theCriticalSection ); }
      
    private:
      LPCRITICAL_SECTION theCriticalSection; 
    };
    
    friend class Lock;
    
  private:
    CRITICAL_SECTION theCriticalSection;
  };

} // KLSysNS



#endif // _WINGUARD_H_
