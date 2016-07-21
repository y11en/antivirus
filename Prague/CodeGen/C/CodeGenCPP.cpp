// CodeGenCPP.cpp : Implementation of CCodeGenCPP
#include "stdafx.h"
#include "CodeGen.h"
#include "..\PCGError.h"

/////////////////////////////////////////////////////////////////////////////
// CCodeGenCPP


CCodeGenCPP::CCodeGenCPP()
{
	m_desc_id = 2;
	m_source_ext = "cpp";
	m_this = "this";
	m_prop_prefix = "p";
}

// ---
STDMETHODIMP CCodeGenCPP::GetDescription( BYTE** description ) {
	return ExtractString( IDS_IFACE_DESCRIPTION_CPP, description );
}


// ---
STDMETHODIMP CCodeGenCPP::GetVersion( BYTE** version ) {
	return ExtractString( IDS_IFACE_VERSION_CPP, version );
}

// ---
HRESULT CCodeGenCPP::GenClassDefinition( COutputFile& of, bool prototype )
{
	CIntMethodInfo imi( m_iface );
	CPubMethodInfo pmi( m_iface );

	COutputSect *sect = new COutputSect( of, if_cpp_defs, 0, false );

	//PutFmtString( *sect, 0, "struct pr_novtable %s : public cObjImpl "BRACE_OS, m_data_name );
	PutFmtString( *sect, 0, "struct pr_novtable %s : public c%s "BRACE_OS, m_data_name, m_iface.Name() );

	PutString(*sect, 0, "private:");
	PutString(*sect, 0, "// Internal function declarations");

	for(imi.GoFirst(); imi; imi.GoNext())
		if( imi.Selected() )
			IFaceMethodDefsToSource(*sect, imi, true);

	PutString(*sect, 0, 0);
	PutString(*sect, 0, "// Property function declarations");

  uint c = m_methods.Count();
  for( uint i=0; i<c; i++ )
    PropMethodDefsToSource( *sect, m_methods[i], true );

	PutString(*sect, 0, 0);
	PutString(*sect, 0, "public:");
	PutString(*sect, 0, "// External function declarations");

	for(pmi.GoFirst(); pmi; pmi.GoNext())
		if( !PutSeparator(*sect, pmi) )
			IFaceMethodDefsToSource(*sect, pmi, true);

	uint shift = m_data_single ? 0 : 2;

	PutString(*sect, 0, 0);
	PutString(*sect, 0, "// Data declaration");

	DWORD count = 0;
	CDataStructInfo ds( m_iface );
	for( ;ds.IsOK() && SUCCEEDED(StructToSource(*sect, ds, shift, cTRUE)); ds.GoNext())
		count++;

	sect = new COutputSect( of, if_cpp_defs_end, 0, false );
	PutString(*sect, 0, "public:");
	PutFmtString(*sect, 2, "mDECLARE_INITIALIZATION(%s)", m_data_name );
	PutString( *sect, 0, BRACE_C_SEMICOLON );

	return S_OK;
}

HRESULT CCodeGenCPP::RegisterIFaceBegin( COutputFile& file )
{
	COutputSect* sect = new COutputSect( file, reg_call_cpp, 0, false );
	PutFmtString( *sect, 0, (const char*)IDS_REGISTER_FUNC_COMMENT_FMT, m_iface.Name() );
	PutFmtString( *sect, 0, "tERROR %s::Register( hROOT root ) {", m_data_name );

  PutString( *sect, 2, "tERROR error;" );
	return S_OK;
}

HRESULT CCodeGenCPP::RegisterCallToSource( COutputFile& file )
{
	COutputSect* sect = new COutputSect( file, reg_call, 0, false ); // IDS_REGISTER_CALL_STAMP
	RegisterCallImplToSource(*sect);

	sect = new COutputSect( file, reg_call_cpp_end, 0, false );
  PutString( *sect, 2, "return error;" );
	PutString( *sect, 0, BRACE_CS );
	PutString( *sect, 0, 0 );
	PutFmtString( *sect, 0, "tERROR pr_call %s_Register( hROOT root ) { return %s::Register(root); }",
		m_iface.Name(), m_data_name );

	return S_OK;
}

// ---
HRESULT CCodeGenCPP::RegisterCallGetDataSize( char *buffer )
{
	::wsprintf( buffer, "sizeof(%s)-sizeof(cObjImpl)", m_data_name );
	return S_OK;
}

// ---
HRESULT CCodeGenCPP::InternalTableToSource( COutputFile& ofile, CIntMethodInfo &imi )
{
	const char *iface_name = m_iface.Name();

	COutputSect *sect = new COutputSect( ofile, in_meth_tbl, 0, false );

	PutFmtString( *sect, 0, "mINTERNAL_TABLE_BEGIN(%s)", iface_name );

	for(imi.GoFirst(); imi; imi.GoNext())
	{
		if( imi.IsSeparator() || !imi.Selected() )
			continue;

		uint len = imi.Name( A3 );
		if( len <= 1 )
			strcpy(W3, "unknown_method");

		if( !strcmp(W3, "Recognize") || !strcmp(W3, "ObjectNew") )
			PutFmtString( *sect, 2, "mINTERNAL_STATIC(%s)", W3 );
		else
			PutFmtString( *sect, 2, "mINTERNAL_METHOD(%s)", W3 );
	}

	PutFmtString( *sect, 0, "mINTERNAL_TABLE_END(%s)", iface_name );
	return S_OK;
}

// ---
HRESULT CCodeGenCPP::ExternalTableToSource( COutputFile& ofile, CPubMethodInfo &pmi )
{
	const char *iface_name = m_iface.Name();

	COutputSect &sect = *new COutputSect( ofile, ex_meth_tbl, 0, false );

	sprintf(W2, "%s%s", (char*)m_plugin.Name(), iface_name);

	bool is_extended = false;
	for(pmi.GoFirst(); pmi; pmi.GoNext())
	{
		if( pmi.IsSeparator() )
			continue;

		uint len = pmi.Name( A3 );
		if( len <= 1 )
			strcpy(W3, "unknown_method");

		const char *iname = iface_name;
		if( pmi.IsExtended() )
			is_extended = true, iname = W2;

		PutFmtString( sect, 2, "mEXTERNAL_METHOD(%s, %s)", iname, W3 );
	}

	if( !is_extended )
		sprintf(W1, "mEXTERNAL_TABLE_BEGIN(%s)", iface_name);
	else
		sprintf(W1, "mEXTERNAL_TABLE_BEGIN_EX(%s, %s)", W2 , iface_name);
	PutString( sect, 0, W1, 0 );

	PutFmtString( sect, 0, "mEXTERNAL_TABLE_END(%s)", iface_name );
	return S_OK;
}

const char * CCodeGenCPP::MakePropFuncName( const char *func_name )
{
	sprintf(W0, "%s::%s", m_data_name, func_name);
	return W0;
}

HRESULT CCodeGenCPP::MakeTablePropFuncName( CAPointer<char> &name, const char *func_name )
{
  if( func_name )
	  sprintf(W0, "FN_CUST(%s)", func_name);
  else
    strcpy(W0, "NULL");
	SET_STRING(name, W0);
	return S_OK;
}

// ---
HRESULT CCodeGenCPP::PropMethodDefsToSource( COutputSect &sect, PropMethod* method, bool forward_decl )
{
	const char *method_name = method->Name();
	AVP_dword len;

	if( forward_decl )
		len = sprintf( W1, "\ttERROR pr_call %s( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );", method_name);
	else
		len = sprintf( W1, "tERROR %s::%s( tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size ) {", m_data_name, method_name);

	return PutString( sect, 0, W1 );
}

// ---
AVP_dword	CCodeGenCPP::MakeIFaceMethodPrefix( char* buffer, const CMethodInfo& mi, bool forward_decl )
{
  AVP_dword s = 0;

  if( 1 > mi.Name(A3) )
    strcpy( W3, "unknown method name" );

	bool f_static = !strcmp(W3, "Recognize") || !strcmp(W3, "ObjectNew");

	if( forward_decl )
		s = sprintf(buffer, "\t%stERROR pr_call %s( ", f_static ? "static " : "", W3);
	else
		s = sprintf(buffer, "tERROR %s::%s( ", m_data_name, W3);

  if( f_static )
		s += sprintf(buffer+s, "%s", "hOBJECT _that");
	return s;
}

// ---
void	CCodeGenCPP::InitDataStructName(CDataStructInfo &ds)
{
	m_data_name_len = ds.Name(A3);

	if( !m_data_single || m_data_name_len <= 1 ) 
		m_data_name_len = sprintf(W3, "c%sImpl", m_iface.Name());

	m_data_name = (char*)::memcpy( new char[m_data_name_len], W3, m_data_name_len );
}




