#ifndef __prop_wrp_h
#define __prop_wrp_h

#include <Property/Property.h>

#ifndef __cplusplus
#error Only C++ code can include the header
#endif

class CAddr;
class CData;
class CProp;


// ---
class CAddr {

private:
  AVP_uint   m_count;
  AVP_dword  m_addr[6];
  AVP_dword* m_ext;

public:
  CAddr();
  CAddr( AVP_dword id0, ... );
  CAddr( const CAddr& other );
  CAddr( const CData& data, AVP_dword* addr, HDATA stop_data );
  ~CAddr();

  operator AVP_dword*();

  AVP_dword* Add( AVP_dword id0, ... );
  bool operator ==( const CAddr& other );
  bool operator !=( const CAddr& other );
};



// ---
class CData {

private:
  HDATA m_data;

public:
  static bool Init_Library( void* (*alloc)(AVP_size_t), void (*free)(void*) );

public:
  CData( AVP_dword id, AVP_size_t val, AVP_dword size ); // root initialization
  CData( HDATA _data ); // any other
  ~CData();
  operator HDATA() const;

  HDATA      Go             ();
  HDATA      Go             ( AVP_dword* addr );
  HDATA      Go             ( AVP_dword id0, ... );

  bool       Serialize      ( AVP_Serialize_Data* opts_in, void* buffer, AVP_dword size_in, AVP_dword* size_out );
  bool       Deserialize    ( AVP_Serialize_Data* opts_out, void* buffer, AVP_dword size_in, AVP_dword* size_out );

  HDATA      Add            ( AVP_dword id, AVP_size_t val, AVP_dword size );
  HDATA      Copy           ( HDATA copy, AVP_dword flags );
  HDATA      Replace        ( AVP_dword id, AVP_size_t val, AVP_dword size );
  HDATA      Attach         ( HDATA data2, AVP_dword flags );
  HDATA      Insert         ( HDATA data2 );
  bool       Multiplication ( HDATA data2, AVP_dword* addr2, HDATA* result, AVP_dword flags );
#ifdef __MEGRE_FUNC__
  bool       Merge          ( HDATA data2, AVP_dword* addr2, HDATA* result, AVP_dword flags, AVP_Merge_Param* param );
#endif
  HDATA      Detach         ( AVP_dword* addr );
  bool       Remove         ( AVP_dword* addr );
  HDATA      Find           ( AVP_dword* addr );
  AVP_dword  Get_Id         ();
  HDATA      Get_Dad        ();
  HDATA      Get_FirstEx    ( AVP_dword flags );
  HDATA      Get_LastEx     ( AVP_dword flags );
  HDATA      Get_Next       ();
  HDATA      Get_Prev       ();
  AVP_dword  For_Each       ( AVP_dword flags, For_Each_Proc proc, void* user_data );
  HDATA      First_That     ( AVP_dword flags, First_That_Proc proc, void* user_data );
  bool       Replace_App_ID ( AVP_byte  app_id, AVP_dword flags );
  bool       Replace_ID     ( AVP_word  id );

  HPROP      Add_Prop       ( AVP_dword pid, AVP_size_t val, AVP_dword size );
  HPROP      Replace_Prop   ( AVP_dword from, AVP_dword to, AVP_size_t val, AVP_dword size );
  bool       Remove_Prop_ID ( AVP_dword pid );
  bool       Remove_Prop_H  ( HPROP     prop );
  HPROP      Find_Prop      ( AVP_dword id );
  HPROP      Get_First_Prop ();
  
  AVP_dword  Set_Val        ( AVP_dword pid, AVP_size_t val, AVP_dword size );
  AVP_dword  Get_Val        ( AVP_dword pid, void* val, AVP_dword size ) const;
  AVP_TYPE   Get_Type       ( AVP_dword pid );
};



// ---
class CProp {

private:
  HPROP m_prop;

public:
  CProp( HPROP prop );

  HPROP      Go             ();
  HPROP      Get_Next       () const;
  AVP_dword  Set_Val        ( AVP_size_t val, AVP_dword size );
  AVP_dword  Get_Val        ( void* val,      AVP_dword size ) const;
  AVP_TYPE   Get_Type       () const;
  AVP_dword  Get_Id         () const;
  HDATA      Get_Dad        () const;
  AVP_dword  Get_SSize      () const;
  HPROP      Replace				( AVP_dword pid, AVP_size_t val, AVP_dword size );
  AVP_bool   Replace_App_ID ( AVP_byte  app_id );
  AVP_bool   Replace_ID     ( AVP_word  id );

  AVP_dword  Arr_Count      ();
  AVP_word   Arr_Delta      ( AVP_word  new_del );
  AVP_dword  Arr_Insert     ( AVP_dword pos, void* from, AVP_dword count );
  AVP_dword  Arr_Set_Items  ( AVP_dword pos, void* from, AVP_dword count ); 
  AVP_dword  Arr_Get_Items  ( AVP_dword pos, void* to,   AVP_dword count ); 
  AVP_dword  Arr_Get_SSize  ();
  AVP_dword  Arr_Remove     ( AVP_dword pos, AVP_dword count );
};



// ---
inline CAddr::operator AVP_dword*() {
  return m_ext ? m_ext : m_addr;
}

// ---
inline bool CAddr::operator !=( const CAddr& o ) {
  return !(*this == o );
}

// ---
bool CData::Init_Library( void* (*alloc)(AVP_size_t), void (*free)(void*) ) {
  return 0 != ::DATA_Init_Library( alloc, free );
}


// ---
inline CData::CData( AVP_dword id, AVP_size_t val, AVP_dword size ) 
  : m_data( ::DATA_Add(0,0,id,val,size) ) {
}

// ---
inline CData::CData( HDATA _data ) : m_data(_data) {
}

// ---
inline CData::operator HDATA() const {
  return m_data;
}

// ---
inline HDATA CData::Go() {
  return m_data = ::DATA_Get_Next( m_data, 0 );
}

// ---
inline HDATA CData::Go( AVP_dword* addr ) {
  return m_data = ::DATA_Find( m_data, addr );
}

// ---
inline bool CData::Serialize( AVP_Serialize_Data* opts_in, void* buffer, AVP_dword size_in, AVP_dword* size_out ) {
  return 0 != ::DATA_Serialize( m_data, 0, opts_in, buffer, size_in, size_out );
}

// ---
inline bool CData::Deserialize( AVP_Serialize_Data* opts_out, void* buffer, AVP_dword size_in, AVP_dword* size_out ) {
  if ( m_data )
    ::DATA_Remove( m_data, 0 );
  m_data = ::DATA_Deserialize( opts_out, buffer, size_in, size_out );
  return m_data != 0;
}

// ---
inline HDATA CData::Add( AVP_dword id, AVP_size_t val, AVP_dword size ) {
  return ::DATA_Add( m_data, 0, id, val, size );
}

// ---
inline HDATA CData::Copy( HDATA copy, AVP_dword flags ) {
  return ::DATA_Copy( m_data, 0, copy, flags );
}

// ---
inline HDATA CData::Replace( AVP_dword id, AVP_size_t val, AVP_dword size ) {
  return ::DATA_Replace( m_data, 0, id, val, size );
}

// ---
inline HDATA CData::Attach( HDATA data2, AVP_dword flags ) {
  return ::DATA_Attach( m_data, 0, data2, flags );
}

// ---
inline HDATA CData::Insert( HDATA data2 ) {
  return ::DATA_Insert( m_data, 0, data2 );
}

// ---
inline bool CData::Multiplication( HDATA data2, AVP_dword* addr2, HDATA* result, AVP_dword flags ) {
  return 0 != ::DATA_Multiplication( m_data, 0, data2, addr2, result, flags );
}

#ifdef __MEGRE_FUNC__
// ---
inline bool CData::Merge( HDATA data2, AVP_dword* addr2, HDATA* result, AVP_dword flags, AVP_Merge_Param* param ) {
  return 0 != ::DATA_Merge( m_data, 0, data2, addr2, result, flags, param );
}
#endif

// ---
inline HDATA CData::Detach( AVP_dword* addr ) {
  return ::DATA_Detach( m_data, addr );
}

// ---
inline bool CData::Remove( AVP_dword* addr ) {
  return 0 != ::DATA_Remove( m_data, addr );
}

// ---
inline HDATA CData::Find( AVP_dword* addr ) {
  return ::DATA_Find( m_data, addr );
}

// ---
inline AVP_dword CData::Get_Id() {
  return ::DATA_Get_Id( m_data, 0 );
}

// ---
inline HDATA CData::Get_Dad() {
  return ::DATA_Get_Dad( m_data, 0 );
}

// ---
inline HDATA CData::Get_FirstEx( AVP_dword flags ) {
  return ::DATA_Get_FirstEx( m_data, 0, flags );
}

// ---
inline HDATA CData::Get_LastEx( AVP_dword flags ) {
  return ::DATA_Get_LastEx( m_data, 0, flags );
}

// ---
inline HDATA CData::Get_Next() {
  return ::DATA_Get_Next( m_data, 0 );
}

// ---
inline HDATA CData::Get_Prev() {
  return ::DATA_Get_Prev( m_data, 0 );
}

// ---
inline AVP_dword CData::For_Each( AVP_dword flags, For_Each_Proc proc, void* user_data ) {
  return ::DATA_For_Each( m_data, 0, flags, proc, user_data );
}

// ---
inline HDATA CData::First_That( AVP_dword flags, First_That_Proc proc, void* user_data ) {
  return ::DATA_First_That( m_data, 0, flags, proc, user_data );
}

// ---
inline bool CData::Replace_App_ID( AVP_byte  app_id, AVP_dword flags ) {
  return 0 != ::DATA_Replace_App_ID( m_data, 0, app_id, flags );
}

// ---
inline bool CData::Replace_ID( AVP_word  id ) {
  return 0 != ::DATA_Replace_ID( m_data, 0, id );
}

// ---
inline HPROP CData::Add_Prop( AVP_dword pid, AVP_size_t val, AVP_dword size ) {
  return ::DATA_Add_Prop( m_data, 0, pid, val, size );
}

// ---
inline HPROP CData::Replace_Prop( AVP_dword from, AVP_dword to, AVP_size_t val, AVP_dword size ) {
  return ::DATA_Replace_Prop( m_data, 0, from, to, val, size );
}

// ---
inline bool CData::Remove_Prop_ID( AVP_dword pid ) {
  return 0 != ::DATA_Remove_Prop_ID( m_data, 0, pid );
}

// ---
inline bool CData::Remove_Prop_H( HPROP prop ) {
  return 0 != ::DATA_Remove_Prop_H( m_data, 0, prop );
}

// ---
inline HPROP CData::Find_Prop( AVP_dword id ) {
  return ::DATA_Find_Prop( m_data, 0, id );
}

// ---
inline HPROP CData::Get_First_Prop() {
  return ::DATA_Get_First_Prop( m_data, 0 );
}

// ---
inline AVP_dword CData::Set_Val( AVP_dword pid, AVP_size_t val, AVP_dword size ) {
  return ::DATA_Set_Val( m_data, 0, pid, val, size );
}

// ---
inline AVP_dword CData::Get_Val( AVP_dword pid, void* val, AVP_dword size ) const {
  return ::DATA_Get_Val( m_data, 0, pid, val, size );
}

// ---
inline AVP_TYPE CData::Get_Type( AVP_dword pid ) {
  return ::DATA_Get_Type( m_data, 0, pid );
}

#endif

