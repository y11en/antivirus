#ifndef __pr_reg_cpp_h
#define __pr_reg_cpp_h

#include <Prague/iface/i_reg.h>

#define cRegUndef ((tRegKey)-1)

// ---
class cAutoKey {
  
  protected:
    cRegistry* m_reg;
    tRegKey    m_key;
    bool       m_own;
    
  public:
    cAutoKey( cRegistry* reg ) : m_reg(reg), m_key(cRegUndef), m_own(true) {}
    ~cAutoKey() { 
      if ( m_reg && (m_key != cRegUndef) && m_own ) 
        m_reg->CloseKey( m_key ); 
    }
    
  public:
    operator tRegKey()  { return m_key; }
    
  public:
    bool ownership()    const               { return m_own; }
    bool ownership( bool owner )            { bool own=m_own; m_own=owner; return own; }
    bool operator  ==( tRegKey key ) const  { return m_key == key; }
    bool operator  !()  const               { return m_key == cRegUndef; }
    tRegKey  relinquish()                   { tRegKey key = m_key; m_key = cRegUndef; return key; }
    
    tRegKey operator = ( tRegKey another_obj ) { 
      if ( m_reg && (m_key!=cRegUndef) && m_own ) 
        m_reg->CloseKey( m_key ); 
      m_key = another_obj; 
      return m_key;
    }
};



#endif // __pr_cpp_h
