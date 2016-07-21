#ifndef __prop_wrp_h
#define __prop_wrp_h

#include <Property/Property.h>

#ifndef __cplusplus
#error Only C++ code can include the header
#endif

class CData;

// ---
class CAddr {

private:
  AVP_uint   m_count;
  AVP_dword  m_addr[6];
  AVP_dword* m_ext;

public:
  CAddr( AVP_dword* seq, ... );
  CAddr( AVP_dword  id0, ... );
  ~CAddr();

  operator AVP_dword*();

  AVP_dword* Add( ... );
  AVP_dword* Make( HDATA data, AVP_dword* addr, HDATA stop_data, AVP_dword** seq );
  const CAddr& operator =( const CAddr& other );
  bool operator ==( const CAddr& other );
  bool operator !=( const CAddr& other );
};



// ---
class CData {

private:
  bool  m_own;
  HDATA m_data;

public:
  static bool Init_Library( void* (*alloc)(AVP_uint), void (*free)(void*) );

public:
  CData();
  CData( HDATA _data );
  ~CData();
  operator HDATA();

  bool Serialize( AVP_dword* addr, AVP_Serialize_Data* opts_in, void* buffer, AVP_dword size_in, AVP_dword* size_out );
  bool Deserialize( AVP_Serialize_Data* opts_out, void* buffer, AVP_dword size_in, AVP_dword* size_out );
};

#endif


