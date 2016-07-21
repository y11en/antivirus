#if !defined (_DUMMYGUARD_H_)
#define _DUMMYGUARD_H_

namespace KLSysNS {

  class DummyGuard {
  public:
    DummyGuard () {}
    ~DummyGuard () {}
    
    class Lock {
    public:
      Lock ( DummyGuard& ) {}
      ~Lock () {}
    };
  };

} // KLSysNS



#endif // _DUMMYGUARD_H_
