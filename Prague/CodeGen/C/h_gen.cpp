#include "StdAfx.h"
#include "CodeGen.h"
#include "resource.h"
#include <datatreeutils/dtutils.h>
#include <version\ver_kl.h>
#include <time.h>
#include "options.h"
#include "..\AVPPveID.h"
#include "..\PCGError.h"
#include <stuff\cpointer.h>
#include <pr_err.h>
#include <pr_types.h>
#include <hashutil/hashutil.h>

#define PAGE_WITDH (50)


// ---
inline char* dups( const char* str, int len = 0 ) {
	if ( !len ) {
		if ( !str || !*str )
			return 0;
		len = strlen( str ) + 1;
		return (char*)memcpy( new char[len], str, len );
	}
	else if ( str[len-1] == 0 )
		return (char*)memcpy( new char[len], str, len );
	else {
		char* ret = (char*)memcpy( new char[len+1], str, len );
		ret[len] = 0;
		return ret;
	}
}




// ---
struct cBuffStr {
	char* m_buff;
	uint  m_len;
	
	cBuffStr( char* buff ) : m_buff(buff) { clean(); }
	void clean() { *m_buff = 0; m_len = 0; }

	uint operator += ( const char* str );
	uint operator += ( int ch );
	operator char* () { return m_buff; }
	operator const char* () const { return m_buff; }
};



// ---
inline uint cBuffStr::operator += ( const char* str ) {
	if ( !str || !*str )
		return m_len;
	uint slen = strlen( str );
	memcpy( m_buff+m_len, str, slen+1 );
	return m_len += slen;
}




// ---
inline uint cBuffStr::operator += ( int ch ) {
	*(m_buff+m_len++) = ch;
	*(m_buff+m_len)   = 0;
	return m_len;
}




// ---
CIfaceParam::CIfaceParam( const char* type, const char* name ) 
	: //m_const( cnst ),
		m_type( dups(type) ),
		//m_pointer( pointer ),
		m_name( dups(name) ) {
}



// ---
CIfaceParam::~CIfaceParam() {
	m_type = 0;
	m_name = 0;
}




// ---
CIfaceMethod::CIfaceMethod( CMethodInfo& mi, CObjTypeInfo& types ) {
	tUINT     len;
	AVP_dword type_id = mi.TypeID();
	AVP_dword to_type = mi.TypeOfType();
	
	if ( 1 >= (len=mi.Name(A3)) )
		m_name = 0;
	else
		m_name = dups( W3, len );
	
	if ( 1 < mi.ShortComment(A3) )
		m_comment = dups( W3 );
	
  if ( 1 >= (len=mi.TypeName(&types,A0)) ) {
    type_id = tid_VOID;
    len = ::lstrlen( UNKNOWN_RESULT_TYPE ) + 1;
    ::memcpy( W0, UNKNOWN_RESULT_TYPE, len );
  }
	else
		strcat( W0, "*" );
	
  if ( HAS_RET_VAL(type_id,to_type) )
		m_params.Add( new CIfaceParam(W0,"result") );
	
}



// ---
CIfaceMethod::~CIfaceMethod() {
	m_name = 0;
	m_params.Flush( Delete );
}



// ---
HRESULT CCodeGen::CppMethodToHdr( COutputSect& sect, CIfaceMethod& meth, uint shift ) {
	char* name = meth.m_name;
	cBuffStr str( W3 );
	if ( !name || !*name ) 
		str += "#error method name not defined";
	else {
		CPArray<CIfaceParam>& params = meth.m_params;
		//*name = tolower( *name );
		if ( !strcmp(name,"delete") )
			name = "remove";
		str += "virtual tERROR pr_call ";
		str += name;
		str += BRACKET_O;
		uint j, k;
		for( j=0,k=params.Count(); j<k; j++ ) {
			CIfaceParam& p = *params[j];
			if ( j )
				str += COMMA;
			str += SPACE;
      if ( strcmp(p.m_name,"...") ) {
        str += p.m_type;
        str += SPACE;
        str += p.m_name;
      }
      else
        str += p.m_name;
		}
		if ( j )
			str += ' ';
		str += BRACKET_C;
		str += " = 0";
		str += SEMICOLON;
		if ( meth.m_comment ) {
			str += " // -- ";
			str += meth.m_comment;
		}
	}
	return PutString( sect, shift, str );
}



// ---
void format_property_get( cBuffStr& str, const char* type, const char* func_name, const char* name ) {
	// tOUTPUT_FUNC pr_call get_pgOUTPUT_FUNC() { return (tOUTPUT_FUNC)getPtr(pgOUTPUT_FUNC); }
	str += type;
	str += " pr_call get_";
	str += name;
	str += BRACKET_O;
	str += BRACKET_C;
	str += SPACE;
	str += BRACE_O;
	str += " return ";
	str += BRACKET_O;
	str += type;
	str += BRACKET_C;
	str += "get";
	str += func_name;
	str += BRACKET_O;
	str += name;
	str += BRACKET_C;
	str += SEMICOLON;
	str += SPACE;
	str += BRACE_C;
}
	



// ---
void format_property_set( cBuffStr& str, const char* type, const char* func_name, const char* cust, const char* name ) {
	// tERROR pr_call set_pgOUTPUT_FUNC( tFUNC_PTR func ) { return setPtr(pgOUTPUT_FUNC,(tPTR)func); }
	str += "tERROR pr_call set_";
	str += name;
	str += BRACKET_O;
	str += SPACE;
	str += type;
	str += " value ";
	str += BRACKET_C;
	str += SPACE;
	str += BRACE_O;
	str += " return set";
	str += func_name;
	str += BRACKET_O;
	str += name;
	str += COMMA;
	str += BRACKET_O;
	str += cust;
	str += BRACKET_C;
	str += "value";
	str += BRACKET_C;
	str += SEMICOLON;
	str += SPACE;
	str += BRACE_C;
}



// ---
void format_str( cBuffStr& str, bool get, const char* type, const char* name ) {
	//tERROR pr_call get_pgOBJECT_NAME( tPTR* buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgOBJECT_NAME,buff,size,cp); }
	//tERROR pr_call set_pgOBJECT_NAME( tPTR* buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgOBJECT_NAME,buff,size,cp); }
	
	str += "tERROR pr_call ";
	str += get ? "get_" : "set_";
	str += name;
	str += BRACKET_O;
	str += " tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ";
	str += BRACKET_C;
	str += SPACE;
	str += BRACE_OS " return ";
	str += get ? "getStr" : "setStr";
	str += BRACKET_O; 
	str += name;
	str += COMMA;
	str += "buff,size,cp";
	str += BRACKET_C;
	str += SEMICOLON;
	str += SPACE;
	str += BRACE_CS;
}




// ---
void format_ordinary_get_strong_size( cBuffStr& str, bool get, const char* type, const char* name ) {
	//tERROR pr_call get_pgTIME_STAMP( tDATETIME* value ) { return get(pgTIME_STAMP,value,sizeof(tDATETIME)); }
	//tERROR pr_call set_pgTIME_STAMP( tDATETIME* value ) { return set(pgTIME_STAMP,value,sizeof(tDATETIME)); }

	char sg = get ? 'g' : 's';
	str += "tERROR pr_call ";
	str += sg;
	str += "et_";
	str += name;
	str += BRACKET_O;
	str += SPACE;
	str += type;
	str += '*';
	str += SPACE;
	str += "value";
	str += SPACE;
	str += BRACKET_C;
	str += SPACE;
	str += BRACE_O;
	str += SPACE;
	str += "return ";
	str += sg;
	str += "et";
	str += BRACKET_O;
	str += name;
	str += COMMA;
	str += "value,sizeof";
	str += BRACKET_O;
	str += type;
	str += BRACKET_C;
	str += BRACKET_C;
	str += SEMICOLON;
	str += SPACE;
	str += BRACE_C;
}



// ---
void format_ordinary_get_soft_size( cBuffStr& str, bool get, const char* type, const char* name ) {
	//tERROR pr_call get_pgIFACE_DATA( tPTR value, tDWORD size ) { return get(pgIFACE_DATA,value,size); }
	//tERROR pr_call set_pgIFACE_DATA( tPTR value, tDWORD size ) { return set(pgIFACE_DATA,value,size); }
	char sg = get ? 'g' : 's';
	str += "tERROR pr_call ";
	str += sg;
	str += "et_";
	str += name;
	str += BRACKET_O;
	str += " tPTR value, tDWORD size ";
	str += BRACKET_C;
	str += SPACE;
	str += BRACE_O;
	str += SPACE;
	str += "return ";
	str += sg;
	str += "et";
	str += BRACKET_O;
	str += name;
	str += COMMA;
	str += "value,size";
	str += BRACKET_C;
	str += SEMICOLON;
	str += SPACE;
	str += BRACE_C;
}



// ---
void format_ordinary( cBuffStr& str, bool get, const char* type, const char* name ) {
	//tObjPtrState pr_call get_pgOBJECT_PTR_STATE() { tObjPtrState value = {0}; get(pgOBJECT_PTR_STATE,&value,sizeof(value)); return value; }
	//tERROR       pr_call set_pgOBJECT_PTR_STATE( tObjPtrState value ) { return set(pgOBJECT_PTR_STATE,&value,sizeof(value)); }

	if ( get ) {
		str += type;
		str += " pr_call get_";
		str += name;
		str += BRACKET_O;
		str += BRACKET_C;
		str += SPACE;
		str += BRACE_O;
		str += SPACE;
		str += type;
		str += " value = {0}; get";
		str += BRACKET_O;
		str += name;
		str += COMMA;
		str += "&value,sizeof(value)";
		str += BRACKET_C;
		str += "; return value; ";
		str += BRACE_C;
	}
	else {
		str += "tERROR pr_call set_";
		str += name;
		str += BRACKET_O;
		str += SPACE;
		str += type;
		str += " value ";
		str += BRACKET_C;
		str += SPACE;
		str += BRACE_OS " return set";
		str += BRACKET_O; 
		str += name;
		str += COMMA;
		str += "&value,sizeof(value)";
		str += BRACKET_C;
		str += SEMICOLON;
		str += SPACE;
		str += BRACE_CS;
	}
}




/*
!!! still not used !!!

#define pTYPE_DATETIME      ( tid_DATETIME    << pTYPE_SHIFT )
#define pTYPE_BINARY        ( tid_BINARY      << pTYPE_SHIFT )

#define pTYPE_EXPORT        ( tid_EXPORT      << pTYPE_SHIFT )
#define pTYPE_IMPORT        ( tid_IMPORT      << pTYPE_SHIFT )
#define pTYPE_TRACE_LEVEL   ( tid_TRACE_LEVEL << pTYPE_SHIFT )
*/


// ---
HRESULT CCodeGen::CppPropToHdr( COutputSect& sect, CPropInfo& prop, CPropTypeInfo& types, uint shift ) {
	
	if ( prop.IsSeparator() )
		return S_OK;

	void (*format)( cBuffStr& str, bool get, const char* type, const char* name ) = 0;

	tDWORD pid = prop.DigitalID();
	tDWORD type_id = prop.TypeID();
	tPROPID prop_id = mPROPERTY_MAKE_GLOBAL( (type_id<<pTYPE_SHIFT), pid );

	if ( (prop_id == pgINTERFACE_VERSION) || (prop_id == pgINTERFACE_COMMENT) )
		return S_OK;

	W1[0] = 'p';
	W1[1] = prop.Native() ? 'g' : 'l';
	prop.Name( &W1[2], sizeof(W1)-2 );
	prop.TypeName( &types, A2 );
	
	cBuffStr str( W3 );
	const char* fname = 0;
	const char* cust;
		
	switch( type_id ) {
		case tid_BOOL:     fname = "Bool";  cust = "tBOOL";   break;
		case tid_BYTE:
		case tid_SBYTE:
		case tid_CHAR:     fname = "Byte";  cust = "tBYTE";   break;
		case tid_WCHAR:
		case tid_SHORT:
		case tid_WORD:		 fname = "Word";  cust = "tWORD";   break;
		case tid_INT:		   
		case tid_UINT:		 fname = "UInt";  cust = "tUINT";   break;
		case tid_ERROR:
		case tid_LONG:
		case tid_CODEPAGE:
		case tid_LCID:
		case tid_IID:
		case tid_PID:
		case tid_ORIG_ID:
		case tid_OS_ID:
		case tid_VID:
		case tid_PROPID:
		case tid_VERSION:
		case tid_DATA:
		case tid_DWORD:		 fname = "DWord"; cust = "tDWORD";  break;
		case tid_LONGLONG:
		case tid_QWORD:		 fname = "QWord"; cust = "tQWORD";  break;
		case tid_OBJECT:   fname = "Obj";   cust = "hOBJECT"; break;
		case tid_IFACEPTR:
		case tid_FUNC_PTR:
		case tid_PTR:      fname = "Ptr";   cust = "tPTR";    break;
		case tid_STRING:
		case tid_WSTRING:  format = format_str;               break;
		default:
			switch( type_id ) {
				case tid_DATETIME:
				case tid_EXPORT  :
				case tid_IMPORT  : format = format_ordinary_get_strong_size; break;
				case tid_BINARY  : format = format_ordinary_get_soft_size;   break;
				default          : format = format_ordinary;                 break;
			}
			fname = cust = 0;
			break;
	}

	if ( format ) {
		format( str, true, W2, W1 );
		PutString( sect, shift, str );
		str.clean();
		format( str, false, W2, W1 );
		PutString( sect, shift, str );
	}
	else if ( fname ) {
		format_property_get( str, W2, fname, W1 );
		PutString( sect, shift, str );
		str.clean();
		format_property_set( str, W2, fname, cust, W1 );
		PutString( sect, shift, str );
	}
	return PutString( sect, 0, 0 );
}



// ---
HRESULT CCodeGen::CppDefinesToHdr( COutputFile& ofile ) {
	// make cpp declarations
	CPArray<CIfaceMethod> methods;
	CollectMethods( methods );
	
	uint c = methods.Count();
	//if ( c ) {
		const char* i_name = m_iface.Name();
		COutputSect& sect = *new COutputSect( ofile, if_cpp_decl, 0, false );
		
		PutString( sect, 0, "#if defined (__cplusplus) && !defined(_USE_VTBL)" );
    if ( c ) {
      PutFmtString( sect, 2, "struct pr_abstract i%s "BRACE_OS, i_name );
		  for( uint i=0; i<c; i++ )
			  CppMethodToHdr( sect, *methods[i], 4 );
      PutString( sect, 2, BRACE_CS";" );
      PutString( sect, 0, 0 );
		  PutFmtString( sect, 2, "struct pr_abstract c%s : public i%s, public iObj "BRACE_OS, i_name, i_name );
    }
    else {
      PutString( sect, 0, 0 );
      PutFmtString( sect, 2, "struct c%s : private iEmpty, public iObj "BRACE_OS, i_name );
    }

		PutString( sect, 0, 0 );
		PutFmtString( sect, 4, "OBJ_IMPL( c%s );", i_name );
		PutString( sect, 0, 0 );
		PutString( sect, 4, "operator hOBJECT() { return (hOBJECT)this; }" );
		m_iface.TypeName( 0, A3 );
		PutFmtString( sect, 4, "operator %s()   { return (%s)this; }", W3, W3 );
		PutString( sect, 0, 0 );

		CPropInfo pi( m_iface );
    CPropTypeInfo pti(m_types);

		for( ; pi; pi.GoNext() )
			CppPropToHdr( sect, pi, pti, 4 );
		
		PutString( sect, 2, BRACE_CS";" );
		PutString( sect, 0, 0 );

		PutString( sect, 0, "#endif // if defined (__cplusplus) && !defined(_USE_VTBL)");
	//}
	return S_OK;	
}



// ---
HRESULT CCodeGen::InterfaceToHdr( COutputFile& ofile ) {

  AVP_dword count = 0;
  AVP_dword id;

  IncludesToHdr( ofile );
  PutIFaceComment( ofile );
  PutIFaceIdentifier( ofile, &id );
  TypesToHdr( ofile );
  ConstsToHdr( ofile, true );
	MsgClassesToHdr( ofile, true );
  ErrorsToHdr( ofile );

  const char* i_name = m_iface.Name();
  m_iface.TypeName( 0, A3 );
	
	// RPC ifdef
	COutputSect* sect = new COutputSect( ofile, if_rpc_def_first, 0, false );
	PutString( *sect, 0, "#if defined( __RPC__ )" );
	PutFmtString( *sect, 2, "typedef tUINT %s;", W3 );
	PutString( *sect, 0, "#else" );

  // forward declaration
  sect = new COutputSect( ofile, if_forw_decl, 0, false ); // IDS_FORWARD_IFACE_DECL_STAMP
  if ( FAILED(PutFmtString(*sect,0,"struct i%sVtbl;",i_name)) ) 
    PutString( *sect, 0, " ***** Cannot put forward interface declaration, example: struct iIOVtbl;" );
  if ( FAILED(PutFmtString(*sect,0,"typedef struct i%sVtbl i%sVtbl;",i_name,i_name)) ) 
    PutString( *sect, 0, " ***** Cannot put forward interface declaration, example: typedef struct iIOVtbl iIOVtbl;" );
	
  char iface_alignment[6];
  char system_alignment[20];
  ::memset( iface_alignment, 0, sizeof(iface_alignment) );
  ::memset( system_alignment, 0, sizeof(system_alignment) );

  AVP_dword len = ::lstrlen( i_name );
  if ( len > 6 )  // sizeof( "SYSTEM" );
    ::memset( system_alignment, ' ', min(sizeof(system_alignment)-1,(len-6)) );
  else if ( len < 6 ) 
    ::memset( iface_alignment, ' ', 6-len );

  // main interface declaration
  sect = new COutputSect( ofile, obj_decl, 0, false ); // IDS_OBJECT_DECL_STAMP
  PutString( *sect, 0, "#if defined( __cplusplus ) && !defined(_USE_VTBL)" );
  PutFmtString( *sect, 2, "struct c%s;", i_name );
  PutFmtString( *sect, 2, "typedef c%s* %s;", i_name, W3);
  PutString( *sect, 0, "#else" );
  if ( 
    FAILED(PutFmtString(*sect,2,"typedef struct tag_%s "BRACE_OS,W3)) || 
    FAILED(PutFmtString(*sect,4,"const i%s%s* %s%s; %s// pointer to the \"%s\" virtual table",i_name,"Vtbl",iface_alignment,"vtbl","",i_name)) ||
    FAILED(PutFmtString(*sect,4,"const iSYSTEMVtbl* %ssys;  // pointer to the \"SYSTEM\" virtual table",system_alignment)) ||
    FAILED(PutFmtString(*sect,2,BRACE_CS" *%s;", W3))
  ) {
    PutString( *sect, 0, " ***** Cannot put interface declaration, example :" );
    PutString( *sect, 0, " ***** typedef struct tag_hIO "BRACE_OS );
    PutString( *sect, 0, " *****   const iIOVtbl*     vtbl; // pointer to the \"IO\" virtual table" );
    PutString( *sect, 0, " *****   const iSYSTEMVtbl* sys;  // pointer to the \"SYSTEM\" virtual table" );
    PutString( *sect, 0, " ***** "BRACE_CS" *hIO;" );
  }
  PutString( *sect, 0, "#endif // if defined( __cplusplus )" );

	// RPC ifdef
	sect = new COutputSect( ofile, if_rpc_def_last, 0, false );
	PutString( *sect, 0, "#endif // if defined( __RPC__ )" );
	
  CPArray<char> defines;
  VtblToHdr( ofile, defines, false );  // interface vtbl declaration

  sect = new COutputSect( ofile, if_props, 0, false ); // IDS_IFACE_PROPERTIES
  CPropInfo pi( m_iface );
  DWORD ws[2] = { 0, 0 };
  while( pi ) {
    CalcPropWidths( pi, true, ws ); // only native and for_hdr
    pi.GoNext();
  }
  pi.GoFirst();
  while( pi ) {
    PropToInclude( *sect, pi, true, ws ); // only native and for_hdr
    pi.GoNext();
  }

  sect = new COutputSect( ofile, if_defines, 0, false ); // IDS_IFACE_CALL_DEFINES
  PutString( *sect, 0, "#if !defined( __cplusplus ) || defined(_USE_VTBL)" );
	uint i, c;
  for( i=0,c=defines.Count(); i<c; i++ )
    PutString( *sect, 2, defines[i] );
  PutString( *sect, 0, "#else // if !defined( __cplusplus )" );
  for( i=0,c=defines.Count(); i<c; i++ )
  {
	char* ptr = strstr(defines[i], "->vtbl->");
	if (ptr)
	{
		int len, substr_len;
		ptr+=2;
		len = ::lstrlen(ptr)+1;
		::memmove(ptr, ptr+6, len-6);
    ptr = strstr(defines[i], "( (_this),");
    if ( ptr )
      substr_len = 10;
    else {
      ptr = strstr(defines[i], "( (_this)");
      substr_len = 9;
    }
    if ( ptr ) {
      len = ::lstrlen(ptr)+1;
      ::memmove(ptr+1, ptr+substr_len, len-substr_len);
      PutString( *sect, 2, defines[i] );
    }
    else
      PutFmtString( *sect, 0, "***** Cannot convert C define to CPP: %s", defines[i] );
	}
	else
	  PutFmtString( *sect, 0, "***** Cannot convert C define to CPP: %s", defines[i] );
  }
  PutString( *sect, 0, "#endif // if !defined( __cplusplus )" );

  // make cpp declarations
  CppDefinesToHdr( ofile );

  return S_OK;
}



// ---
HRESULT CCodeGen::HeaderToHdr( COutputFile& ofile, const char* file_name, const char* def_str, bool header ) {
  // header of hdr file

	if ( header ) {
		time_t now = ::time( 0 );
		tm* tm = ::localtime( &now );
		::strftime( A0, "// -------- %A,  %d %B %Y,  %H:%M --------", tm );
		
		COutputSect& sect = *new COutputSect( ofile, if_header, 0, false );
		
		PutString( sect, 0, W0 );
		
		ExtractFileName( file_name, W3 );
		PutFmtString( sect, 0, "// File Name   -- %s", W3 ); // IDS_HDR_INC_FNAME_FORMAT
		
		::lstrcpy( W0, "// " );
		::lstrcat( W0, VER_LEGALCOPYRIGHT_STR );
		PutString( sect, 0, W0 );
		PutString( sect, 0, "// -------------------------------------------" );
	}

  COutputSect& sect = *new COutputSect( ofile, h_if_def, 0, false );
  PutFmtString( sect, 0, "#if !defined( %s )", def_str );
  PutFmtString( sect, 0, "#define %s", def_str );

  return S_OK;
}




// ---
HRESULT CCodeGen::IncludesToHdr( COutputFile& ofile ) {

  AVP_dword i;
  COutputSect* sect = ofile.find( 0, h_includes );
	if ( !sect )
		sect = new COutputSect( ofile, h_includes, 0, false );
  
	PutString( *sect, 0, "#include <pr_vtbl.h>" );

  if ( m_iface.HeaderName(A3) ) 
    ::_strlwr( W3 );
  else
    W3[0] = 0;

  AVP_dword l;
  for( i=0; 0 != (l=m_iface.IncludeFile(i,A2)); i++ ) {
    char* p = W2;
    l--;
    while( l && *p && (*p==' ') )
      p++, l--;
    if ( l ) {
      char* p2 = p + l - 1;
      while( p2>=p && *p==' ' ) {
        *p = 0;
        p--,l--;
      }
      if ( (*p == BRACE_O) && (*p2 == BRACE_C) )
        continue;
      if ( p2 >= p ) {
        ::_strlwr( p );
        if ( ::lstrcmp(p,W3) ) {
          const char* fmt;
          const char* add;
          if ( p[0] == '\"' || p[0] == '<' ) {
            fmt = "#include %s%s";
            add = "";
          }
          else {
            fmt = "#include <%s%s.h>";
            add = m_inc_prefix;
          }
          PutFmtString( *sect, 0, fmt, add, p );
        }
      }
    }
  }
  return S_OK;
}


// ---
HRESULT CCodeGen::TypeToHdr( COutputSect &sect, CTypeInfo &ti, bool is_comment ) {

  CObjTypeInfo		oti( m_types );
  DWORD						prev_id = ~0;
  CAPointer<char> comment;
  char  align[31];
  AVP_dword type_id = ti.TypeID();
	AVP_dword len;

  if ( (prev_id == (~0)) || (prev_id != type_id) ) {
    DWORD len;
    prev_id = type_id;
    if ( 1 >= (len=ti.TypeName(&oti,A2)) )
      len = ::lstrlen( ::lstrcpy(W2,"unknown_base_type") ) + 1;
    if ( len < 31 ) {
      ::memset( align, ' ', 31-len );
      align[31-len] = 0;
    }
    else
      align[0] = 0;
  }
  if ( 1 >= ti.Name(A3) ) 
    ::lstrcpy( W3, "unknown_derivative_type" );

	if ( 1 <= (len=ti.ShortComment(A1)) )
		comment = (char*)::memcpy( new char[len], W1, len );
	else
		comment = (char*)::memcpy( new char[4], " --", 4 );
	
  PutFmtString( sect, 0, "%stypedef %s %s%s; // %s", is_comment ? "// " : "", W2, align, W3, (const char*)comment );
	return S_OK;
}

// ---
HRESULT CCodeGen::TypesToHdr( COutputFile& ofile ) {

  COutputSect&  sect = *new COutputSect( ofile, if_typedefs, 0, false ); // IDS_TYPEDEFS_STAMP

  for(CTypeInfo ti( m_iface ); ti; ti.GoNext() )
		TypeToHdr(sect, ti, false);

  return S_OK;
}


// ---
HRESULT CCodeGen::ConstToHdr( COutputSect &sect, CConstantInfo &ci, bool for_prototype, bool is_comment ) {

  if ( (!for_prototype && ci.IsNative()) || PutSeparator(sect,ci) )
		return S_OK;

  CObjTypeInfo    oti( m_types );
  CAPointer<char> val;
  CAPointer<char> comment;

  AVP_dword len;
  AVP_dword type_id = ci.TypeID();
  char align[31];

  if ( 1 >= (len = ci.Value(A2)) ) 
    len = ::lstrlen( ::lstrcpy(W2,"unknown_constant_value") ) + 1;

  if ( 1 >= ci.TypeName(&oti,A3) )
    ::lstrcpy( W3, "unknown_base_type" );

  if ( (type_id == (AVP_dword)tid_STRING || type_id == (AVP_dword)tid_WSTRING) && W2[0] != '\"' ) {
    ::memmove( W2+1, W2, len );
    W2[0] = '\"';
    *((WORD*)(W2+len)) = MAKEWORD( '\"', 0 );
    len += 2;
  }

  val = (char*)::memcpy( new char[len], W2, len );

  if ( 1 >= (len=ci.Name(A2)) ) 
    len = ::lstrlen( ::lstrcpy(W2,"unknown_constant_name") );

  if ( len < 31 ) {
    ::memset( align, ' ', 31-len );
    align[31-len] = 0;
  }
  else
    align[0] = 0;

	if ( 1 <= (len=ci.ShortComment(A1)) )
		comment = (char*)::memcpy( new char[len], W1, len );
	else
		comment = (char*)::memcpy( new char[4], " --", 4 );

  PutFmtString( sect, 0, "%s#define %s %s((%s)(%s)) // %s",
		is_comment ? "//! " : "", W2, align, W3, (const char*)val, (const char*)comment );
	return S_OK;
}

// ---
HRESULT CCodeGen::ConstsToHdr( COutputFile& ofile, bool for_prototype ) {

  COutputSect& sect = *new COutputSect( ofile, if_constants, 0, false ); // IDS_CONSTANTS_STAMP
	
  for( CConstantInfo ci( m_iface ); ci; ci.GoNext() )
		ConstToHdr(sect, ci, for_prototype, false);

  return S_OK;
}

// ---
HRESULT CCodeGen::ErrorToHdr( COutputSect &sect, CErrInfo &ei, AVP_dword &cur_id, bool is_comment, bool from_iface ) 
{
	if( PutSeparator(sect,ei) )
		return S_OK;
	
	CAPointer<char> comment;
	AVP_dword err_code;
	AVP_dword len;
	char align[41];
	AVP_dword id;
	char mnem_id[0x100];
	char define_name[0x40];
	AVP_dword error_val;
	
	if ( 1 >= (len=ei.Name(A3)) ) 
		len = ::lstrlen( ::lstrcpy(W3,"unknown_error_name") ) + 1;
	
	if ( 0 == (err_code=ei.Value()) )
		err_code = cur_id++;
	
	if ( len < sizeof(align) ) {
		::memset( align, ' ', sizeof(align)-len );
		align[sizeof(align)-len] = 0;
	}
	else
		align[0] = 0;
	
	if ( 1 <= (len=ei.ShortComment(A1)) )
		comment = (char*)::memcpy( new char[len], W1, len );
	else
		comment = (char*)::memcpy( new char[1], "", 1 );
	

	strcpy(define_name, "PR_MAKE_");
	strcat(define_name, from_iface ? "DECL_" : "IMP_");
	strcat(define_name, ei.IsWarning() ? "WARN" : "ERR");
	if (from_iface)
	{
		m_iface.MnemonicID(mnem_id, sizeof(mnem_id)-1);
		id = m_iface.IFaceID();
	}
	else
	{
		m_plugin.MnemonicID(mnem_id, sizeof(mnem_id)-1);
		id = m_plugin.PluginID();
	}
	error_val = PR_MAKE_ERR( (ei.IsWarning() ? SEV_WARN : SEV_ERROR),  (from_iface ? fac_PREDECLARED : fac_IMPLEMENTATION_DECLARED), id, err_code);
		
	PutFmtString( sect, 0, "#define %s %s%s(%s, 0x%03x) // 0x%08x,%d (%u) -- %s", 
		W3, align, define_name, mnem_id, err_code, error_val, error_val, PR_ERR_CODE(error_val), comment );
	
	cur_id++;
	return S_OK;
}

// ---
HRESULT CCodeGen::ErrorsToHdr( COutputFile& ofile ) {

  COutputSect&  sect = *new COutputSect( ofile, if_errors, 0, false ); // IDS_ERRORS_STAMP
  DWORD         next = 1;

  for(CErrInfo ei( m_iface ); ei; ei.GoNext())
		ErrorToHdr(sect, ei, next, false, true);
  return S_OK;
}



HRESULT CCodeGen::ParseIFaceMethods( CPArray<char>&names, CPArray<char>&typedefs, CPArray<char>& methods, CPArray<char>& defines, bool internal, bool extended )
{
  CPubMethodInfo mi( m_iface );

  if ( mi ) {
    DWORD widths[5] = { 0 };

    for( ;mi; mi.GoNext()) {
			if( extended && !mi.IsExtended() )
				continue;

      CalcMethodsWidths( mi, widths, internal ); // , forwards );
    }
    
    if ( (widths[0]+widths[1]+widths[2]) > PAGE_WITDH ) {
      if ( (widths[0] + widths[1]) > PAGE_WITDH ) {
        if ( widths[0] > PAGE_WITDH ) {
          widths[0] = PAGE_WITDH;
          widths[1] = widths[1] = 0;
        }
        else {
          widths[1] = PAGE_WITDH - widths[0];
          widths[2] = 0;
        }
      }
      else
        widths[2] = PAGE_WITDH - (widths[0] + widths[1]);
    }

    for(mi.GoFirst(); mi; mi.GoNext()) {
			int i;
			AVP_dword len = 0;
			AVP_dword m_len = 0;

			if( extended && !mi.IsExtended() )
				continue;

			if ( mi.IsSeparator() ) {
    
				DWORD len;
    
				typedefs.Add( (char*)::memcpy(new char[1],"",1) );
				methods.Add( 0 );
				names.Add( 0 );
				/*
				if ( m_opt.delimiter_comment() ) {
					len = ::lstrlen( COutputSect::g_delimiter_line ) + 1;
					typedefs.Add( (char*)::memcpy(new char[len],COutputSect::g_delimiter_line,len) );
					methods.Add( 0 );
					names.Add( 0 );
				}
				*/

				::lstrcpy( W3, "// ----------- " );
				::lstrcat( W3, mi.Name() );
				::lstrcat( W3, " ----------" );
				len = ::lstrlen( W3 ) + 1;
				typedefs.Add( (char*)::memcpy(new char[len],W3,len) );
				methods.Add( 0 );
				names.Add( 0 );

			}
			else {
				if ( widths[0] > widths[1] )
					i = widths[0];
				else
					i = widths[1];
				if ( i < (int)widths[2] )
					i = widths[2];
				if ( i < (int)widths[3] )
					i = widths[3];

				CAPointer<char> d( new char[100+i] );
				char* m = new char[widths[4]+100];

				*((WORD*)m) = MAKEWORD( ' ', ' ' );
				::memcpy( m+2, "tERROR", 7 );

				for( i=0; i<((int)(((int)widths[0])-((int)6))); i++ )
					*(d+i) = ' ';
				::memcpy( d+i, " (pr_call *", 12 );
				i += 12;
				::lstrcat( m, d );

				if ( 1 >= (m_len=mi.Name(A1)) )
					m_len = ::lstrlen( ::lstrcpy(W1,UNKNOWN_METHOD_NAME) ) + 1;

				names.Add( (char*)::memcpy(new char[m_len],W1,m_len) );

				len = ::wsprintf( W3, "%s%s_%s", extended ? m_plugin.Name() : "", m_iface.Name(), W1 ) + 1;
				typedefs.Add( (char*)::memcpy(new char[len],W3,len) );

				len = ::wsprintf( W2, "fnp%s", W3 ) + 1;
				::lstrcat( m, W2 );
				::lstrcat( m, ")" );

				for( i=0; i<((int)(((int)widths[1])-((int)len))); i++ )
					*(d+i) = ' ';
				*(d+i++) = '(';
				*(d+i++) = ' ';
				*(d+i++) = 0;
				::lstrcat( m, d );

				CObjTypeInfo ti(m_types);

				if ( internal )
					::lstrcat( m, m_int_type_name );
				else {
					m_iface.TypeName( 0, A2 );
					::lstrcat( m, W2 );
				}

				::lstrcat( m, " _this" );

				AVP_dword type_id = mi.TypeID();
				AVP_dword to_type = mi.TypeOfType();

				if ( 1 >= (len=mi.TypeName(&ti,A0)) ) {
					type_id = tid_VOID;
					len = ::lstrlen( UNKNOWN_RESULT_TYPE ) + 1;
					::memcpy( W0, UNKNOWN_RESULT_TYPE, len );
				}

				if ( HAS_RET_VAL(type_id,to_type) ) {
					::lstrcat( m, ", " );
					::lstrcat( m, W0 );
					::lstrcat( m, "* result" );
				}

				char* define = new char[2*widths[4]+100];
				CAPointer<char> def_body( new char[widths[4]] );

				::wsprintf( W2, "CALL_%s", W3 );

				def_body[0] = 0;
				::lstrcpy( define, "#define " );
				::lstrcat( define, W2 );
				::lstrcat( define, "( _this" );
				if ( HAS_RET_VAL(type_id,to_type) ) 
					::lstrcat( define, ", result" );

				int width = 0;
				int width_n = m_len;
				DWORD count = 2;
				CMethodParamInfo pi( mi );
				while( pi ) {
    
					*((WORD*)W2) = *((WORD*)W3) = MAKEWORD( ',', ' ' );

					if ( 1 >= (len=pi.Name(B3(2))) )
						len = ::wsprintf( W3+2, "param%d", count ) + 1;

					width += len - 1; // ' ' - term0
					width_n += len + 2 - 1; // ' ' - term0

					if ( ::lstrcmp(W3+2,"...") ) {

						if ( 1 >= (len=GetParamType(pi,B2(2))) )
							len = ::lstrlen( ::lstrcpy(W2+2,UNKNOWN_PARAM_TYPE) ) + 1;

						width += len + 3 - 1; // ',' + ' ' + ' ' - term0

						::lstrcat( m, W2 );
						::lstrcat( m, W3+1 );
						::lstrcat( define, W3 );
						::lstrcat( def_body, W3 );
					}
					else {
						::lstrcat( m, ", ..." );
						width += 5; // ' ' - term0
						break;
					}
					pi.GoNext();
					count++;
				}

				::lstrcat( m, " );" );

				for( i=0; i<((int)widths[2])-width; i++ )
					*(d+i) = ' ';
				*(d+i++) = ' ';
				*(d+i++) = '/';
				*(d+i++) = '/';
				*(d+i++) = ' ';
				*(d+i++) = '-';
				*(d+i++) = '-';
				*(d+i++) = ' ';
				*(d+i++) = 0;
				::lstrcat( m, d );

				::lstrcat( define, " ) " );
				int c = ((int)widths[3]) - width_n;
				if ( HAS_RET_VAL(type_id,to_type) ) 
					c -= 8;
				for( i=0; i<c; i++ )
					*(d+i) = ' ';
				*(d+i) = 0;

				::lstrcpy( W2, "((_this)->vtbl->" );
				::lstrcpy( W3, "( (_this)" );

				::lstrcat( define, d );
				::lstrcat( define, W2 );
				::lstrcat( define, W1 );
				::lstrcat( define, W3 );
				if ( HAS_RET_VAL(type_id,to_type) )
					::lstrcat( define, ", result" );
				::lstrcat( define, def_body );
				::lstrcat( define, " ))" );
				defines.Add( define );
				
				if ( (1<mi.ShortComment(A2)) || (1<mi.ExtComment(A2)) || (1<mi.BehaviorComment(A2)) || (1<mi.ValueComment(A2)) )
					::lstrcat( m, W2 );
				
				methods.Add( m );
			}
    }
  }
  else if( !extended ) {
    AVP_dword len;
    names.Add( (char*)::memcpy(new char[6],"dummy",6) );
    len = ::wsprintf( W2, "%s_dummy", m_iface.Name() );
    typedefs.Add( (char*)::memcpy(new char[len+1],W2,len+1) );
    m_iface.TypeName( 0, A3 );
    len = ::wsprintf( W1, "void (*fnp%s)( %s _this )", W2, W3 ) + 1;
    methods.Add( (char*)::memcpy(new char[len],W1,len) );
  }
	return S_OK;
}



// ---
HRESULT CCodeGen::FooterToHdr( COutputFile& ofile, const char* def_str ) {
	
  COutputSect& sect = *new COutputSect( ofile, h_endif, 0, false );
	
  ::memcpy( W2, "#endif // ", 10 );
	::lstrcpy( W2+10, def_str );
  PutString( sect, 0, W2 );
	
  return S_OK;
}




// ---
HRESULT CCodeGen::CollectMethods( CPArray<CIfaceMethod>& methods, bool extend ) {
  CPubMethodInfo mi( m_iface );
	CObjTypeInfo   oti( m_types );
	
	for( ; mi; mi.GoNext() ) {
		if ( mi.IsSeparator() || extend && !mi.IsExtended() )
			continue;

		tUINT count = 0;
		CIfaceMethod* next = new CIfaceMethod( mi, oti );
		CMethodParamInfo mpi( mi );

		while( mpi ) {
			tUINT nlen;
			tUINT tlen = GetParamType( mpi, A3 );
			if ( 1 >= (nlen=mpi.Name(A2)) )
				nlen = 1 + wsprintf( W2, "param%d", count );
			next->add( new CIfaceParam(W3,W2) );
			mpi.GoNext();
		}
		
		methods.Add( next );
	}

	return S_OK;
}




// ---
HRESULT CCodeGen::VtblToHdr( COutputFile& ofile, CPArray<char>& defines, bool internal ) {

  HRESULT hr = S_OK;

  CPArray<char> names;
  CPArray<char> typedefs;
  CPArray<char> methods;

	ParseIFaceMethods(names, typedefs, methods, defines, internal);

  uint i;
  uint c = methods.Count();
  uint a = 0;
  const char* tdef;
  COutputSect* sect;

  if ( !internal ) {
    sect = new COutputSect( ofile, private_defs_begin, 0, false );
    PutFmtString( *sect, 0, "#if !defined( %s_PRIVATE_DEFINITION )", m_iface.Name() );
  }

  sect = new COutputSect( ofile, if_meth_forwards, 0, false ); // IDS_FUNC_TYPEDEFS_STAMP
  if ( c ) {
    DWORD shift;
    if ( internal ) 
      shift = 0;
    else {
      shift = 2;
      PutString( *sect, 0, 0 );
      PutString( *sect, 0, 0 );
      //PutString( *sect, 0, "#if defined( __cplusplus )" );
      //PutString( *sect, 0, "extern \"C\" "BRACE_OS );
      //PutString( *sect, 0, "#endif" );
      //PutString( *sect, 0, 0 );
    }
    for( i=0; i<c; i++ ) {
      tdef = typedefs[i];
      uint cl = ::lstrlen( tdef );
      if ( cl && (*tdef != '/') ) {
        if ( a < cl )
          a = cl;
        PutFmtString( *sect, shift, (const char*)IDS_FUNC_TYPEDEF_FMT, methods[i] );
      }
      else
        PutString( *sect, shift, tdef );
    }
    if ( !internal ) {
      PutString( *sect, 0, 0 );
      //PutString( *sect, 0, "#if defined( __cplusplus )" );
      //PutString( *sect, 0, BRACE_CS );
      //PutString( *sect, 0, "#endif" );
      //PutString( *sect, 0, 0 );
      PutString( *sect, 0, 0 );
    }
  }

  sect = new COutputSect( ofile, if_decl, 0, false ); // IDS_IFACE_DECL_STAMP
  PutFmtString( *sect, 0, "struct i%sVtbl "BRACE_OS, m_iface.Name() );

  char* al = new char[a+1];
  ::memset( al, ' ', a );
  al[a] = 0;
  for( i=0; i<c; i++ ) {
    tdef = typedefs[i];
    uint cl = ::lstrlen( tdef );
    if ( cl && (*tdef != '/') ) {
      al[ a-cl ] = 0;
      PutFmtString( *sect, 2, "fnp%s %s %s;", tdef, al, names[i] );
      al[ a-cl ] = ' ';
    }
    else
      PutString( *sect, 0, tdef );
  }
  delete [] al;

  PutFmtString( *sect, 0, BRACE_CS"; // \"%s\" external virtual table prototype", m_iface.Name() );

  if ( !internal ) {
    sect = new COutputSect( ofile, private_defs_end, 0, false );
    PutFmtString( *sect, 0, "#endif // if !defined( %s_PRIVATE_DEFINITION )", m_iface.Name() );
  }
  return S_OK;
}



// ---
HRESULT CCodeGen::CalcMethodsWidths( const CMethodInfo& mi, DWORD widths[5], bool internal ) { // , CPArray<char>& forwards ) {

  AVP_dword len = 0;
  AVP_dword m_len = 0;

  if ( mi.IsSeparator() )
    return S_OK;

  if ( widths[0] < 6 ) // lstrlen("tERROR")
    widths[0] = 6;


  if ( 1 >= (m_len=mi.Name(0,0)) )
    m_len = ::lstrlen( UNKNOWN_METHOD_NAME ) + 1;

  m_len += 5 + ::lstrlen( m_iface.Name() ); // fnpIFACENAME_%s

  if ( widths[1] < m_len )
    widths[1] = m_len;

  if ( internal )
    len = ::lstrlen( m_int_type_name ) + 1;
  else {
    CObjTypeInfo oti( m_types );
    len = mi.TypeName( &oti, 0, 0 );
  }

  if ( 1 >= len ) 
    len = ::lstrlen( UNKNOWN_RESULT_TYPE ) + 1;

  DWORD width = len;
  DWORD width_n = m_len;
  DWORD count = 0;

  CMethodParamInfo mpi( mi );
  while( mpi ) {
    len = GetParamType( mpi, 0, 0 ); //, &forwards );
    width += len + 3 - 1; // ',' + ' ' + ' ' - term0
    
    if ( 1 >= (len=mpi.Name(0,0)) )
      len = 6 + (count>9); // "param%d"
    
    width += len - 1; // - term0
    width_n += len + 2 - 1; // ',' + ' ' - term0
    
    mpi.GoNext();
    count++;
  }
  
  if ( widths[2] < width )
    widths[2] = width;
  
  if ( widths[3] < width_n )
    widths[3] = width_n;
  
  len = mi.ShortComment(0,0);
  if ( 1 >= len ) {
    len = mi.ExtComment(0,0);
    if ( 1 >= len ) {
      len = mi.BehaviorComment(0,0);
      if ( 1 >= len )
        len = mi.ValueComment(0,0);
    }
  }
  len += widths[0] + widths[1] + widths[2];
  if ( widths[4] < len )
    widths[4] = len;
  
  return S_OK;
}


// ---
HRESULT CCodeGen::MsgClassInfoToHdr( COutputSect& sect, CMsgClassInfo& mci, bool for_prototype, bool is_comment ){
//  if ( for_prototype && !mci.IsNative() )
//		return S_OK;

	if( !is_comment && !for_prototype && mci.IsNative() )
		is_comment = true;

	PutString( sect, 0, 0 );
	PutString( sect, 0, "// message class" );
	PutExtendedComment( sect, 0, 0, mci, VE_PID_LARGECOMMENT );
	mci.ShortComment( A3 );
	//DWORD id = mci.Id();
	DWORD id = iCountCRC32str( mci.Name() );

	PutFmtString( sect, 0, "%s#define %s 0x%08x // %s", is_comment ? "//! " : "", (const char*)mci.Name(), id, W3 );

	for( CMsgInfo mc( mci ); mc; mc.GoNext() ) {
		if( for_prototype == !mc.IsNative() )
			continue;
		id = mc.Id();
		mc.ShortComment( A3 );
		PutString( sect, 0, 0 );
		PutFmtString( sect, 2, "#define %s 0x%08x // (%u) -- %s", (const char*)mc.Name(), id, id, W3 );
		PutExtendedComment( sect, 2, "// context comment", mc, I(M_MESSAGECONTEXT) );
		PutExtendedComment( sect, 2, "// data content comment", mc, I(M_MESSAGEDATA) );
	}
	return S_OK;
}


// ---
HRESULT CCodeGen::MsgClassesToHdr( COutputFile& file, bool for_prototype ) {
	COutputSect&  sect = *new COutputSect( file, if_msg_cls, 0, false );

  // not native classes are printed if it has at least one not commented out message
  for( CMsgClassInfo mci( m_iface ); mci; mci.GoNext() )
		MsgClassInfoToHdr(sect, mci, for_prototype, false);

	return S_OK;
}




// not native classes and messages are commented out
/*
// ---
inline bool comm_str( const char*& comm, bool for_prototype, bool native ) {
  if ( for_prototype == native ) 
    comm = "";
  else
    comm = "// ";
  return !(for_prototype && !native);
}



    for( ; mci; mci.GoNext() ) {
      const char* comment;
      if ( !comm_str(comment,for_prototype,mci.IsNative()) )
        continue;
    
      PutString( sect, 0, 0 );
      PutString( sect, 0, "// message class" );
      PutExtendedComment( sect, 0, 0, mci, VE_PID_LARGECOMMENT );
      mci.ShortComment( A3 );
      id = mci.Id();
      PutFmtString( sect, 0, "%s#define %s 0x%08x // (%u) -- %s", comment, (const char*)mci.Name(), id, id, W3 );
    
      CMsgInfo mc( mci );
      for( ; mc; mc.GoNext() ) {
        if ( !comm_str(comment,for_prototype,mc.IsNative()) )
          continue;
        id = mc.Id();
        mc.ShortComment( A3 );
        PutString( sect, 0, 0 );
        PutFmtString( sect, 2, "%s#define %s 0x%08x // (%u) -- %s", comment, (const char*)mc.Name(), id, id, W3 );
        PutExtendedComment( sect, 2, "// context comment", mc, I(M_MESSAGECONTEXT) );
        PutExtendedComment( sect, 2, "// data content comment", mc, I(M_MESSAGEDATA) );
      }
    }
*/