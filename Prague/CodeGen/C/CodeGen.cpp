// CodeGen.cpp : Implementation of CCodeGen
#include "stdafx.h"
#include "CodeGen.h"
#include "Options.h"

#include "..\AVPPveId.h"
#include "..\PCGError.h"

// commonfiles
//#include <datatreeutils/dtutils.h>
//#include <dtdropper.h>
#include <serialize/kldtser.h>
#include <avpprpid.h>
#include <stuff\comutils.h>
#include <_avpio.h>

/////////////////////////////////////////////////////////////////////////////
// CCodeGen

const IID IID_IFaceInfo = {0xEB1D2F6E,0xCAAC,0x11D4,{0xB7,0x65,0x00,0xD0,0xB7,0x17,0x0E,0x50}};
const CLSID CLSID_PrIFaceInfo = {0xEB1D2F6F,0xCAAC,0x11D4,{0xB7,0x65,0x00,0xD0,0xB7,0x17,0x0E,0x50}};

// ---
CCodeGen::CCodeGen() 
	:	m_types( 0 ),
		m_plugin( 0, false ),
		m_tree( 0, false ),
		m_iface( 0, false ),
		m_opt(0),
		m_int_type_name( 0 ),
		m_data_single( true ),
		m_data_name( 0 ),
		m_data_name_len( 0 ),
		m_output_folder( 0 ),
		m_public_folder( 0 ),
		m_plugin_public( false ),
		m_kernel( false ),
		m_start_project( true ),
		m_version( 0 ),
		m_this("_this"),
		m_source_ext("c"),
		m_inc_prefix("iface/"),
		m_prop_prefix(""),
		m_ex_methods(NULL),
		m_is_ex_methods(false),
		m_additional_ifaces(),
		m_additional_includes(0)
{
	m_desc_id = 0;
}

const char*  CCodeGen::m_desriptions[] = 
{
	"ANSI C Code Generator 1.0",
	"ANSI C Code Generator 2.0",
	"ANSI C++ Code Generator 1.0",
	NULL
};

// ---
CCodeGen::~CCodeGen() {
}



// ---
STDMETHODIMP CCodeGen::StartProject( DWORD len, BYTE* buffer ) {
	
	m_output_folder = 0;
	m_public_folder = 0;
	m_start_project = true;
	m_change_version = false;
	m_version = 0;
	m_plugin_mode = false;
	m_iface_names.RemoveAll();

	CTreeInfo seance( len, buffer );
  if ( seance ) {
    len = seance.StrAttr( VE_PID_CGPI_PRIVATEDIR, A3 );
    if ( len <= 1 )
      m_output_folder = 0;
    else {
      if ( W3[len-2] != '\\' ) {
			  W3[len-1] = '\\';
			  W3[len++] = 0;
		  }
		  m_output_folder = (char*)::memcpy( new char[len], W3, len );
		}

    len = seance.StrAttr( VE_PID_CGPI_PUBLICDIR, A3 );
		if ( len <= 1 ) 
      m_public_folder = 0;
    else {
			if ( (W3[len-2] != '\\') && (W3[len-2] != '/') ) {
				W3[len-1] = '\\';
				W3[len++] = 0;
			}
			m_public_folder = (char*)::memcpy( new char[len], W3, len );
		}
	  return S_OK;
  }
	else
		return MAKE_HRESULT(1,20,ERROR_INVALID_DATA);
}




// ---
STDMETHODIMP CCodeGen::PrototypeGenerate( DWORD clen, BYTE *cont, DWORD olen, BYTE *opt ) {
	
	HRESULT hr;

  if ( (m_public_folder == 0) || (*m_public_folder == 0) )
    hr = MAKE_HRESULT(1,20,ERROR_CURRENT_DIRECTORY);
  else if ( FAILED(hr=LoadContents(clen,cont)) )
    ;
	else if ( FAILED(hr=LoadOptions(olen,opt)) )
		;
	else 
		hr = GenProtoFiles();
	
	if ( m_additional_includes )
		m_additional_includes = 0;
	
	m_iface = 0;
	m_plugin = 0;
	m_tree = 0;
	return hr;
}




// ---
STDMETHODIMP CCodeGen::ImplementationGenerate( DWORD clen, BYTE* cont, DWORD olen, BYTE* opt ) {

	HRESULT hr;

  if ( (m_output_folder == 0) || (*m_output_folder == 0) )
    hr = MAKE_HRESULT(1,20,ERROR_INVALID_DATA);
	else if ( FAILED(hr=LoadContents(clen,cont)) )
		;
	else if ( FAILED(hr=LoadOptions(olen,opt)) )
		;
	else if ( FAILED(hr=GenPluginFiles()) )
		;
	else 
		hr = GenSourceFiles();

	if ( m_additional_includes )
		m_additional_includes = 0;

	m_iface = 0;
	m_plugin = 0;
	m_tree = 0;
	return hr == S_FALSE ? S_OK : hr;
}



// ---
STDMETHODIMP CCodeGen::EndProject() {
	
	m_output_folder = 0;
	m_public_folder = 0;
	m_start_project = false;
	m_change_version = false;
	m_version = 0;
	m_plugin_mode = false;
	m_iface_names.RemoveAll();
	delete m_ex_methods;
	m_ex_methods = NULL;
	m_is_ex_methods = false;
	
	m_additional_ifaces.Release();
	m_additional_ifaces = 0;

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

	if ( !err_str )
		return E_INVALIDARG;

	IMalloc* alloc = 0;
	HRESULT hr = ::CoGetMalloc( 1, &alloc );
	if ( SUCCEEDED(hr) ) {
		*err_str = (BYTE*)alloc->Alloc( 0x200 );
		alloc->Release();
		if ( *err_str ) {
			const char* msg = 0;
			switch( err ) {
				case CG_E_IFACE_NAME                       : msg = "Interface name not defined"; break;
				case CG_E_IFACE_TYPE                       : msg = "Interface type not defined"; break;
				case CG_E_IFACE_DEF                        : msg = "Error in interface definition"; break;
				case CG_E_OBJECT_NAME                      : msg = "Object name not defined"; break;
				case CG_E_METHODS                          : msg = "Interface has no methods"; break;
				case CG_E_METHOD_NAME                      : msg = "Method name not defined"; break;
				case CG_E_METHOD_RETURN_TYPE               : msg = "Return type not defined"; break;
				case CG_E_PARAM_TYPE                       : msg = "Method parameter type not defined"; break;
				case CG_E_PROP_DEFINITION                  : msg = "Error in property definition"; break;
				case CG_E_PROP_NAME                        : msg = "Property name not defined"; break;
				case CG_E_PROP_ID                          : msg = "Property ID not defined"; break;
				case CG_E_PROP_TYPE                        : msg = "Propetry type not defined"; break;
				case CG_E_STRUCT_NAME                      : msg = "Data struct name not defined"; break;
				case CG_E_MEMBER_TYPE                      : msg = "Type of data struct member not defined"; break;
				case CG_E_MEMBER_NAME                      : msg = "Name of data struct member not defined"; break;
				case CG_E_MEMBER_PROP                      : msg = "Attributes of data struct member not properly defined"; break;
				case CG_E_MEMBER_PROP_FN_RW_COINC          : msg = "Names for \"Get\" and \"Set\" property method are equal"; break; // сопадение имен ф-ий R & W
				case CG_E_MEMBER_METHOD_TABLE_EMPTY        : msg = "Interface has no external methods defined"; break; // пустая таблица методов 
				case CG_E_INT_METHOD_TABLE_EMPTY           : msg = "Interface has no table of internal methods"; break; // пустая таблица системных методов 
				case CG_E_SERIALIZE_PARAMETER              : msg = "Parameter (de)serialization ends with error"; break;
				case CG_E_DATA_MATCH                       : msg = "Data doesn't match to operation"; break;
				case CG_E_PROP_WITHOUT_VALUE               : msg = "Some property not implemented"; break;
				case CG_E_BACKUP_ERROR                     : msg = "Cannot create backup copy"; break;
				case CG_E_RDONLY                           : msg = "File is READ ONLY or already open"; break;
				case CG_E_LOAD_ERROR                       : msg = "Cannot load source file"; break;
				case CG_E_PLUGIN_UNDEFINED                 : msg = "Plugin mnemonic id is not defined -- you have to define it!\nGo to the plugin implementaition, select \"Plugin\" lable in left window and fill \"Name\" property"; break;
				case MAKE_HRESULT(1,20,ERROR_INVALID_DATA) : msg = "Output folder is not defined"; break;
        case MAKE_HRESULT(1,20,ERROR_CURRENT_DIRECTORY) : msg = "Public folder is not defined"; break;
        case CG_FILE_NAME:
				case MAKE_HRESULT(1,20,ERROR_BAD_FORMAT)   : msg = "Source file name is not defined in the interface"; break;
			}
			if ( msg ) {
				::lstrcpy( (char*)*err_str, msg );
				return S_OK;
			}
			else if ( ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,err,0,(char*)*err_str,0x200,0) )
				return S_OK;
			else
				return HRESULT_FROM_WIN32( ::GetLastError() );
		}
		else
			return E_OUTOFMEMORY;
	}
	else
		return hr;
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
	if ( len && objtypes ) {
		if ( m_types.Init(len,objtypes) ) {
			/* save types to file
			::LoadIO();
			::KLDT_Serialize( "d:\\types.ptt", m_types.Data(), "" );
			*/
		}
		else
			return CG_E_SERIALIZE_PARAMETER;
	}
	return S_OK;
}


#define NO_CHECK_OPT
#if defined(_DEBUG) && defined(CHECK_OPT)
	void save_opt( DWORD len, BYTE* cont ) {
		HANDLE file = CreateFile( "o:\\prague\\debug\\1.opt", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
		if ( INVALID_HANDLE_VALUE != file ) {
			WriteFile( file, cont, len, &len, 0 );
			CloseHandle( file );
		}
	}
#else
	#define save_opt(a,b) 
#endif


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

	save_opt( len, contents );

	HDATA cont = 0;
	HDATA cont2 = 0;
	if ( ::KLDT_DeserializeFromMemoryUsingSWM((char*)contents,len,&cont) && cont ) {

		DWORD type = CBaseInfo::DetectEnType( cont );
		if ( type == ifPlugin ) {
			m_plugin = cont;
			m_plugin.Own( true );
			m_iface = m_plugin.FirstIFace();
			m_kernel = !::lstrcmpi( "Kernel", m_plugin.Name() );
			return S_OK;
		}

		if ( type != ifIFace ) { // just a set of prototypes
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
	if ( !len || !contents )
		return E_INVALIDARG;
	else if ( m_opt.Init(len,contents) ) 
		return S_OK;
	else 
		return CG_E_SERIALIZE_PARAMETER;
}


// ---
STDMETHODIMP CCodeGen::GetPrototypeOptionTemplate( DWORD *len, BYTE **options ) {
	MAKE_ADDR1( addr, OPTIONS_PID );
	return ExtractTemplate( IDR_OPTIONS_TEMPLATE, addr, len, options, true );
}



// ---
STDMETHODIMP CCodeGen::GetImplementationOptionTemplate( DWORD* len, BYTE** options ) {
	MAKE_ADDR1( addr, OPTIONS_PID );
	return ExtractTemplate( IDR_OPTIONS_TEMPLATE, addr, len, options, false );
}



// ---
STDMETHODIMP CCodeGen::GetInfo( DWORD* len, BYTE** info ) {
	return ExtractTemplate( IDR_IFACE_DESCRIPTION, 0, len, info, false );
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

