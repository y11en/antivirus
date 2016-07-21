#include "stdafx.h"
#include "htmlgen.h"
#include "../AVPPveId.h"
#include "../PGIServerInterface.h"


#define BUFFS   (sizeof(g_work))
#define REST(l) (BUFFS-l)


char      g_work[0x1000];
const int g_works = BUFFS;


// ---
void replace_spaces( char* s, int l ) {
  for( ; l; s++,l-- ) {
		if ( *s == ' ' ) {
			::memmove( s+6, s+1, l-1 );
			::memcpy( s, "&nbsp;", 6 );
			s += 5;
		}
	}
}




// ---
HRESULT CCodeGen::MethodHTML( CHtml* parent, CMethodInfo& method ) {
	CHtml* params;
	CHtml* table;
	CAPointer<char> str;

	DWORD len = PrepareMethodString( method, str );
	//replace_spaces( str, len );
	parent->styled_text( gst_syntax, str );

	CComment comm( method );
	comment( parent, comm, m_lang, t_short );
	comment( parent, comm, m_lang, t_large );
	comment( parent, comm, m_lang, t_behav );
	
	CMethodParamInfo pi( method );
	if ( pi ) {
		CCommentList cpi( pi, m_lang );
		bool large_comment = cpi.exist( m_lang, t_lb );

		params = parent->para();
		params->add( "h5" )->line( "ѕараметры" );

		CPArray<char> titles(3,10,false);
		titles.Add( "тип" );
		titles.Add( "им€" );
		titles.Add( "описание" );
		if ( large_comment )
			titles.Add( "подробное описание" );
		
		table = params->table( titles );

		pi.GoFirst();
		while( pi ) {
			CHtml* row = table->add( "tr" );
			CHtml* field;
			CComment comm( pi );

			len = GetParamType( pi, g_work, sizeof(g_work) );
			replace_spaces( g_work, len );

			row->add( "td", "width", "1" )->str( g_work );
			row->add( "td", "width", "1" )->style( gst_item_id )->str( pi.Name() );
			field = row->add( "td" );
			comment( field, comm, m_lang, t_short, false );
			if ( large_comment ) {
				field = row->add( "td" );
				if ( !comment(field,comm,m_lang,t_large,false) )
					comment( field, comm, m_lang, t_behav,false );
			}
			pi.GoNext();
		}
	}

	return S_OK;
}





// ---
DWORD CCodeGen::PrepareMethodString( const CMethodInfo& mi, CAPointer<char>& str ) {

  tDWORD s=0, l=0, c;
  tDWORD type_id = mi.TypeID();
	tDWORD tot     = mi.TypeOfType();

  /*
  s = ::LoadString( _Module.GetModuleInstance(), IDS_METHOD_PREFIX, g_buff, REST(0) );
  if ( !s )
    s = lstrlen( lstrcpy(g_buff,"static ") );
  */

  ::memcpy( g_buff, "tERROR pr_call ", s=15 );

	l = ::lstrlen( mi.Name() );
	::memcpy( g_buff+s, mi.Name(), l );
	s += l;

	*((WORD*)(g_buff+s)) = MAKEWORD( BRACKET_O, SPACE );  s += 2;
	
	*(g_buff + s++) = 'h';
  l = ::lstrlen( m_iface.Name() );
	::memcpy( g_buff+s, m_iface.Name(), l );
	s += l;

  ::memcpy( g_buff+s, " _this", 6 );
	s += 6;
	
  if ( HAS_RET_VAL(type_id,tot) ) {
    CObjTypeInfo oti( m_types );
    l = mi.TypeName( &oti, g_work, sizeof(g_work) );
    if ( 1 >= l ) 
      ::memcpy( g_work, UNKNOWN_RESULT_TYPE, l=::lstrlen(UNKNOWN_RESULT_TYPE)+1 );

    if ( ::lstrcmp(g_work,"void") && ::lstrcmp(g_work,"tVOID") ) {
      *((WORD*)(g_buff+s)) = MAKEWORD( COMMA, SPACE );  s += 2;
      ::memcpy( g_buff+s, g_work, l );                  s += l-1;
      ::memcpy( g_buff+s, "* result", 8 );              s += 8;
    }
  }

  c = 1;
  CMethodParamInfo mpi( mi );
  while( mpi ) {
    
    *((WORD*)(g_buff+s)) = MAKEWORD( COMMA, SPACE );  s += 2;

    if ( 1 > mpi.Name(g_work,sizeof(g_work)) )
      ::wsprintf( g_work, "param%d", c );

    if ( !::lstrcmp(g_work,"...") ) {
      ::memcpy( g_buff+s, "...", 3 );
      s += 3;
      break;
    }

    if ( 1 >= (l=GetParamType(mpi,g_buff+s,REST(s))) ) {
      ::memcpy( g_buff+s, "???", 3 );
      l = 4;
    }

    s += l-1;

    g_buff[s++] = SPACE;

    l = ::lstrlen( g_work );
    ::memcpy( g_buff+s, g_work, l );
    s += l;

    mpi.GoNext();
    c++;
  }

  *((WORD*)(g_buff+s)) = MAKEWORD( SPACE, BRACKET_C ); s += 2;
  *(g_buff+s++) = 0;

	tDWORD spaces = 10 + c * 2;
	spaces *= 10;
	str = (char*)::memcpy( new char[s+spaces], g_buff, s );
  return s;
}




// ---
DWORD CCodeGen::GetParamType( CMethodParamInfo& param, char* buffer, tDWORD ilen ) { // , CPArray<char>* forwards ) {
	
  if ( !buffer != !ilen )
    return ERROR_INVALID_PARAMETER;
	
  tDWORD s;
  tDWORD l;
  BOOL      pointer = param.IsPointer();
  BOOL      cnst;
  const char unknown_type[] = "unknown_type";
  const char const_type[] = "const ";
  CObjTypeInfo oti( m_types );
	
	if ( pointer )
		cnst = param.IsConst();
	else {
		tDWORD pid = ResolveTypeID( param );
		switch( pid ) {
		case tid_STRING:
		case tid_WSTRING:
		case tid_PTR:
		case tid_FUNC_PTR:
		case tid_IFACEPTR:
		case tid_OBJECT:
		case tid_BINARY:
		case tid_DATETIME:
		case tid_EXPORT:
		case tid_IMPORT:
		case tid_LONGLONG:
		case tid_QWORD:
			cnst = param.IsConst();
			break;
		default:
			cnst = false;
		}
	}
	
  // calculate common type len
  if ( cnst )
    s = sizeof(const_type);
  else
    s = 0;
	
  if ( pointer ) 
    s++;
	
  l = param.TypeName( &oti, 0, 0 );
  if ( !l ) 
    l = sizeof(unknown_type);
	
  s += l;
	
  if ( !buffer ) 
    return s;
  
  buffer[0] = 0;
	
  if ( s > ilen )
    return 0;
	
  if ( cnst )
    ::memcpy( buffer, const_type, (s=sizeof(const_type)-1) );
  else
    s = 0;
	
  l=param.TypeName( &oti, buffer+s, ilen-s );
  if ( !l )
    ::memcpy( buffer, unknown_type, l=sizeof(unknown_type)-1 );
	
  s += l;
	
  if ( pointer )
    *(buffer + s++ - 1) = '*';
	
  *(buffer + s - 1) = 0;
	
  return s;
}




const IID   IID_IPragueIServer = {0xE635B364,0xB186,0x11D4,{0x96,0xB2,0x44,0x45,0x53,0x54,0x00,0x00}};
const CLSID CLSID_PragueIServer = {0x661A3E92,0xB16B,0x11D4,{0x96,0xB2,0x44,0x45,0x53,0x54,0x00,0x00}};


// ---
struct CNodeUID {
	TCHAR m_UID[41];
	CNodeUID()	{ memset(m_UID, 0, sizeof(m_UID)); }
	CNodeUID( TCHAR *pUID, DWORD dwSize)	{ 
		memset(m_UID, 0, sizeof(m_UID)); 
		memcpy(m_UID, pUID, min(dwSize, sizeof(m_UID) - 1));
		if ( lstrlen(m_UID) < sizeof(m_UID) - 1 )
			memset(m_UID + lstrlen(m_UID), 0, sizeof(m_UID) - lstrlen(m_UID));
	}
	operator LPSTR () { return m_UID; }
	operator LPCSTR () { return m_UID; }
	operator LPBYTE () { return (LPBYTE)m_UID; }
	operator LPVOID () { return (LPVOID)m_UID; }
  DWORD size() const { return sizeof(m_UID); }
  DWORD len()  const { return ::lstrlen(m_UID); }
};





// ---
tDWORD CCodeGen::ResolveTypeID( const CBaseInfo& item ) {
  tDWORD type_of_type  = item.TypeOfType();
  
  if ( type_of_type == VE_NTT_GLOBALTYPE )
    return item.TypeID();
	
  else if ( type_of_type == VE_NTT_INTERFACE ) 
    return tid_IFACEPTR; // tid_OBJECT ???
	
  else if ( type_of_type != VE_NTT_USERTYPE ) // unknown type of type
    return 0;
  
  else  {
    char name[MAX_PATH];
    CTypeInfo ti( m_iface );
    CComPtr<IPragueIServer> ifaces = 0;
    HPROP includes = 0;
    tDWORD if_count = 0;
    tDWORD if_ind = 0;
    CNodeUID  if_uid;
    tDWORD typedef_recursion_count = 0;
    CPointer<CFaceInfo> iface = 0;
    item.TypeName( 0, name, sizeof(name) );
    do {
      while( ti ) {
        const char* c_name = ti.Name();
        if ( !::lstrcmp(name,c_name) ) {
          type_of_type = ti.TypeOfType();
          if ( type_of_type == VE_NTT_GLOBALTYPE )
            return ti.TypeID();
          else if ( type_of_type == VE_NTT_INTERFACE )
            return tid_IFACEPTR;
          else if ( type_of_type == VE_NTT_USERTYPE ) {
            if ( ++typedef_recursion_count > 10 ) // possible infinite looping
              return 0;
            ti.TypeName( 0, name, sizeof(name) );
            ti.GoFirst();
            if_ind = 0;
          }
          else
            return 0;
        }
        else
          ti.GoNext();
      }
      
      if ( !ifaces ) {
        if ( SUCCEEDED(::CoCreateInstance(CLSID_PragueIServer,0,CLSCTX_SERVER,IID_IPragueIServer,(LPVOID*)&ifaces)) )
          ifaces->Init();
        else
          break;
        includes = ::DATA_Find_Prop( m_iface.Data(), 0, VE_PID_LINKEDUNIQUEID );
        if ( includes )
          if_count = ::PROP_Arr_Count( includes );
        else
          break;
      }
			
      if ( if_ind >= if_count )
        break;
			
      if ( 1 >= ::PROP_Arr_Get_Items(includes,if_ind++,if_uid,if_uid.size()) )
        break;
			
      DWORD if_size = 0;
      BYTE* if_content = 0;
      if ( FAILED(ifaces->LoadInterface(if_uid.len(),if_uid,FALSE,itPrototype,&if_size,&if_content)) || !if_content || !if_size ) {
        if ( if_content )
          ::CoTaskMemFree( if_content );
        continue;
      }
			
      iface = new CFaceInfo( if_size, if_content );
      ::CoTaskMemFree( if_content );
      if ( iface && iface->IsOK() )
        ti = iface->Data();
      else
        ti = 0;
    } while( 1 );
		
    return 0;
  }
}


