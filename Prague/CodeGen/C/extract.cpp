#include "stdafx.h"
#include "CodeGen.h"
#include <serialize/kldtser.h>



// ---
HRESULT CCodeGen::ExtractTemplate( UINT id, AVP_dword* addr, DWORD* len, BYTE** output, bool hdr ) {
	
	if ( !len || !output )
		return E_INVALIDARG;

	else {
		*len = 0;
		*output = 0;
	}
	
	HINSTANCE hInstance = _Module.GetModuleInstance();
	
	HRSRC found = ::FindResource( hInstance, MAKEINTRESOURCE(id), RT_RCDATA );
  if ( !found )
    return S_FALSE;
  
	AVP_dword size = ::SizeofResource( hInstance, found );
  if ( !size )
    return S_FALSE;

	HGLOBAL resource = ::LoadResource( hInstance, found );
  if ( !resource )
    return HRESULT_FROM_WIN32( GetLastError() );

	HRESULT hr = S_OK;
  HDATA root = 0;
  void* buffer = ::LockResource( resource );

	if ( !buffer ) 
    hr = HRESULT_FROM_WIN32( GetLastError() );

  else if ( !addr ) {
    *output = (BYTE*)::CoTaskMemAlloc( size );
    if ( *output ) 
      ::memcpy( *output, buffer, *len=size );
    else
      hr = E_OUTOFMEMORY;
  }
  else if ( !::KLDT_DeserializeFromMemoryUsingSWM((char*)buffer,size,&root) || !root ) 
    hr = E_UNEXPECTED;

  else {
		HDATA node = ::DATA_Find( root, addr );
		if ( !node ) 
      hr = E_UNEXPECTED;

    else {

			if ( hdr ) {
				HDATA del;
				MAKE_ADDR1( a, 0 );
				a[0] = H___HEADER_FILE_GENERATION_PID;
				del = ::DATA_Find( node, a );
				if ( del )
					::DATA_Remove( node, a );
				a[0] = C___SOURCE_FILE_GENERATION_PID;
				del = ::DATA_Find( node, a );
				if ( del )
					::DATA_Remove( node, a );
				a[0] = MAKE_INTERFACE_DATA_PUBLIC_PID;
				del = ::DATA_Find( node, a );
				if ( del )
					::DATA_Remove( node, a );
			}
			
			size = ::KLDT_SerializeToMemoryUsingSWM( 0, 0, node, "" );
			if ( size ) {
        *output = (BYTE*)::CoTaskMemAlloc( size );
				if ( *output ) 
					*len = ::KLDT_SerializeToMemoryUsingSWM( (char*)*output, size, node, "" );
				else
					hr = E_OUTOFMEMORY;
			}
			else
				hr = E_UNEXPECTED;
		}
		::DATA_Remove( root, 0 );
	}
	::FreeResource( resource );
	return hr;
}



// ---
HRESULT CCodeGen::ExtractString( UINT id, BYTE** output ) {
	
	if ( !output ) 
		return E_INVALIDARG;
	
	static char buff[1000];
	HINSTANCE inst = _Module.GetModuleInstance();
	int len = ::LoadString( inst, id, buff, sizeof(buff) ) + 1;
	
	if ( len ) {
		IMalloc* alloc = 0;
		HRESULT hr = ::CoGetMalloc( 1, &alloc );
		if ( SUCCEEDED(hr) ) {
			*output = (BYTE*)alloc->Alloc( len );
			::memcpy( *output, buff, len );
			alloc->Release();
		}
		return hr;
	}
	else
		return HRESULT_FROM_WIN32( ::GetLastError() );
}



