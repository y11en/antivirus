#include "stdafx.h"
#include "CodeGen.h"
#include "Options.h"
#include "..\PCGError.h"




// ---
HRESULT CCodeGen::GenProtoFile( COutputFile& of ) {
	HRESULT hr;
	CAPointer<char> def_str;
	
	if ( FAILED(hr=CreateIFaceDefString(def_str)) )
		return hr;
	if ( FAILED(hr=HeaderToHdr(of,m_hdr_name,def_str,true)) )
		return hr;
	if ( FAILED(hr=InterfaceToHdr(of)) )
		return hr;
	if ( FAILED(hr=FooterToHdr(of,def_str)) )
		return hr;
	return S_OK;
}




// ---
HRESULT CCodeGen::GenIncludeFile( COutputFile& of ) {
	HRESULT hr;
	
  if ( FAILED(hr=PutHeader(of,m_hdr_name)) )
		return hr;
	if ( FAILED(hr=HeaderToInclude(of,m_hdr_name)) )
		return hr;
  
	// just to say it has to be moved
	new COutputSect( of, pl_id, 0, false );
  new COutputSect( of, ven_id, 0, false );
	
	if ( FAILED(hr=InterfaceHdrsToInclude(of)) )
		return hr;
	if ( FAILED(hr=IncludesToHdr(of)) )
		return hr;
	if ( FAILED(hr=ConstsToHdr(of,false)) )
		return hr;
	if ( FAILED(hr=MsgClassesToHdr(of,false)) )
		return hr;
	if ( FAILED(hr=InterfaceToInclude(of,m_src_name)) )
		return hr;
	if ( FAILED(hr=FooterToInclude(of,m_hdr_name)) )
		return hr;
	return hr;
}



// ---
HRESULT CCodeGen::GenSourceFile( COutputFile& of ) {
	HRESULT hr;
  
	if ( FAILED(hr=PutHeader(of,m_src_name)) )
		return hr;
	if ( FAILED(hr=HeaderToSource(of,m_src_name)) )
		return hr;
	if ( FAILED(hr=InterfaceToSource(of,m_hdr_name)) )
		return hr;
	return hr;
}


HRESULT CCodeGen::GenPluginIncludeDefs( COutputFile& of ) {
  AVP_dword id;

  // plugin's identifier
  COutputSect* sect = new COutputSect( of, pl_id, 0, false ); // IDS_PLUGIN_ID_STAMP
  if ( (1 >= m_plugin.MnemonicID(A2)) || (W2[0] == 0) || (0 == (id=m_plugin.PluginID())) )
		PutString( *sect, 0, "#error cannot get plugin ID" );
  else if ( FAILED(PutFmtString(*sect,0,"#define %s  ((tPID)(%u))",W2,id)) )
    PutString( *sect, 0, "#error cannot put plugin ID" );
	
  // vendor's identifier
  sect = new COutputSect( of, ven_id, 0, false ); // IDS_VENDOR_ID_STAMP
  if ( (1 >= m_plugin.VendorMnemonicID(A2)) || (W2[0] == 0) || (0 == (id=m_plugin.VendorID())) )
		PutString( *sect, 0, "#error cannot get vendor ID" );
  else if ( FAILED(PutFmtString(*sect,0,"#define %s  ((tVID)(%u))",W2,id)) )
    PutString( *sect, 0, "#error cannot put vendor ID" );
	return S_OK;
}

// ---
HRESULT CCodeGen::GenPluginIncludeFile( COutputFile& of, bool v2_public ) {
	HRESULT   hr;

	// make plugin header
	if ( FAILED(hr=PutHeader(of,m_hdr_name)) )
		return hr;
	
	CAPointer<char> def_str;
	::lstrcpy( W2, "__" );
	if ( v2_public )
		::lstrcat( W2, "public_" );
	::lstrcat( W2, "plugin_" );
	::lstrcat( W2, m_plugin.Name() );
	::_strlwr( W2 );
	int len = ::lstrlen( W2 ) + 1;

	def_str = (char*)::memcpy( new char[len], W2,len );
	HeaderToHdr( of, m_hdr_name, def_str, false );

	// #include <prague.h>
	COutputSect* sect = new COutputSect( of, h_includes, 0, false );
	PutString( *sect, 0, "#include <prague.h>" );

	GenPluginIncludeDefs(of);

	FooterToHdr( of, def_str );
		
	return S_OK;
}


// ---
HRESULT CCodeGen::GenPluginSourceInclude( COutputSect& sect )
{
	tDWORD          len;

	int enum_flags = 0;
	while( EnumIFaces(enum_flags) ) {
		if ( 1 >= (len=m_iface.Name(A3)) ) 
			continue;
		m_iface_names.Add( (char*)::memcpy(new char[len],W3,len) );
    if ( 1 >= (len=m_iface.SourceFileName(A3)) ) 
      PutString( sect, 0, "#error Cannot get interface name for header including!" );
    else {
      ::_strlwr( W3 );
			::memcpy( W2, "#include \"", 10 );
			::memcpy( W2+10, W3, len-1 );
			::memcpy( W2+10-1+len, ".h\"", 4 );
			PutString( sect, 0, W2 );
    }
	}
	return S_OK;
}

// ---
HRESULT CCodeGen::GenPluginSourceImpl( COutputFile& of )
{
	tDWORD          len;
	CAPointer<char> pid;
	uint            i, c;

	COutputSect& sect = *new COutputSect( of, pl_def, 0, false );

	if ( 1 < (len=m_plugin.MnemonicID(A3)) )
		pid = (char*)::memcpy( new char[len], W3, len );
	else
		pid = (char*)::memcpy( new char[8], "PID_...", 8 );

	c = m_iface_names.Count();

	// PutString( sect, 0, "#include <stdarg.h>" );
	PutString( sect, 0, "PR_MAKE_TRACE_FUNC;" );
	PutString( sect, 0, 0 );
	PutString( sect, 0, "hROOT  g_root = NULL;" );
	PutString( sect, 0, 0 );
	PutString( sect, 0, "tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pError ) "BRACE_OS );
		PutString( sect, 2, "// tDWORD count;" );
		PutString( sect, 2, 0 );
		PutString( sect, 2, "switch( dwReason ) "BRACE_OS );
			PutString( sect, 4, "case DLL_PROCESS_ATTACH:" );
			PutString( sect, 4, "case DLL_PROCESS_DETACH:" );
			PutString( sect, 4, "case DLL_THREAD_ATTACH :" );
			PutString( sect, 4, "case DLL_THREAD_DETACH :" );
				PutString( sect, 6, "break;" );
				PutString( sect, 0, 0 );
			PutString( sect, 4, "case PRAGUE_PLUGIN_LOAD :" );
				PutString( sect, 6, "g_root = (hROOT)hInstance;" );
				PutString( sect, 6, "*pError = errOK;" );
				PutString( sect, 6, "//resolve  my imports" );
				PutFmtString( sect, 6, "//if ( PR_FAIL(*pError=CALL_Root_ResolveImportTable(g_root,&count,import_table_variable...,%s)) ) "BRACE_OS, (const char*)pid );
					PutString(  sect, 6, "//   PR_TRACE(( g_root, prtERROR, \"cannot resolve import table for ...\" ));" );
					PutString(  sect, 6, "//   return cFALSE;" );
					PutString(  sect, 6, "//"BRACE_CS );
				PutString( sect, 0, 0 );
				PutString( sect, 6, "//register my exports" );
				PutFmtString( sect, 6, "//CALL_Root_RegisterExportTable( g_root, &count, export_table_..., %s );", (const char*)pid );
				PutString( sect, 0, 0 );
				PutString( sect, 6, "//register my custom property ids" );
				PutString( sect, 6, "//if ( PR_FAIL(*pError=CALL_Root_RegisterCustomPropId(g_root,&some_propid_variable,\"some_property_name\",pTYPE_DWORD)) ) "BRACE_OS );
				PutString( sect, 6, "//  PR_TRACE(( g_root, prtERROR, \"cannot register custom property ...\" ));" );
				PutString( sect, 6, "//  return cFALSE;" );
				PutString( sect, 6, "//"BRACE_CS );
				PutString( sect, 0, 0 );
				PutString( sect, 6, "// register my interfaces" );
				if ( !c ) {
					PutString( sect, 6, "//if ( PR_FAIL(*pError=SomeInterface_Register(g_root)) )"BRACE_OS );
					PutString( sect, 6, "//  PR_TRACE(( g_root, prtERROR, \"cannot register \\\"SomeInterface\\\" interface\"));" );
					PutString( sect, 6, "//  return cFALSE;" );
					PutString( sect, 6, "//"BRACE_CS );
				}
				else for( i=0; i<c; i++ ) {
					GenPluginRegCall( sect, m_iface_names[i] );
					if ( i != (c-1) )
						PutString( sect, 0, 0 );
				}
				PutString( sect, 6, "break;" );
			PutString( sect, 0, 0 );
			PutString( sect, 4, "case PRAGUE_PLUGIN_UNLOAD :" );
				PutString( sect, 6, "// free system resources" );
				PutString( sect, 6, "// unregister my custom property ids -- you can drop it, kernel do it by itself" );
				PutString( sect, 6, "// release    my imports		         -- you can drop it, kernel do it by itself" );
				PutString( sect, 6, "// unregister my exports		         -- you can drop it, kernel do it by itself" );
				PutString( sect, 6, "// unregister my interfaces          -- you can drop it, kernel do it by itself" );
				PutString( sect, 6, "g_root = NULL;" );
				PutString( sect, 6, "break;" );
			PutString( sect, 2, BRACE_CS );
		PutString( sect, 2, "return cTRUE;" );
	PutString( sect, 0, BRACE_CS );

	return errOK;
}

// ---
int CCodeGen::GenPluginRegCall( COutputSect& sect, const char* iface_name, int where ) {
	
	int c = sect.Count();
	if ( where > c )
		where = c;
	
	::wsprintf( W2, "if ( PR_FAIL(*pError=%s_Register(g_root)) ) "BRACE_OS, iface_name );
	::wsprintf( W3, "PR_TRACE(( g_root, prtERROR, \"cannot register \\\"%s\\\" interface\"));", iface_name );
	
	PutString( sect, 6, W2, where++ );
	PutString( sect, 8, W3, where++ );
	PutString( sect, 8, "return cFALSE;", where++ );
	PutString( sect, 6, BRACE_CS, where++ );
	return where;
}

// ---
HRESULT CCodeGen::GenPluginSourceFile( COutputFile& of ) {
	HRESULT         hr;
	COutputSect*    sect = 0;
		
	if ( FAILED(hr=PutHeader(of,m_hdr_name)) )
		return hr;

	sect = new COutputSect( of, h_includes, 0, false );
	PutString( *sect, 0, "#include <prague.h>" );

	if ( FAILED(hr=GenPluginSourceInclude(*sect)) )
		return hr;

	return GenPluginSourceImpl(of);
}




// ---
HRESULT CCodeGen::GenProtoFiles() {
	HRESULT hr = S_OK;

	int enum_flags = ENUM_IF_SELECTED | ENUM_IF_INIT_INFO;
	while( SUCCEEDED(hr) && EnumIFaces(enum_flags) )
			hr = GenFile( Prototype, true );
	
	return hr;
}



// ---
HRESULT CCodeGen::GenPluginFiles() {
	COutputFile of(this);
	
	if ( m_kernel )
		return S_OK;
	
	if ( !m_start_project )
		return S_OK;
	
	m_start_project = false;

	if ( !m_plugin.Selected() /*&& !m_plugin_public*/ )
		return S_OK;
	
	if ( !::_stricmp(m_plugin.Name(),"Plugin") || (1 >= m_plugin.MnemonicID(A3)) || !::lstrcmp(W3,"PID_PLUGIN") )
		return CG_E_PLUGIN_UNDEFINED;
	
	m_plugin_mode = true;
	m_iface_names.RemoveAll();

	HRESULT hr=GenFile( Plugin, true );
	if ( SUCCEEDED(hr) /*&& m_plugin.Selected()*/ )
		hr = GenFile( Plugin, false );
	
	m_plugin_mode = false;
	m_iface_names.RemoveAll();
	
	return hr;
}



// ---
HRESULT CCodeGen::GenSourceFiles() {
	HRESULT hr = S_OK;

	int enum_flags = ENUM_IF_SELECTED | ENUM_IF_INIT_INFO;
	while( SUCCEEDED(hr) && EnumIFaces(enum_flags) ) {
		hr = GenFile( Source, true );
		if ( SUCCEEDED(hr) )
			hr = GenFile( Source, false );
	}

	return hr;
}

// ---
bool CCodeGen::IsGenType( GenType type )
{
	switch(type)
	{
		case Prototype : return m_prototype;
		case Plugin : return true;
		case Source : return m_prototype ? m_opt.header() : m_opt.source(); break;
	}
	return false;
}

// ---
HRESULT CCodeGen::GenFileName( GenType type, bool prototype, char* name, int size )
{
	int len = 0;
	switch(type)
	{
		case Plugin : len = sprintf( W3, m_plugin_public && prototype ? "p_%s" : "plugin_%s", m_plugin.Name() ); break;
		case Prototype : len = m_iface.HeaderName(A3); break;
		case Source : len = m_iface.SourceFileName(A3); break;
	}
	if( len < 1 )
		return MAKE_HRESULT(1,20,ERROR_BAD_FORMAT);

	if( !*W3 )
		return CG_FILE_NAME;

	::_strlwr( W3 );

	char *path = prototype && ( m_plugin_public && (type == Plugin) || (type == Prototype)) ? m_public_folder : m_output_folder;
	sprintf(name, "%s%s.%s", path, W3, prototype ? "h" : m_source_ext);
	return S_OK;
}
	


// ---
HRESULT CCodeGen::GenFile( GenType type, bool prototype ){
	HRESULT hr;

	m_prototype = prototype;

	if( !IsGenType(type) )
		return S_OK;

	char src_name[MAX_PATH], hdr_name[MAX_PATH];
	if( FAILED(hr = GenFileName(type, true, hdr_name, MAX_PATH)) )
		return hr;
	if( FAILED(hr = GenFileName(type, false, src_name, MAX_PATH)) )
		return hr;

	const char *gen_name = prototype ? hdr_name : src_name;

	if ( !CheckFile(gen_name) ) 
		return CG_E_RDONLY;

	char new_name[MAX_PATH], old_name[MAX_PATH];
	sprintf(new_name, "%s.new", gen_name);
	sprintf(old_name, "%s.bak", gen_name);

	COutputFile of(this);
	COutputFile oldf(this);
	COutputFile* old = 0;
	
	if ( FAILED(hr=LoadFile(gen_name,&oldf)) )
		return hr;
	else
		old = &oldf;
	
// check version
	if( !m_change_version && old )
		if( !CheckVersion(*old, gen_name) )
			return E_FAIL;

	m_src_name = src_name;
	m_hdr_name = src_name;

	switch(type)
	{
		case Prototype :
			hr = GenProtoFile( of );
			break;
		case Plugin :
			if( prototype )
				hr = GenPluginIncludeFile( of );
			else
				hr = GenPluginSourceFile( of );
			break;
		case Source :
			if( prototype )
				hr = GenIncludeFile( of );
			else
				hr = GenSourceFile( of );
			break;
	}
	
	if ( FAILED(hr) )
		;
	else if ( old && old->Count() ) {
		old->merge( of );
		hr = old->flash( new_name, m_opt );
	}
	else
		hr = of.flash( new_name, m_opt );
	
	if ( SUCCEEDED(hr) )
		hr = SwitchFile( new_name, gen_name, old_name );
	::DeleteFile( new_name );
	
	return hr;
}





// ---
HRESULT CCodeGen::LoadFile( const char* file_name, COutputFile* old ) {
	if ( old ) {
		HANDLE h = ::CreateFile( file_name, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
		if ( h && (h != INVALID_HANDLE_VALUE) ) {
			old->load( h );
			::CloseHandle( h );
			return S_OK;
		}
		else {
			DWORD err = ::GetLastError();
			if ( (err == ERROR_FILE_NOT_FOUND) || (err==ERROR_PATH_NOT_FOUND) )
				return S_OK;
			else
				return CG_E_BACKUP_ERROR;
		}
	}
	else
		return S_OK;
}



bool CCodeGen::EnumIFaces( int& flags ) {
	if( !(flags & ENUM_IF_NEXT) )	{
		flags |= ENUM_IF_NEXT;
		m_iface.GoFirst();
	}
	else
		m_iface.GoNext();

	for( ; m_iface; m_iface.GoNext() ) {
		if( m_iface.Linked() )
			continue;

		if( (flags & ENUM_IF_SELECTED) && !m_iface.Selected() )
			continue;

		const char* iname = m_iface.Name();
		if( !iname || !*iname ) {
			if( (flags & ENUM_IF_INIT_EXPORT) && !m_is_ex_methods && !m_ex_methods) {
				m_ex_methods = new CPubMethodInfo(m_iface);
				m_ex_methods->GoFirst();
				if(*m_ex_methods)
					m_is_ex_methods = true;
			}

			continue;
		}

		if( flags & ENUM_IF_INIT_INFO )
			InitIFaceInfo();

		return true;
	}
	return false;
}


// ---
bool CCodeGen::InitIFaceInfo() {
	
	m_kernel = false;

	m_prop_tbl_count = 0;
	m_prop_errors.RemoveAll();
	m_delayed_props.RemoveAll();
	m_methods.RemoveAll();

	m_int_type_name = 0;
	m_data_name = 0;
	m_data_single = true;

	if ( !m_iface )
		return false;

	const static char* kernel_names[] = { "Root", "Tracer", "IFaceEnum", "ImpexManager", "MsgReceiver" };

	const char* name = m_iface.Name();
	for( int i=0; !m_kernel && i<countof(kernel_names); i++ )
		m_kernel = !::lstrcmpi( name, kernel_names[i] );

	*(DWORD*)W3 = MAKELONG( MAKEWORD('h','i'), MAKEWORD('_',0) );
	DWORD len = m_iface.Name( B3(3) ) + 3;
	m_int_type_name = (char*)::memcpy( new char[len], W3, len );
	
	m_version = 0;

	CDataStructInfo ds( m_iface );
	if( ds )
	{
		m_data_single = (ds.Next() == 0);
		InitDataStructName(ds);
	}
	else
	{
		m_data_single = true;
		m_data_name = 0;
		m_data_name_len = 0;
	}
	return true;
}

// ---
void	CCodeGen::InitDataStructName(CDataStructInfo &ds)
{
	DWORD len;
	if ( m_data_single && (1<(len=ds.Name(A3))) ) 
		m_data_name = (char*)::memcpy( new char[len], W3, len );
	else {
		len = m_iface.Name( A3 );
		::memcpy( W3+len-1, "Data", 5 );
		len += 5;
		m_data_name = (char*)::memcpy( new char[len], W3, len );
	}
	m_data_name_len = len;
}


