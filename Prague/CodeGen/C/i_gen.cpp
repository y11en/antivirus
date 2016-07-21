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


#define PAGE_WITDH (60)

// ---
HRESULT CCodeGen::HeaderToInclude( COutputFile& ofile, const char* file_name ) {

  GUID guid;
  ::CoCreateGuid( &guid );
  ::StringFromGUID2( guid, (LPOLESTR)A2 );
  size_t l = ::wcstombs( W3, (LPOLESTR)A2 );
  ::memmove( W3, W3+1, l-2 );
  W3[l-2] = 0;
  ::_strlwr( W3 );
	char* ptr;
  for( ptr=W3; *ptr; ptr++ ) {
    if ( *ptr == '-' )
      *ptr = '_';
  }
  
  ExtractFileName( file_name, W2 );
  for( ptr=W2; *ptr; ptr++ ) {
    if ( *ptr == '.' )
      *ptr = '_';
  }
  ::_strlwr( W2 );

  ::wsprintf( W1, "__%s__%s", W2, W3 );
  COutputSect& sect = *new COutputSect( ofile, h_if_def, 0, false );
  PutFmtString( sect, 0, "#if !defined( %s )", W1 );
  PutFmtString( sect, 0, "#define %s", W1 );

  return S_OK;
}



// ---
HRESULT CCodeGen::InterfaceHdrsToInclude( COutputFile& ofile ) {
  tDWORD len;
	COutputSect& sect = *new COutputSect( ofile, h_includes, 0, false );
  PutFmtString( sect, 0, "#include <%si_root.h>", m_inc_prefix );
  if ( 1 < m_iface.HeaderName(A3) ) {
    ::_strlwr( W3 );
    if ( !::strstr(W3,".h") )
      ::lstrcat( W3, ".h" );
    PutFmtString( sect, 0, "#include <%s%s>", m_inc_prefix, W3 );
  }
	else
		PutString( sect, 0, "#error Cannot get interface name for header including" );
	
	len = m_plugin.Name( A3 );
	if ( 1 < len ) {
		::_strlwr( W3 );
		if( m_plugin_public )
			PutFmtString( sect, 0, "#include <plugin/p_%s.h>", W3 );
		else
			PutFmtString( sect, 0, "#include \"plugin_%s.h\"", W3 );
	}
	else
		PutString( sect, 0, "#error Cannot get plugin name for header including" );
  return S_OK;
}


// ---
HRESULT CCodeGen::InterfaceToInclude( COutputFile& ofile, const char* src_file_name ) {

	bool root = !::lstrcmp("Root",m_iface.Name());
  PutIFaceComment( ofile );

   COutputSect* sect = new COutputSect( ofile, h_regcall, 0, false ); // IDS_INCLUDE_RGISTER_CALL_STAMP
	PutFmtString( *sect, 2, "tERROR pr_call %s_Register( hROOT root );", m_iface.Name() );

	if ( !root ) {
		COutputSect& sect = *new COutputSect( ofile, data_str, 0, false ); // IDS_DATA_STRUCT_STAMP
		if ( !m_opt.private_structure() ) {
			DataStructsToSource( sect );
			PutString( sect, 0, 0 );
			PutFmtString( sect, 0, "// to get pointer to the structure %s from the hOBJECT obj", m_data_name );
			PutFmtString( sect, 0, "#define CUST_TO_%s(object)  ((%s*)( (((tPTR*)object)[2]) ))", m_data_name, m_data_name );
		}
		else
			PutFmtString( sect, 0, "  // data structure %s is declared in %s source file", m_data_name, src_file_name );
		new COutputSect( ofile, obj_decl, 0, false ); // empty object description
	}

  sect = new COutputSect( ofile, pr_table, 0, false ); // IDS_INCLUDE_PROPERTY_TABLE_STAMP
  CPropInfo pi( m_iface ); // .FirstProperty()
  DWORD ws[2] = { 0 };
  while( pi ) {
    CalcPropWidths( pi, false, ws );
    pi.GoNext();
  }
  pi.GoFirst();
  while( pi ) {
    PropToInclude( *sect, pi, false, ws );
    pi.GoNext();
  }
	return S_OK;
}



// ---
HRESULT CCodeGen::FooterToInclude( COutputFile& ofile, const char* file_name ) {
  
  ExtractFileName( file_name, W2 );

  for( char*ptr=W2; *ptr; ptr++ ) {
    if ( *ptr == '.' )
      *ptr = '_';
    else if ( *ptr == '\\' )
      *ptr = '_';
    else if ( *ptr == '/' )
      *ptr = '_';
  }
  ::_strlwr( W2 );

  COutputSect& sect = *new COutputSect( ofile, h_endif, 0, false );
  PutFmtString( sect, 0, "#endif // %s", W2 );

  return S_OK;
}



