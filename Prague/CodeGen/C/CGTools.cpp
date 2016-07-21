#include "StdAfx.h"
#include "CodeGen.h"
#include <time.h>
#include <version\ver_kl.h>
#include "options.h"
#include <stdarg.h>
#include "..\AVPPveID.h"
#include "..\PCGError.h"
#include <stuff\cpointer.h>

const IID   IID_IPragueIServer = {0xE635B364,0xB186,0x11D4,{0x96,0xB2,0x44,0x45,0x53,0x54,0x00,0x00}};
const CLSID CLSID_PragueIServer = {0x661A3E92,0xB16B,0x11D4,{0x96,0xB2,0x44,0x45,0x53,0x54,0x00,0x00}};

char work[4][0x1000];


// ---
HRESULT CCodeGen::PutString( COutputSect& sect, DWORD shift, const char* s, int pos ) {
  DWORD l = s ? ::lstrlen( s ) : 0;
  
  while( l && ((s[l-1] == ' ') || (s[l-1] == '\r') || (s[l-1] == '\n')) )
    l--;

	char* p = 0;
  if ( l ) {
    DWORD h, w;
    if ( shift ) {
      if ( m_opt.tabs() ) {
        h = shift / 2;
        w = '\t';
      }
      else {
        h = shift / 2 * m_opt.indent();
        w = ' ';
      }
    }
    else 
      h = 0;

    p = new char[ h + l + 1 ];
    if ( h )
      ::memset( p, w, h );
    ::memcpy( p+h, s, l );
    p[h+l] = 0;
  }
  else
    p = (char*)::memcpy( new char[1], "", 1 );

	int c = sect.Count();
	if ( pos > c )
		sect.Add( p );
	else
		sect.Insert( pos, p );
  return S_OK;
}



// ---
HRESULT CCodeGen::PutFmtString( COutputSect& file, DWORD shift, const char* fmt, ... ) {
  HRESULT hr;

  if ( !fmt )
    return E_UNEXPECTED;

  if ( fmt && (HIWORD(fmt) == 0) ) {
    if ( !::LoadString(_Module.GetModuleInstance(),(DWORD)fmt,A1) ) 
      return HRESULT_FROM_WIN32( GetLastError() );
    fmt = W1;
  }

  DWORD   dword[10];
  va_list arglist;
  va_start( arglist, fmt );
  //va_arg( arglist, UINT );
  for( int i=0; i<sizeof(dword)/sizeof(dword[0]); i++ )
    dword[i] = va_arg( arglist, DWORD );
  va_end( arglist );
  if ( !::wsprintf(W0,fmt,dword[0],dword[1],dword[2],dword[3],dword[4],dword[5],dword[6],dword[7],dword[8],dword[9]) )
    return HRESULT_FROM_WIN32( GetLastError() );
  else if ( FAILED(hr=PutString(file,shift,W0)) )
    return hr;
  else
    return S_OK;
}



/*
// ---
HRESULT CCodeGen::AddFmtString( CPArray<char>& strings, DWORD shift, const char* fmt, ... ) { // load format from resource and PutString. Uses W0 and W1. All pars MUST be convertible to DWORD

  if ( fmt ) {
    if ( HIWORD(fmt) == 0 ) {
      if ( !::LoadString(_Module.GetModuleInstance(),(DWORD)fmt,A1) ) 
        return HRESULT_FROM_WIN32( GetLastError() );
    }
    else
      ::lstrcpy( W1, (const char*)fmt );
  }
  else 
    return E_UNEXPECTED;

  DWORD   dword[10];
  va_list arglist;
  va_start( arglist, fmt );
  //va_arg( arglist, UINT );
  for( int i=0; i<sizeof(dword)/sizeof(dword[0]); i++ )
    dword[i] = va_arg( arglist, DWORD );
  va_end( arglist );

  int len = ::wsprintf( W0, W1, dword[0], dword[1], dword[2], dword[3], dword[4], dword[5], dword[6], dword[7], dword[8], dword[9] );
  char* str = new char[ len + 1 ];
  if ( str ) {
    ::memcpy( str, W0, len+1 );
    strings.Add( str );
    return S_OK;
  }
  else
    return E_OUTOFMEMORY;
}
*/



// ---
HRESULT CCodeGen::PutHeader( COutputFile& file, const char* file_name ) {

  COutputSect& sect = *new COutputSect( file, if_header, 0, false );
  PutFmtString( sect, 0, "// -------- %s --------", m_desriptions[m_desc_id] );

  time_t now = ::time( 0 );
  tm* tm = ::localtime( &now );
  ::strftime( A0, "// -------- %A,  %d %B %Y,  %H:%M --------", tm );
  PutString( sect, 0, W0 );

  ::lstrcpy( W3, "// " );
  ::lstrcat( W3, VER_LEGALCOPYRIGHT_STR );
  PutString( sect, 0, "// -------------------------------------------" );
  PutString( sect, 0, W3 );
  PutString( sect, 0, "// -------------------------------------------" );
  if ( 1 < m_plugin.Project(A3) )
    PutFmtString( sect, 0, "// Project     -- %s", W3 );
  else
    PutString( sect, 0, "// Project     -- Not defined" );

  if ( 1 < m_plugin.SubProject(A3) )
    PutFmtString( sect, 0, "// Sub project -- %s", W3 );
  else
    PutString( sect, 0, "// Sub project -- Not defined" );

  if ( 1 < m_plugin.ShortComment(A3) )
    PutFmtString( sect, 0, "// Purpose     -- %s", W3 );
  else
    PutString( sect, 0, "// Purpose     -- Not defined" );

  if ( m_plugin.Author(A3) )
    PutFmtString( sect, 0, "// Author      -- %s", W3 );
  else
    PutString( sect, 0, "// Author      -- Not defined" );

  PutFmtString( sect, 0, "// File Name   -- %s", ExtractFileName(file_name,W3) ); 
  PutExtendedComment( sect, 0, "Additional info", m_plugin, VE_PID_LARGECOMMENT );
  PutString( sect, 0, "// -------------------------------------------" );

  return S_OK;
}




// ---
const char* CCodeGen::ExtractFileName( const char* path_name, char* file_name ) {

  ::lstrcpy( file_name, path_name );

  char* ptr = ::strrchr( file_name, '\\' );
  if ( ptr )
    ptr++;
  else {
    ptr = ::strrchr( file_name, '/' );
    if ( ptr )
      ptr++;
    else
      ptr = file_name;
  }
  ::memmove( file_name, ptr, ::lstrlen(ptr)+1 );
  ::_strlwr( file_name );
  return file_name;
}



// ---
HRESULT CCodeGen::PutExtendedComment( COutputSect& sect, DWORD shift, const char* hdr, const CBaseInfo& node, DWORD id, bool new_line ) {
  HPROP prop;
  DWORD len;

  if ( node && (0 != (prop = ::DATA_Find_Prop(node(),0,id))) && (1 < (len = ::PROP_Get_Val(prop,0,0))) ) {
    CAPointer<char> cont = new char[len];
    char* p = cont;
		DWORD i;
    DWORD str_cnt = 1;
    ::PROP_Get_Val( prop, p, len );
    for( i=0; i<len; i++ ) {
      if ( (p[i] == '\r') || (p[i] == '\n') ) {
        str_cnt++;
        if ( ((*(WORD*)(p+i)) == MAKEWORD('\r','\n')) || ((*(WORD*)(p+i)) == MAKEWORD('\n','\r')) ) 
          ::memmove( p+i+1, p+i+2, --len-i-1 );
        p[i] = 0;
      }
    }
  
    if ( HIWORD(hdr) ) {
      int s = 0;
      if ( new_line && (*((WORD*)hdr) != MAKEWORD('/','/')) ) {
        s = 3;
        *((DWORD*)W3) = MAKELONG( MAKEWORD('/','/'), MAKEWORD(' ',0) );
      }
      ::lstrcpy( W3+s, hdr );
    }
    else if ( !hdr || !::LoadString(_Module.GetModuleInstance(),(DWORD)hdr,A3) )
			W3[0] = 0;

    if ( new_line && W3[0] ) {
      PutString( sect, shift, W3 );
      W3[0] = 0;
    }

    p = cont;
    for( i=0; i<str_cnt; i++ ) {
      len = ::lstrlen( p );
      PutFmtString( sect, shift, "//   %s %s", W3, p );
      p += len + 1;
      if ( !new_line ) {
        len = ::lstrlen( W3 );
        ::memset( W3, ' ', len );
        W3[len] = 0;
        new_line = true;
      }
    }
  }
  return S_OK;
}



// ---
HRESULT CCodeGen::CalcPropWidths( CPropInfo& prop, bool native, DWORD widths[2] ) {

  if ( prop.IsSeparator() )
		return S_OK;

  AVP_bool real = prop.Predefined() || prop.Native();

  if ( !real == !native ) {
    CPropTypeInfo pti(m_types);
    AVP_dword l1, l2;

    l1 = prop.Name(0,0);
    if ( 1 >= l1 ) {
      l1 = 0; // l1 = 22; // "unknown_property_name"
		}

    l2 = pti.MnemonicID( ResolveTypeID(prop), 0, 0 );
    if ( !l2 ) {
      //l2 = 15; // "pTYPE_DWORD   " instead "pTYPE_..."
			//return PutFmtString( sect, 0, "#error -- Cannot get type name for property \"%s\"", W2 );
		}

    if ( widths[0] < l1 )
      widths[0] = l1;

    if ( widths[1] < l2 )
      widths[1] = l2;
  }

  return S_OK;
}



// ---
HRESULT CCodeGen::PropToInclude( COutputSect& sect, CPropInfo& prop, AVP_bool native, DWORD ws[2], bool is_comment ) {

	if ( prop.IsSeparator() )
		return S_OK;

  AVP_bool predef = prop.Predefined();
  AVP_bool real   = predef || prop.Native();

  if ( !native == !real ) {
    CPropTypeInfo pti(m_types);

    AVP_dword l1, l2;
    AVP_dword dig_id;

    l1 = prop.Name(A2);
    if ( 1 >= l1 ) {
			prop.TypeName( 0, A2 );
			return PutFmtString( sect, 0, "#error -- Cannot get property name (id=0x%08x, type=\"%s\")", prop.DigitalID(), W2 );
      //::lstrcpy( W2, "unknown_property_name" );
      //l1 = 22; 
    }

    l2 = pti.MnemonicID( ResolveTypeID(prop), A3 );
    if ( !l2 ) {
			return PutFmtString( sect, 0, "#error -- Cannot get property type for \"%s\"", W2 );
      //::lstrcpy( W3, "pTYPE_DWORD   "); // instead of ::lstrcpy( W3, "pTYPE_..." );
      //l2 = 15;
    }

    dig_id = prop.DigitalID();
    if ( !dig_id )
      dig_id = 0xffffffff;

    l1 = ws[0] - l1;
    l2 = ws[1] - l2;

    CAPointer<char> sp1( new char[l1+1] );
    CAPointer<char> sp2( new char[l2+1] );

    ::memset( sp1, ' ', l1 ); *(sp1+l1) = 0;
    ::memset( sp2, ' ', l2 ); *(sp2+l2) = 0;

    // prop name to W2 ( pgPROP_NAME or plPROP_NAME )
		/*
    UINT id = native ? IDS_PROP_NAME_GLOBAL_FMT : IDS_PROP_NAME_LOCAL_FMT; // IDS_PROP_NAME_REQ_FMT;
    if ( !::LoadString(_Module.GetModuleInstance(),id,A1) ) 
		*/
      ::lstrcpy( W1, native ? "pg%s" : "pl%s" );

    ::memcpy( W2, W0, ::wsprintf(W0,W1,W2)+1 );

		if( !is_comment && native && predef )
			is_comment = true;

    return PutFmtString( sect, 0, "%s#define %s %smPROPERTY_MAKE_%s( %s, %s0x%08x )",
			is_comment ? "//! " : "", W2, (char*)sp1, native ? "GLOBAL" : "LOCAL", W3, (char*)sp2, dig_id );
  }

  return S_OK;
}

// ---
HRESULT CCodeGen::CreateIFaceDefString( CAPointer<char>& def_str ) {
  char guid[50];
  char f_name[MAX_PATH];
  int  len = m_iface.Guid( guid, sizeof(guid) );
 
  if ( 1 < len ) { // if ( 1 < m_iface.HeaderGUID(guid,sizeof(guid)) )
    --len;
    if ( guid[0] == BRACE_O )
      memcpy( guid, guid+1, --len );
    if ( guid[len-1] == BRACE_C )
      guid[--len] = 0;
    for( int i=0; i<len; i++ ) {
      if ( guid[i] == '-' )
        guid[i] = '_';
      else
        guid[i] = tolower( guid[i] );
    }
  }
  else
    *guid = 0;

  if ( 1 < m_iface.HeaderName(f_name,sizeof(f_name)) ) {
    ::_strlwr( f_name );
    char* dot = ::strstr( f_name, ".h" );
    if ( dot )
      *dot = 0;
  }
  else
    ::lstrcpy( f_name, "unknown_iface_file" );

	len = ::lstrlen(f_name) + ::lstrlen(guid)	+ 5;
	def_str = new char[len];
  ::wsprintf( def_str, "__%s__%s", f_name, guid );
  return S_OK;
}



// ---
DWORD CCodeGen::GetParamType( CMethodParamInfo& param, char* buffer, AVP_dword ilen ) { // , CPArray<char>* forwards ) {

  if ( !buffer != !ilen )
    return ERROR_INVALID_PARAMETER;

  AVP_dword s;
  AVP_dword l;
  BOOL      pointer = param.IsPointer();
  BOOL      dpointer = FALSE;
  BOOL      cnst;
  const char unknown_type[] = "unknown_type";
  const char const_type[] = "const ";
  CObjTypeInfo oti( m_types );

	if ( pointer ) {
		cnst = param.IsConst();
		dpointer = param.IsDoublePointer();
	}
	else {
		AVP_dword pid = ResolveTypeID( param );
		switch( pid ) {
			case tid_VOID:
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

  if ( pointer ) {
    s++;
		if ( dpointer )
			s++;
	}

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

  if ( pointer ) {
    *(buffer + s++ - 1) = '*';
		if ( dpointer )
			*(buffer + s++ - 1) = '*';
	}

  *(buffer + s - 1) = 0;

  return s;
}


// ---
HRESULT CCodeGen::PutErrors( COutputSect& sect, CPArray<char>& p, const char* header ) {
  
  *(DWORD*)W0 = MAKELONG( MAKEWORD('/','/'), MAKEWORD(' ',0) );
  
  if ( HIWORD(header) )
    ::lstrcpy( W0+3, header );
  else 
    ::LoadString( _Module.GetModuleInstance(), (UINT)header, B0(3) );

  PutString( sect, 0, W0 );

  *(DWORD*)(W0+3) = MAKELONG( MAKEWORD(' ',' '), MAKEWORD('-',' ') );
  for( uint i=0,c=p.Count(); i<c; i++ ) {
    ::lstrcpy( W0+7, (char*)p[i] );
    PutString( sect, 0, W0 );
  }
  return S_OK;
}





// ---
HRESULT CCodeGen::PutIFaceComment( COutputFile& file, COutputSect* to_sect ) {

  COutputSect& sect = to_sect ? *to_sect : *new COutputSect( file, if_comment, 0, false ); // IDS_IFACE_COMMENT_STAMP
    
  /*
  if ( m_iface.Guid(A2) ) { // m_iface.HeaderGUID(A2)
    ::strlwr( W2 );
    PutFmtString( sect, 0, MAKEINTRESOURCE(IDS_IFS_COMM_DELIMITER) );
    PutFmtString( sect, 0, MAKEINTRESOURCE(IDS_IFS_COMM_START), W2 );
    PutFmtString( sect, 0, MAKEINTRESOURCE(IDS_IFS_COMM_DELIMITER) );
  }
  */

  PutFmtString( sect, 0, MAKEINTRESOURCE(IDS_IFS_COMM_NAME), m_iface.Name() );

  if ( m_iface.ShortComment(A2) ) 
    PutFmtString( sect, 0,MAKEINTRESOURCE(IDS_IFS_COMM_SHORT_COMMENTS), W2 );

  if ( m_opt.ext_comments() && !to_sect ) 
    PutExtendedComment( sect, 0, 0, m_iface, VE_PID_LARGECOMMENT );

  return S_OK;
}



// ---
HRESULT CCodeGen::PutIFaceIdentifier( COutputFile& file, AVP_dword* out_id ) {

  // идентификатор интерфейса
  if ( !m_iface.MnemonicID(A2) ) 
    ::lstrcpy( W2, "unknown mnemonic id" );

  AVP_dword id = m_iface.IFaceID();
  
  COutputSect& sect = *new COutputSect( file, if_mn_id, 0, false ); // IDS_MNEMONIC_ID_STAMP
  if ( 0 == id ) {
    id = 0xfffffff;
    PutString( sect, 0, " ***** VE_PID_IF_DIGITALID isn't defined ***** " );
  }
  PutFmtString( sect, 0, "#define %s  ((tIID)(%u))", W2, id );
  
  if ( out_id ) 
    *out_id = id;

  return S_OK;
}



// ---
bool CCodeGen::HasMethodTable( DWORD table_id ) {

  if ( table_id == VE_PID_IF_METHODS ) 
    return 0 != m_iface.FirstPubMethod();
  else if ( table_id == VE_PID_IF_SYSTEMMETHODS ) {
    CIntMethodInfo mi( m_iface );
    while( mi ) {
      if ( mi.Selected() )
        return true;
      mi.GoNext();
    }
  }
  return false;
}

// ---
HRESULT CCodeGen::PutMethodExComment( COutputSect& sect, const CMethodInfo& mi ) {
	tUINT len;
	tUINT name_len = 0;
	tCHAR align[21];
	PutExtendedComment( sect, 0, "// Extended method comment", mi, VE_PID_LARGECOMMENT     );
	PutExtendedComment( sect, 0, "// Behaviour comment",       mi, VE_PID_BEHAVIORCOMMENT  );
	PutExtendedComment( sect, 0, "// Result comment",          mi, VE_PID_VALUECOMMENT     );

	CMethodParamInfo mpi( mi );
	while( mpi ) {
	  len = ::lstrlen( mpi.Name() );
	  if ( name_len < len )
		name_len = len;
	  mpi.GoNext();
	}

	PutString( sect, 0, "// Parameters are:" );
	mpi.GoFirst();
	while( mpi ) {
	  len = ::lstrlen( mpi.Name() );
	  align[0] = 0;
  
	  len = name_len - len;
	  if ( len ) {
		if ( len >= sizeof(align)-1 ) 
		  len = sizeof(align)-1;
		::memset( align, ' ', len );
	  }
	  align[len] = 0;

	  ::wsprintf( W2, "\"%s\"%s :", mpi.Name(), align );
	  PutExtendedComment( sect, 0, W2, mpi, VE_PID_LARGECOMMENT, false );
	  mpi.GoNext();
  }
	return S_OK;
}

// ---
bool CCodeGen::PutSeparator( COutputSect& sect, const CBaseInfo& item ) {

  if ( item.IsSeparator() ) {
    uint len;

    PutString( sect, 0, 0 );
    if ( m_opt.delimiter_page_break() )
      PutString( sect, 0, 0 );

    if ( 1 < (len=item.Name(A3)) ) {
      if ( m_opt.delimiter_comment() ) 
        PutFmtString( sect, 0, "// ----------  %s  ----------", W3 );
      else {
        *(DWORD*)W2 = MAKELONG( MAKEWORD('/','/'), MAKEWORD(' ',0) );
        ::memcpy( W2+3, W3, len );
        PutString( sect, 0, W2 );
      }
    }

    *(DWORD*)W3 = MAKELONG( MAKEWORD('/','/'), MAKEWORD(' ',0) );
    if ( 1 < (len=item.ShortComment(B3(3))) )
      PutString( sect, 0, W3 );
    if ( m_opt.ext_comments() && (1<(len=item.ExtComment(B3(3)))) ) {
      uint i, sl = 0;
      char* str = W3;
      len += 3;
      for( i=3; (i<len) && W3[i]; i++ ) {
        char next = 0;

        if ( W3[i] == '\n' )
          next = '\r';
        else if ( W3[i] == '\r' )
          next = '\n';
        if ( next ) {
          W3[i] = 0;
          PutString( sect, 0, str );
          if ( W3[i+1] == next )
            i++;
          str = W3 + i - 2;
          *(str+0) = '/';
          *(str+1) = '/';
          *(str+2) = ' ';
          sl = 0;
        }
        else
          sl++;
      }
      if ( sl )
        PutString( sect, 0, str );
    }
    return TRUE;
  }
  else
    return FALSE;
}


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
AVP_dword CCodeGen::ResolveTypeID( const CBaseInfo& item ) {
  AVP_dword type_of_type  = item.TypeOfType();
  
  if ( type_of_type == VE_NTT_GLOBALTYPE )
    return item.TypeID();

  else if ( type_of_type == VE_NTT_INTERFACE ) 
    return tid_IFACEPTR; // tid_OBJECT ???

  else if ( type_of_type != VE_NTT_USERTYPE ) // unknown type of type
    return 0;
  
  else  {
    char name[MAX_PATH];
    CTypeInfo ti( m_iface );
    AVP_dword if_count = 0;
    AVP_dword if_ind = 0;
    CNodeUID  if_uid;
    AVP_dword typedef_recursion_count = 0;
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
      
			if ( !m_additional_includes )
				m_additional_includes = ::DATA_Find_Prop( m_iface.Data(), 0, VE_PID_LINKEDUNIQUEID );
			if ( m_additional_includes )
				if_count = ::PROP_Arr_Count( m_additional_includes );
			else
				break;

      if ( if_ind >= if_count )
        break;

      if ( 1 >= ::PROP_Arr_Get_Items(m_additional_includes,if_ind++,if_uid,if_uid.size()) )
        break;

      if ( !m_additional_ifaces ) {
        if ( SUCCEEDED(::CoCreateInstance(CLSID_PragueIServer,0,CLSCTX_SERVER,IID_IPragueIServer,(LPVOID*)&m_additional_ifaces)) )
          m_additional_ifaces->Init();
        else
          break;
      }
			
      DWORD if_size = 0;
      BYTE* if_content = 0;
      if ( FAILED(m_additional_ifaces->LoadInterface(if_uid.len(),if_uid,FALSE,itPrototype,&if_size,&if_content)) || !if_content || !if_size ) {
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




// ---
/*
static HRESULT CheckTypesData( HDATA d, AVP_dword id1, AVP_dword id2 ) { 
	
	if ( !d )
		return CG_E_DATA_MATCH;
	
	HPROP names = ::DATA_Find_Prop( d, 0, AVP_PID_VALUE );
	HPROP ids   = ::DATA_Find_Prop( d, 0, id1 );
	HPROP defs  = ::DATA_Find_Prop( d, 0, id2 );
	
	if ( 
		!names || !ids || !defs || 
		!GET_AVP_PID_APP(::PROP_Get_Id(names)) || 
		!GET_AVP_PID_APP(::PROP_Get_Id(ids)) ||
		!GET_AVP_PID_APP(::PROP_Get_Id(defs)) 
		)
		return CG_E_DATA_MATCH;
	
	AVP_dword c = ::PROP_Arr_Count( names );
	if ( c != ::PROP_Arr_Count(ids) )
		return CG_E_DATA_MATCH;
	
	if ( c != ::PROP_Arr_Count(defs) )
		return CG_E_DATA_MATCH;
	
	return S_OK;
}
*/



// ---
HRESULT CCodeGen::SwitchFile( const char* new_file_name, const char* real_file_name, const char* back_file_name ) {
	DWORD err;
	if ( !::DeleteFile(back_file_name) ) {
		err = ::GetLastError();
		if ( (err != ERROR_FILE_NOT_FOUND) && (err!=ERROR_PATH_NOT_FOUND) )
			return CG_E_BACKUP_ERROR;
	}
	HANDLE h = ::CreateFile( real_file_name, 0, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( h != INVALID_HANDLE_VALUE ) {
		::CloseHandle( h );
		if ( !::MoveFile(real_file_name,back_file_name) )
			return CG_E_BACKUP_ERROR;
	}
	else {
		err = ::GetLastError();
		if ( (err != ERROR_FILE_NOT_FOUND) && (err!=ERROR_PATH_NOT_FOUND) )
			return CG_E_BACKUP_ERROR;
	}
	if ( ::MoveFile(new_file_name,real_file_name) )
		return S_OK;
	else
		return CG_E_BACKUP_ERROR;
}


tBOOL CCodeGen::CheckVersion( COutputFile &ofile, const char* file_name ) {
	COutputSect *sect = ofile.find(NULL, if_header);
	if( !sect )
		return cTRUE;

	if( !sect->Count() )
		return cTRUE;

	const char *str = (*sect)[0];
	int desc_pos = 0;

	for(int i=0; m_desriptions[i]; i++)
		if( strstr(str, m_desriptions[i]) ) {
			desc_pos = i;
			break;
		}

	if( desc_pos == m_desc_id )
		return cTRUE;

	sprintf(W0, "Warning: File \"%s\" was generated by \"%s\".\nBy changing code generator, you can have some problems.\nSelect OK to continue.",
		file_name, m_desriptions[desc_pos] );

	int result = ::MessageBox( GetActiveWindow(), W0, m_desriptions[m_desc_id], MB_OKCANCEL );
	if( result == IDCANCEL )
		return cFALSE;

	m_change_version = true;
	return cTRUE;
}

// ---
tBOOL CCodeGen::CheckFile( const char* name ) {
	tDWORD attr = ::GetFileAttributes( name );
	tDWORD err = GetLastError();
	HANDLE h = INVALID_HANDLE_VALUE;
	tBOOL  res = TRUE;
	static char text[0x200];
		
	if ( (attr == 0xffffffff) && ((ERROR_FILE_NOT_FOUND == err) || (ERROR_PATH_NOT_FOUND == err)) )
		return TRUE;

	if (!CheckOutFile(name))
		return FALSE;

	attr = ::GetFileAttributes( name );
	
	if ( FILE_ATTRIBUTE_READONLY & attr ) {
		::wsprintf( text, "File \"%s\" is \"read only\". Try again ?", name );
		res = FALSE;
	}
	else if ( INVALID_HANDLE_VALUE == (h=CreateFile(name,GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0)) ) {
		::wsprintf( text, "File \"%s\" is already open. Try again ?", name );
		res = FALSE;
	}
	else 
		res = TRUE;
	
	while( !res ) {
		int result = ::MessageBox( GetActiveWindow(), text, "C code generator", MB_YESNO );
		if ( result == IDNO )
			break;
		attr = ::GetFileAttributes( name );
		if ( FILE_ATTRIBUTE_READONLY & attr ) {
			::wsprintf( text, "File \"%s\" is still \"read only\". Try again ?", name );
			continue;
		}
		
		h = CreateFile( name, GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
		if ( h == INVALID_HANDLE_VALUE ) {
			::wsprintf( text, "File \"%s\" is still open. Try again ?", name );
			continue;
		}
		
		res = TRUE;
		break;
	}
	
	if ( h != INVALID_HANDLE_VALUE )
		CloseHandle( h );
	
	return res;
}



