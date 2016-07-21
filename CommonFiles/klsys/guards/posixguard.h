#if !defined (_POSIXGUARD_H_)
#define _POSIXGUARD_H_

#include <pthread.h>

namespace KLSysNS {

  class PosixGuard {
  public:
    PosixGuard () { pthread_mutex_init ( &theMutex, 0 ); }
    ~PosixGuard () { pthread_mutex_destroy ( &theMutex ); }

    class Lock {
    public:
      Lock ( PosixGuard& aPosixGuard ) : theMutex ( &aPosixGuard.theMutex ) { pthread_mutex_lock ( theMutex ); }
      ~Lock () { pthread_mutex_unlock ( theMutex ); }
    private:
      pthread_mutex_t* theMutex;
    };
    
    friend class Lock;
    
  private:
    pthread_mutex_t theMutex;
  };

} // KLSysNS

#endif // _POSIXGUARD_H_
