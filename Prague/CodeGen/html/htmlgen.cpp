// CodeGen.cpp : Implementation of CCodeGen
#include "stdafx.h"
#include "htmlgen.h"
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include "options.h"
#include "..\AVPPveId.h"
#include "..\PCGError.h"
#include <_AVPIO.h>

/////////////////////////////////////////////////////////////////////////////
// CCodeGen

//#define CHECK_OPT

// ---
CCodeGen::CCodeGen() 
	: m_types( 0 ),
		m_plugin( 0, false ),
    m_tree( 0, false ),
    m_iface( 0, false ),
		m_full( true ),
		m_brick( false ),
		m_lang( l_any ),
		m_plugin_done( false )
{
	#if defined(_DEBUG) && defined(CHECK_OPT)
		AvpCloseHandle=CloseHandle;
		AvpWriteFile=WriteFile;
		AvpCreateFile=CreateFile;
		AvpGetFileSize=GetFileSize;
		AvpReadFile=ReadFile;
	#endif
}



// ---
CCodeGen::~CCodeGen() {
}



// ---
STDMETHODIMP CCodeGen::GetImplementationOptionTemplate( DWORD* len, BYTE** options ) {
	MAKE_ADDR1( addr, OPTIONS_PID );
  return ExtractTemplate( IDR_IFACE_OPTIONS, addr, len, options );
}



// ---
STDMETHODIMP CCodeGen::ImplementationGenerate( DWORD clen, BYTE* cont, DWORD olen, BYTE* opt ) {
  
  HRESULT hr;
  
  if ( FAILED(hr=LoadContents(clen,cont)) )
    ;
  else if ( FAILED(hr=LoadOptions(olen,opt)) )
    ;
  else {
    CAPointer<char> src;
    CAPointer<char> src_new;
    CAPointer<char> src_old;

    ::lstrcpy( g_buff, get_pl_name(m_plugin) );
    ::lstrcat( g_buff, "_plugin" );
    
    make_file_names( g_buff, "html", src, src_new, src_old );
    hr = GenFile( MakePluginHTML, false, src, src_new, src_old );

    m_iface.GoFirst();
    while( SUCCEEDED(hr) && m_iface ) {
      if ( !m_iface.Selected() )
        ;
      else if ( 1 >= m_iface.SourceFileName(g_buff,g_buffs) )
        hr = MAKE_HRESULT(1,20,ERROR_BAD_FORMAT);
      else {
        make_file_names( g_buff, "html", src, src_new, src_old );
        hr = GenFile( MakeIfaceHTML, false, src, src_new, src_old );
      }
      m_iface.GoNext();
    }
  }
  m_iface = 0;
  m_plugin = 0;
  m_tree = 0;
  return hr;
}




// ---
STDMETHODIMP CCodeGen::GetPrototypeOptionTemplate( DWORD *len, BYTE **options ) {
  return GetImplementationOptionTemplate( len, options );
}



// ---
STDMETHODIMP CCodeGen::PrototypeGenerate( DWORD clen, BYTE *cont, DWORD olen, BYTE *opt ) {
  
  HRESULT hr;
  
  if ( FAILED(hr=LoadContents(clen,cont)) )
    ;
  else if ( FAILED(hr=LoadOptions(olen,opt)) )
    ;
  else {
    hr = S_OK;
    m_iface.GoFirst();
    while( SUCCEEDED(hr) && m_iface ) {
      if ( !m_iface.Selected() )
        ;
      else if ( 1 >= m_iface.HeaderName(g_buff,g_buffs) )
        hr = MAKE_HRESULT(1,20,ERROR_BAD_FORMAT);
      else {
        CAPointer<char> hdr;
        CAPointer<char> hdr_new;
        CAPointer<char> hdr_old;
        
        make_file_names( g_buff, "html", hdr, hdr_new, hdr_old );
        hr = GenFile( MakeIfaceHTML, true, hdr, hdr_new, hdr_old );
      }
      m_iface.GoNext();
    }
  }
	m_iface = 0;
  m_plugin = 0;
  m_tree = 0;
  return S_OK;
}




// ---
STDMETHODIMP CCodeGen::GetProjectOptions( DWORD *len, BYTE** buffer ) {
	
	if ( !len || !buffer )
		return E_INVALIDARG;
	else {
		*len = 0;
		*buffer = 0;
	}
	
	return S_OK;
}



// ---
STDMETHODIMP CCodeGen::GetErrorString( HRESULT err, BYTE** err_str ) {
	
	static char msg[] = "Some error occurred, but I don't know why, sorry :-)";
	
	if ( !err_str )
		return E_INVALIDARG;
	*err_str = (BYTE*)::CoTaskMemAlloc( sizeof(msg) );
	if ( !*err_str ) 
		return E_OUTOFMEMORY;
	::lstrcpy( (char*)*err_str, msg );
	return S_OK;
}



// ---
STDMETHODIMP CCodeGen::EditNode( HWND dad, DWORD *len, BYTE **node_stream ) {
	if ( !len || !*len || !node_stream )
		return E_INVALIDARG;
	return S_OK;
}



// ---
STDMETHODIMP CCodeGen::SetObjectTypes( DWORD len, BYTE* objtypes ) {
	m_types = 0;
	if ( len && objtypes && !m_types.Init(len,objtypes) )
		return CG_E_SERIALIZE_PARAMETER;
	return S_OK;
}



// ---
STDMETHODIMP CCodeGen::GetInfo( DWORD* len, BYTE** info ) {
	return ExtractTemplate( IDR_IFACE_DESCRIPTION, 0, len, info );
}



// ---
STDMETHODIMP CCodeGen::GetDescription( BYTE** description ) {
	return ExtractString( IDS_IFACE_DESCRIPTION, description );
}



// ---
STDMETHODIMP CCodeGen::GetVendor( BYTE** vendor ) {
	return ExtractString( IDS_IFACE_VENDOR, vendor );
}



// ---
STDMETHODIMP CCodeGen::GetVersion( BYTE** version ) {
	return ExtractString( IDS_IFACE_VERSION, version );
}



// ---
STDMETHODIMP CCodeGen::StartProject( DWORD len, BYTE* buffer ) {
	m_plugin_done = false;
	CTreeInfo seance( len, buffer );
	if ( seance && (1 < (len=seance.StrAttr(VE_PID_CGPI_PRIVATEDIR,g_buff,g_buffs))) ) {
		if ( g_buff[len-2] != '\\' ) {
			g_buff[len-1] = '\\';
			g_buff[len++] = 0;
		}
		m_output_folder = (char*)::memcpy( new char[len], g_buff, len );
	}
	return S_OK;
}



// ---
STDMETHODIMP CCodeGen::EndProject() {
	m_plugin_done = false;
	m_output_folder = 0;
	return S_OK;
}



// ---
HRESULT CCodeGen::ExtractString( UINT id, BYTE** output ) {
	int len;
	static char buff[1000];
	
	if ( !output ) 
		return E_INVALIDARG;
	
	len = ::LoadString( _Module.GetModuleInstance(), id, buff, sizeof(buff) ) + 1;
	if ( !len )
		return HRESULT_FROM_WIN32( ::GetLastError() );

	*output = (BYTE*)::CoTaskMemAlloc( len );
	if ( !*output )
		return E_OUTOFMEMORY;

	::memcpy( *output, buff, len );
	return S_OK;
}



// ---
HRESULT CCodeGen::ExtractTemplate( UINT id, DWORD* addr, DWORD* len, BYTE** output ) {
	HRESULT   hr;
	HINSTANCE hInstance;
	HRSRC     found;
	AVP_dword size;
	HGLOBAL   resource;
	void*     buffer;

	if ( !len || !output )
		return E_INVALIDARG;
	else {
		*len = 0;
		*output = 0;
	}
	
	hInstance = _Module.GetModuleInstance();
	found = ::FindResource( hInstance, MAKEINTRESOURCE(id), RT_RCDATA );
	if ( !found ) 
		return S_FALSE;
	
	size = ::SizeofResource( hInstance, found );
	if ( !size )
		return S_FALSE;
	
	resource = ::LoadResource( hInstance, found );
	if ( !resource ) 
		return HRESULT_FROM_WIN32( GetLastError() );
	
	buffer = ::LockResource( resource );
	if ( !buffer )
		hr = HRESULT_FROM_WIN32( GetLastError() );
	else if ( addr ) {
		HDATA node = 0;
		HDATA root = 0;
		if ( !::KLDT_DeserializeFromMemoryUsingSWM((char*)buffer,size,&root) && root ) 
			hr = E_UNEXPECTED;
		else if ( 0 == (node=::DATA_Find(root,addr)) )
			hr = E_UNEXPECTED;
		else {
			tDWORD size = ::KLDT_SerializeToMemoryUsingSWM( 0, 0, node, "" );
			if ( !size )
				hr = E_UNEXPECTED;
			else if ( 0 == (*output=(BYTE*)::CoTaskMemAlloc(size)) )
				hr = E_OUTOFMEMORY;
			else {
				#if defined(_DEBUG) && defined(CHECK_OPT)
					::KLDT_SerializeUsingSWM( "o:\\prague\\debug\\1.opt", node, "" );
				#endif
				*len = ::KLDT_SerializeToMemoryUsingSWM( (char*)*output, size, node, "" );
				hr = S_OK;
			}
		}
		if ( root )
			::DATA_Remove( root, 0 );
	}
	else if ( 0 == (*output=(BYTE*)::CoTaskMemAlloc(size)) )
		hr = E_OUTOFMEMORY;
	else {
		::memcpy( *output, buffer, *len=size );
		hr = S_OK;
	}
	
	::FreeResource( resource );
	return hr;
}



// ---
HRESULT CCodeGen::LoadContents( DWORD len, BYTE* contents ) {

	if ( m_iface ) 
		m_iface = 0;

	if ( m_plugin ) 
		m_plugin = 0;

  if ( m_tree ) 
    m_tree = 0;
  
  if ( !len || !contents )
		return E_INVALIDARG;

	HDATA cont = 0;
	if ( ::KLDT_DeserializeFromMemoryUsingSWM((char*)contents,len,&cont) && cont ) {
    HDATA cont2 = 0;
    
    DWORD type = CBaseInfo::DetectEnType( cont );
    if ( type == ifPlugin ) {
      m_plugin = cont;
      m_plugin.Own( true );
      m_iface = m_plugin.FirstIFace();
      return S_OK;
    }
    
    if (  type != ifIFace ) { // just a set of prototypes
      cont2 = ::DATA_Get_FirstEx( cont, 0, DATA_IF_ROOT_CHILDREN );
      if ( !cont2 )
        return CG_E_SERIALIZE_PARAMETER;
      type = CBaseInfo::DetectEnType( cont2 );
    }
    
    if ( type == ifIFace ) {
      if ( cont2 ) {
        m_tree = cont;
        m_tree.Own( true );
        m_iface = cont2;
      }
      else {
        m_iface = cont;
        m_iface.Own( true );
      }
      return S_OK;
    }
    else 
      ::DATA_Remove( cont, 0 );
  }
	return CG_E_SERIALIZE_PARAMETER;
}




// ---
HRESULT CCodeGen::LoadOptions( DWORD len, BYTE* contents ) {
	HDATA opt = 0;
	m_full = true;
	if ( ::KLDT_DeserializeFromMemoryUsingSWM((char*)contents,len,&opt) && opt ) {
		AVP_bool  val = 1;
		AVP_group lang;
		AVP_dword addr[3];
		HDATA     data;

		#if defined(_DEBUG) && defined(CHECK_OPT)
			::KLDT_SerializeUsingSWM( "d:\\prague\\debug\\2.opt", opt, "" );
		#endif

		addr[0] = FULL_INFO_PID;
		addr[1] = 0;
		data = ::DATA_Find( opt, addr );
		if ( data && (sizeof(val) == ::DATA_Get_Val(data,0,AVP_PID_VALUE,&val,sizeof(val))) )
			m_full = !!val;
		else
			m_full = true;

		addr[0] = BRICK_STYLE_PID;
		addr[1] = 0;
		data = ::DATA_Find( opt, addr );
		if ( data && (sizeof(val) == ::DATA_Get_Val(data,0,AVP_PID_VALUE,&val,sizeof(val))) )
			m_brick = !!val;
		else
			m_brick = false;
		
		m_lang = l_any;
		addr[0] = LANGUAGE_PID; 
		addr[1] = LANGUAGEID_PID;
		addr[2] = 0;
		data = ::DATA_Find( opt, addr );
		if ( data && (sizeof(lang) == ::DATA_Get_Val(data,0,AVP_PID_VALUE,&lang,sizeof(lang))) )
      m_lang = (tLang) (l_rus + lang);
    else
      m_lang = l_rus;
    ::DATA_Remove( opt, 0 );
  }
  return S_OK;
}





// ---
HRESULT CCodeGen::GenFile( tfnc fn, bool prototype, const char* src_name, const char* new_name, const char* old_name ) {
  
  if ( !src_name || !*src_name )
    return CG_FILE_NAME;
  
  if ( !CheckFile(src_name) ) 
    return CG_E_RDONLY;
  
  HRESULT hr = (this->*fn)( prototype, new_name );
  
  if ( SUCCEEDED(hr) )
    hr = SwitchFile( new_name, src_name, old_name );
  ::DeleteFile( new_name );
  
  return hr;
}




// ---
void CCodeGen::make_file_names( const char* name, const char* ext, CAPointer<char>& fn, CAPointer<char>& fnew, CAPointer<char>& fold ) {
  
  ::strlwr( (char*)name );
  
  fn = new char[MAX_PATH];
  fnew = new char[MAX_PATH];
  fold = new char[MAX_PATH];
  
  ::lstrcpy( fn, m_output_folder );
  ::lstrcat( fn, name );
  ::lstrcat( fn, "." );
  ::lstrcat( fn, ext );
  
  ::lstrcpy( fnew, fn );
  ::lstrcpy( fold, fn );
  
  ::lstrcat( fnew, ".new" );
  ::lstrcat( fold, ".bak" );
}




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




// ---
tBOOL CheckFile( const char* name ) {
  tDWORD attr = ::GetFileAttributes( name );
  tDWORD err = GetLastError();
  HANDLE h = INVALID_HANDLE_VALUE;
  tBOOL  res = TRUE;
  static char text[0x200];
		
  if ( (attr == 0xffffffff) && ((ERROR_FILE_NOT_FOUND == err) || (ERROR_PATH_NOT_FOUND == err)) )
    return TRUE;
  
	if (!CheckOutFile(name))
		return FALSE;

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



