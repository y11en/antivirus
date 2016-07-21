// CodeGen.cpp : Implementation of CCodeGen
#include "stdafx.h"
#include "CodeGen.h"
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include "Options.h"
#include "..\AVPPveId.h"
#include "..\PCGError.h"

/////////////////////////////////////////////////////////////////////////////
// CCodeGen

// ---
CCodeGen::CCodeGen() 
	: m_types( 0 ),
		m_plugin( 0, false ),
		m_iface( 0, false ),
		m_opt( 0 )
{
}



// ---
CCodeGen::~CCodeGen() {
}



// ---
STDMETHODIMP CCodeGen::GetInfo( DWORD* len, BYTE** info ) {
	return ExtractTemplate( IDR_IFACE_DESCRIPTION, len, info );
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
	CTreeInfo seance( len, buffer );
	return S_OK;
}



// ---
STDMETHODIMP CCodeGen::EndProject() {
	return S_OK;
}



// ---
STDMETHODIMP CCodeGen::GetImplementationOptionTemplate( DWORD* len, BYTE** options ) {
	return ExtractTemplate( IDR_OPTIONS_TEMPLATE, len, options );
}



// ---
STDMETHODIMP CCodeGen::ImplementationGenerate( DWORD clen, BYTE* cont, DWORD olen, BYTE* opt ) {
	int interfaces = 0;
	int selected = 0;
	int option_checked = 0;
	
	HRESULT hr = LoadContents( clen, cont );
	if ( FAILED(hr) )
		return hr;

	hr = LoadOptions( olen, opt );
	if ( FAILED(hr) )
		return hr;

	m_iface.GoFirst();
	while( m_iface ) {
		interfaces++;
		if ( m_iface.Selected() )
			selected++;
		if ( m_opt.some_option() )
			option_checked++;
		m_iface.GoNext();
	}

	m_iface = 0;
	return S_OK;
}




// ---
STDMETHODIMP CCodeGen::GetPrototypeOptionTemplate( DWORD *len, BYTE **options ) {
	return ExtractTemplate( IDR_OPTIONS_TEMPLATE, len, options );
}



// ---
STDMETHODIMP CCodeGen::PrototypeGenerate( DWORD clen, BYTE *cont, DWORD olen, BYTE *opt ) {

	int interfaces = 0;
	int selected = 0;
	int option_checked = 0;

	HRESULT hr = LoadContents( clen, cont );
	if ( FAILED(hr) )
		return hr;

	hr = LoadOptions( olen, opt );
	if ( FAILED(hr) )
		return hr;

	m_iface.GoFirst();
	while ( m_iface ) {
		interfaces++;
		if ( m_iface.Selected() )
			selected++;
		if ( m_opt.some_option() )
			option_checked++;
		m_iface.GoNext();
	}

	m_iface = 0;
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
HRESULT CCodeGen::ExtractTemplate( UINT id, DWORD* len, BYTE** output ) {
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

	if ( !len || !contents )
		return E_INVALIDARG;

	HDATA cont = 0;
	if ( ::KLDT_DeserializeFromMemoryUsingSWM((char*)contents,len,&cont) && cont ) {

		DWORD type = CBaseInfo::DetectEnType( cont );
		if ( type == ifPlugin ) {
			m_plugin = cont;
			m_plugin.Own( true );
			m_iface = m_plugin.FirstIFace();
			return S_OK;
		}
		else if ( type == ifIFace ) {
			m_iface = cont;
			m_iface.Own( true );
			return S_OK;
		}
		else 
			::DATA_Remove( cont, 0 );
	}
	return CG_E_SERIALIZE_PARAMETER;
}




// ---
HRESULT CCodeGen::LoadOptions( DWORD len, BYTE* contents ) {
	if ( !len || !contents )
		return E_INVALIDARG;
	else if ( !m_opt.Init(len,contents) ) 
		return CG_E_SERIALIZE_PARAMETER;
	else 
		return S_OK;
}



