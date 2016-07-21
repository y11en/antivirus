// CCodeGen2.cpp : Implementation of CCodeGen2
#include "stdafx.h"
#include "CodeGen.h"
#include "..\PCGError.h"
#include "..\AVPPveId.h"

#include <vector>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CCodeGen2


CCodeGen2::CCodeGen2()
{
	m_plugin_public = true;
	m_desc_id = 1;
}

// ---
STDMETHODIMP CCodeGen2::GetVersion( BYTE** version ) {
	return ExtractString( IDS_IFACE_VERSION2, version );
}

HRESULT CCodeGen2::GenPluginSourceInclude( COutputSect& sect )
{
	return PutFmtString( sect, 0, "#include <%si_root.h>", m_inc_prefix );
}

HRESULT CCodeGen2::GenPluginExportMethods( COutputFile& of )
{
	COutputSect* sect = new COutputSect( of, pl_exports, 0, false );

	if( m_plugin.MnemonicID(A1) < 1 )
		strcpy(W1, "unknown");

	PutString( *sect, 0, "#ifdef IMPEX_TABLE_CONTENT" );

	vector<string> decl_arr;
	CPubMethodInfo &mi = *m_ex_methods;
	for(mi.GoFirst(); mi; mi.GoNext())
	{
    if ( mi.IsSeparator() ) {
      PutString( *sect, 0, "// ---------------------------" );
      decl_arr.push_back( "// ---------------------------" );
      if ( mi.Name() ) {
        ::wsprintf( W2, "// %s", mi.Name() );
        PutString( *sect, 0, W2 );
        decl_arr.push_back(W2);
      }
      continue;
    }

    string em_name = mi.Name(A3) > 0 ? W3 : "unknown method name";
		string ret_type = "tERROR";
		if( HAS_RET_VAL(mi.TypeID(), mi.TypeOfType()) )
		{
			CObjTypeInfo oti( m_types );
			if ( mi.TypeName( &oti, A3 ) > 1 ) 
				ret_type = W3;
		}

		AVP_dword s = sprintf(W2, "IMPEX_DECL %s IMPEX_FUNC(%s)( ", ret_type.c_str(), em_name.c_str());
		s += MakeIFaceMethodPosix(W2 + s, mi, true);
		strcpy(W2 + s, "IMPEX_INIT;");

		decl_arr.push_back(W2);
		PutFmtString( *sect, 0, "{ (tDATA)&%s, %s, 0x%08xl },", em_name.c_str(), W1, mi.MethodID()/*decl_arr.size()*/ );
	}

	PutString( *sect, 0, 0);
	PutString( *sect, 0, "#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)" );

	PutFmtString( *sect, 0, "#include <iface/impexhlp.h>" );
	PutString( *sect, 0, 0);

	for( UINT i = 0; i < decl_arr.size(); i++)
		PutString( *sect, 0, decl_arr[i].c_str() );

	PutString( *sect, 0, "#endif" );

	return S_OK;
}

HRESULT CCodeGen2::GenPluginSourceExportImpl( COutputFile& of, CPubMethodInfo &mi )
{
	COutputSect& sect = *new COutputSect( of, pl_ex_impl, (char*)mi.Name(), false );
	if ( m_opt.ext_comments() )
		PutMethodExComment(sect, mi);

	string em_name = mi.Name(A3) > 0 ? W3 : "unknown method name";
	string ret_type = "tERROR";
	if( HAS_RET_VAL(mi.TypeID(), mi.TypeOfType()) )
	{
		CObjTypeInfo oti( m_types );
		if ( mi.TypeName( &oti, A3 ) > 1 ) 
			ret_type = W3;
	}

	AVP_dword s = sprintf(W2, "%s %s( ", ret_type.c_str(), em_name.c_str());
	MakeIFaceMethodPosix(W2 + s, mi, false);

	PutString( sect, 0, W2 );

	PutString( sect, 0, 0 );
	PutString( sect, 2,   "// Place your code here" );
	PutString( sect, 0, 0 );

	if( ret_type != "void" && ret_type != "tVOID" )
		PutString( sect, 2, "return 0;" );

	PutString( sect, 0, BRACE_CS );
	return S_OK;
}

HRESULT CCodeGen2::GenPluginIncludeInterfaceDefs( COutputSect& sect )
{
	CPArray<char> names, typedefs, methods, defines;
	ParseIFaceMethods(names, typedefs, methods, defines, false, true);

	uint i, len, max_len = 0, c = methods.Count();
	if( !c )
		return S_OK;

	PutString( sect, 0, 0 );
	PutFmtString( sect, 0, "// plugin interface" );

	for(i = 0; i < c; i++)
	{
		const char* tdef = typedefs[i];
		if( *tdef != '/' )
		{
			if( (len = strlen(tdef)) > max_len )
				max_len = len;
			PutFmtString(sect, 0, (const char*)IDS_FUNC_TYPEDEF_FMT, methods[i]);
		}
		else
			PutString(sect, 0, tdef);
	}

	sprintf(W1, "%s%s", m_plugin.Name(), m_iface.Name());

	PutString(sect, 0, 0 );
	PutFmtString(sect, 0, "typedef struct tag_i%sVtbl "BRACE_OS, W1);
	PutFmtString(sect, 2, "struct i%sVtbl prt;", m_iface.Name());

	char l_align[MAX_PATH];
	for(i = 0; i < c; i++)
	{
		const char* tdef = typedefs[i];
		memset(l_align, ' ', len = max_len-strlen(tdef)); l_align[len]=0;
		if( *tdef != '/' )
			PutFmtString(sect, 2, "fnp%s %s %s;", tdef, l_align, names[i]);
		else
			PutString(sect, 0, tdef);
	}

	PutFmtString(sect, 0, BRACE_CS" i%sVtbl;", W1);
	PutString(sect, 0, 0 );

  PutFmtString(sect, 0, "typedef struct tag_%s "BRACE_OS, W1);
	PutFmtString(sect, 2, "const i%sVtbl* vtbl; // pointer to the \"%s\" virtual table", W1, W1);
	PutFmtString(sect, 2, "const iSYSTEMVtbl* sys;  // pointer to the \"SYSTEM\" virtual table");
	PutFmtString(sect, 0, BRACE_CS" *h%s;", W1);

	PutString(sect, 0, 0);

	for(i = 0; i < c; i++)
		PutString(sect, 0, defines[i]);

	PutString(sect, 0, 0);

	PutString( sect, 0, "#if defined (__cplusplus)" );
	PutFmtString( sect, 0, "struct c%s : public c%s "BRACE_OS, W1, m_iface.Name() );

	CPArray<CIfaceMethod> i_methods;
	CollectMethods(i_methods, true);
	
	c = i_methods.Count();

	for(i = 0; i < c; i++)
		CppMethodToHdr(sect, *i_methods[i], 2);
	
	PutString( sect, 0, 0 );
	PutString( sect, 2, "operator hOBJECT() { return (hOBJECT)this; }" );
	PutFmtString( sect, 2, "operator h%s()   { return (h%s)this; }", W1, W1 );
	PutString( sect, 0, 0 );

  CPropTypeInfo pti(m_types);
	for(CPropInfo pi( m_iface ); pi; pi.GoNext())
		if( !pi.Predefined() && !pi.Native() )
			CppPropToHdr( sect, pi, pti, 2);
	
	PutString( sect, 0, BRACE_CS";" );
	PutString( sect, 0, 0 );

	PutString( sect, 0, "#endif // if defined (__cplusplus)");
	return S_OK;
}

HRESULT CCodeGen2::GenPluginIncludeDefs( COutputFile& of )
{
	CCodeGen::GenPluginIncludeDefs(of);

	HRESULT hr = S_OK;

	set<string> const_set, msg_set, prop_set, type_set, error_set;
	bool is_empty;

	int enum_flags = ENUM_IF_INIT_EXPORT, c;
	while( EnumIFaces(enum_flags) )
	{
		const char *iface_name = m_iface.Name();

		COutputSect* h_sect = of.find( 0, h_includes );
		if( h_sect )
		{
			if( m_iface.HeaderName(A3) > 1 )
			{
				_strlwr( W3 ); if( !strstr(W3,".h") ) strcat( W3, ".h" );
				PutFmtString( *h_sect, 0, "#include <%s%s>", m_inc_prefix, W3 );
			}
			else
				PutString( *h_sect, 0, "#error Cannot get interface name for header including" );
		}

		COutputSect& sect = *new COutputSect( of, pl_definitons, iface_name, false );

		PutIFaceComment(of, &sect);

		// interface types
		c = sect.Count(), is_empty = true;
		for( CTypeInfo ti( m_iface ); ti; ti.GoNext() )
		{
			if( ti.IsNative() )
				continue;

			pair<set<string>::iterator, bool> pit = type_set.insert((const char*)ti.Name());
			TypeToHdr(sect, ti, !pit.second);
			is_empty = false;
		}
		if( !is_empty )
		{
			PutString( sect, 0, 0, c );
			PutString( sect, 0, "// types", c+1 );
		}

		// interface constants
		c = sect.Count(), is_empty = true;
		for( CConstantInfo ci( m_iface ); ci; ci.GoNext() )
		{
			if( ci.IsNative() )
				continue;

			pair<set<string>::iterator, bool> pit = const_set.insert((const char*)ci.Name());
			ConstToHdr(sect, ci, false, !pit.second);
			is_empty = false;
		}
		if( !is_empty )
		{
			PutString( sect, 0, 0, c );
			PutString( sect, 0, "// constants", c+1 );
		}

		// interface errors
		c = sect.Count(), is_empty = true;
		AVP_dword idx = 1;
		for( CErrInfo ei( m_iface ); ei; ei.GoNext() )
		{
			if( ei.IsNative() )
				continue;

			pair<set<string>::iterator, bool> pit = error_set.insert((const char*)ei.Name());
			ErrorToHdr(sect, ei, idx, !pit.second, false);
			is_empty = false;
		}
		if( !is_empty )
		{
			PutString( sect, 0, 0, c );
			PutString( sect, 0, "// errors", c+1 );
		}

		// interface properties
		c = sect.Count(), is_empty = true;

		DWORD ws[2] = { 0 };
		CPropInfo pi( m_iface );
		for( ; pi; pi.GoNext())
			CalcPropWidths( pi, false, ws );

		for( pi.GoFirst(); pi; pi.GoNext() ) {
			if( pi.Predefined() || pi.Native() )
				continue;

			pair<set<string>::iterator, bool> pit = prop_set.insert((const char*)pi.Name());
			PropToInclude(sect, pi, false, ws, !pit.second);
			is_empty = false;
		}
		if( !is_empty ) {
			PutString( sect, 0, 0, c );
			PutString( sect, 0, "// properties", c+1 );
		}

		// interface messages
		for( CMsgClassInfo mci( m_iface ); mci; mci.GoNext() )
		{
			pair<set<string>::iterator, bool> pit = msg_set.insert((const char*)mci.Name());
			MsgClassInfoToHdr(sect, mci, false, !pit.second);
		}

		// interface declarations
		GenPluginIncludeInterfaceDefs(sect);
	}
	return S_OK;
}

HRESULT CCodeGen2::GenPluginIncludeFile( COutputFile& of, bool v2_public )
{
	CCodeGen::GenPluginIncludeFile(of,true);

	if( m_is_ex_methods )
			GenPluginExportMethods(of);

	return S_OK;
}

HRESULT CCodeGen2::GenPluginSourceImpl( COutputFile& of )
{
  COutputSect &sect = *new COutputSect( of, pl_ex_defs, 0, false );

	int enum_flags = 0;
	while( EnumIFaces(enum_flags) )
	{
		tDWORD len;
		if( (len = m_iface.Name(A3)) < 1 )
			continue;

		m_iface_names.Add( (char*)::memcpy(new char[len],W3,len) );

		PutFmtString( sect, 0, "extern tERROR pr_call %s_Register( hROOT root );", m_iface.Name() );
	}

	PutString( sect, 0, 0 );

	int len = m_plugin.Name( A3 );
	if( m_is_ex_methods && len > 1 )
	{
		_strlwr( W3 );
		PutString( sect, 0, "#define  IMPEX_EXPORT_LIB" );
		PutFmtString( sect, 0, "#include <plugin/p_%s.h>", W3 );
		PutString( sect, 0, 0 );
		PutString( sect, 0, "#define  IMPEX_TABLE_CONTENT" );
		PutString( sect, 0, "EXPORT_TABLE( export_table )" );
		PutFmtString( sect, 0, "#include <plugin/p_%s.h>", W3 );
		PutString( sect, 0, "EXPORT_TABLE_END" );
	}

	CCodeGen::GenPluginSourceImpl(of);

	if( !m_is_ex_methods )
		return S_OK;

	CPubMethodInfo &mi = *m_ex_methods;
	for(mi.GoFirst(); mi; mi.GoNext())
		GenPluginSourceExportImpl(of, mi);

	return S_OK;
}

bool CCodeGen2::IsGenType( GenType type )
{
	if( m_prototype && type == Source && m_opt.private_structure() )
		return false;
	return CCodeGen::IsGenType(type);
}

HRESULT CCodeGen2::GenIncludeFile( COutputFile& of )
{
	HRESULT hr = S_OK;
	
  if ( FAILED(hr=PutHeader(of,m_hdr_name)) )
		return hr;
	if ( FAILED(hr=HeaderToInclude(of,m_hdr_name)) )
		return hr;
	if ( FAILED(hr=GenIncludeDefinition(of, true)) )
		return hr;
	if ( FAILED(hr=FooterToInclude(of,m_hdr_name)) )
		return hr;
	return hr;
}

HRESULT CCodeGen2::GenSourceFile( COutputFile& of )
{
	HRESULT hr;
  
	if ( FAILED(hr=PutHeader(of,m_src_name)) )
		return hr;

	PrivateDecl( of );
	VersionDecl( of );

	if( m_opt.private_structure() )
	{
		if ( FAILED(hr=GenIncludeDefinition(of, false)) )
			return hr;
	}
	else
	{
		COutputSect *sect = new COutputSect( of, if_includes, 0, false );
		if ( 1 < m_iface.SourceFileName(A3) )
			PutFmtString( *sect, 0, "#include \"%s.h\"", W3 );
	}

	return InterfaceToSource(of, m_hdr_name);
}

// ---
HRESULT CCodeGen2::GenIncludeDefinition( COutputFile& of, bool prototype )
{

	COutputSect *sect = new COutputSect( of, h_includes, 0, false );

	PutString( *sect, 0, "#include <prague.h>");
	PutFmtString( *sect, 0, "#include <%si_root.h>", m_inc_prefix );
	IncludesToHdr(of);
	m_plugin.Name(A3); _strlwr(W3);
	PutFmtString( *sect, 0, "#include <plugin/p_%s.h>", W3 );

  CollectPropMethods();

	return GenClassDefinition(of, prototype);
}

// ---
HRESULT CCodeGen2::GenClassDefinition( COutputFile& of, bool prototype )
{
	COutputSect& sect = *new COutputSect( of, data_str, 0, false ); // IDS_DATA_STRUCT_STAMP
	DataStructsToSource( sect );

	if( prototype )
	{
		PutString( sect, 0, 0 );
		PutFmtString( sect, 0, "// to get pointer to the structure %s from the hOBJECT obj", m_data_name );
		PutFmtString( sect, 0, "#define CUST_TO_%s(object)  ((%s*)( (((tPTR*)object)[2]) ))", m_data_name, m_data_name );
	}
//	PutFmtString( sect, 0, "  // data structure \"%s\" is described in \"%s\" header file", m_data_name, inc_file_name );

	ObjDescription( of );
	return S_OK;
}

HRESULT CCodeGen2::InternalTableToSource( COutputFile& ofile, CIntMethodInfo &imi )
{
	const char* iface_name = m_iface.Name();

	COutputSect *sect = new COutputSect( ofile, in_meth_tbl, 0, false );

	PutFmtString( *sect, 0, "static iINTERNAL i_%s_vtbl = "BRACE_OS, iface_name );
	for(imi.GoFirst(); imi; )
	{
		if( imi.Name(A2) < 1 )
			strcpy(W2, "unknown_method_name");

		bool f_selected = imi.Selected();

		imi.GoNext();

		int l = 25-sprintf(W1, "(tIntFn%s)", W2);
		if( f_selected )
			sprintf(W0, "%s_%s", iface_name, W2);
		else
			strcpy(W0, "NULL");

		sprintf(W3, "%s %*s%s%c", W1, l>0?l:0 , "", W0, imi ? ',' : ' ' );
		PutString(*sect, 2, W3);
	}
	PutString( *sect, 0, BRACE_C_SEMICOLON );
	return S_OK;
}

HRESULT CCodeGen2::ExternalTableToSource( COutputFile& ofile, CPubMethodInfo &pmi )
{
	const char* iface_name = m_iface.Name();
	sprintf(W0, "%s%s", (char*)m_plugin.Name(), iface_name);

	COutputSect *sect = new COutputSect( ofile, ex_meth_tbl, 0, false );

	bool is_extended = false;
	for(pmi.GoFirst(); pmi; )
	{
		if( pmi.Name(A2) < 1 )
			strcpy(W2, "unknown_method_name");

		if( pmi.IsSeparator() )
		{
			pmi.GoNext();
			continue;
		}

		const char *prefix = iface_name;
		if( pmi.IsExtended() )
			is_extended = true, prefix = W0;

		int l = 25-sprintf(W1, "(fnp%s_%s)", prefix, W2);

		pmi.GoNext();

		sprintf(W3, "%s %*s%s_%s%c", W1, l>0?l:0 , "", iface_name, W2, pmi ? ',' : ' ' );
		PutString(*sect, 2, W3);
	}

	sprintf(W3, "static i%sVtbl e_%s_vtbl = "BRACE_OS, is_extended ? W0 : iface_name, iface_name);
	PutString( *sect, 0, W3, 0 );

	PutString( *sect, 0, BRACE_C_SEMICOLON );
	return S_OK;
}

// ---
HRESULT CCodeGen2::CautionsToSource( COutputFile& ofile )
{
	uint i, cd = m_delayed_props.Count(), ce = m_prop_errors.Count();
	if ( !cd && !ce )
		return S_OK;

	COutputSect* sect = new COutputSect( ofile, caution, 0, false ); // IDS_CAUTION_STAMP

	for(i = 0; i < cd; i++)
		PutFmtString(*sect, 0, "// You have to implement propetry: %s", m_delayed_props[i]);

	for(i = 0; i < ce; i++)
		PutFmtString(*sect, 0, "// %s", m_prop_errors[i]);

	return S_OK;
}

// ---
HRESULT CCodeGen2::InterfaceToSource( COutputFile& ofile, const char* inc_file_name )
{
	if ( !m_plugin )
		return CG_E_IFACE_DEF;

	GlobalPropVarsToSource( ofile );

	CIntMethodInfo imi( m_iface );
	CPubMethodInfo pmi( m_iface );

	for(imi.GoFirst(); imi; imi.GoNext())
		if ( imi.Selected() )
			IFaceMethodToSource( ofile, imi, true );

	uint c = m_methods.Count();
	for(uint i = 0; i < c; i++)
		PropMethodToSource( ofile, m_methods[i] );

	for(pmi.GoFirst(); pmi; pmi.GoNext())
		IFaceMethodToSource( ofile, pmi, false );

	RegisterIFaceBegin( ofile );

	PropTableToSource( ofile );
	InternalTableToSource( ofile, imi );
	ExternalTableToSource( ofile, pmi );

	RegisterCallToSource( ofile );

	CautionsToSource(ofile);
	return S_OK;
}
