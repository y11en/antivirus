#if !defined (_UTILS_H_)
#define _UTILS_H_

namespace PragueRPC {
  class Lock {
  public:
    
    Lock ( pthread_mutex_t* aLock )
      : theLock ( aLock ),
      theLocked ( false )
      {
        lock ();
      }
  
    void lock () {
      if ( theLocked )
        return;
      pthread_mutex_lock ( theLock );
      theLocked = true;
    }
    
    void unlock () {
      if ( !theLocked )
        return;
      pthread_mutex_unlock ( theLock );
      theLocked = false;
    }
    
    ~Lock () { 
      unlock ();
    }
    
  private:
    pthread_mutex_t* theLock;
    bool theLocked;
  };

}

#if defined (__APPLE__)
#define MSG_NOSIGNAL 0
#endif
#endif // _UTILS_H_
