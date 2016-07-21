#include "StdAfx.h"
#include "CodeGen.h"
#include "../AVPPveID.h"
#include "../PCGError.h"
#include "../IFaceInfo.h"
#include <stuff/cpointer.h>
#include <Pr_types.h>



// ---
const char* GetSizeString( AVP_dword type );
const char* req_write( AVP_bool req, AVP_bool writable );
tSTRING fs( AVP_dword type );  // format specifications
//tSTRING ts( AVP_dword type );  // type name

const char * CCodeGen::MakePropFuncName( const char *func_name )
{
	::wsprintf( W0, "%s_PROP_%s", m_iface.Name(), func_name );
	return W0;
}

// ---
HRESULT CCodeGen::MakeTablePropFuncName( CAPointer<char> &name, const char *func_name ) {
	const char *make_name = func_name ? MakePropFuncName(func_name) : "NULL";
	uint len = strlen(make_name)+1;
	name = (char*)::memcpy( new char[len], make_name, len );
	return S_OK;
}

// ---
HRESULT CCodeGen::Body_of_the_Recognize_and_ObjectNew( COutputSect& osect, DWORD rtype, const CMethodInfo& mi ) {
	
	DWORD len;
	DWORD tot = mi.TypeOfType();
	if ( HAS_RET_VAL(rtype,tot) ) {
		CObjTypeInfo oti( m_types );
		if ( mi && (1<(len=mi.TypeName(&oti,A2))) && ::memcmp(W2,"tVOID",6) && ::memcmp(W2,"void",5) ) {
			char comp[30];
			const char* comp1;
			const char* comp2;
			
			int   len = ::lstrlen( W2 );
			if ( (len == 6) && (len > (sizeof(comp)-1+6)) )
				comp1 = comp2 = "";
			else if ( len > 6 ) {
				comp1 = comp;
				comp2 = "";
			}
			else { // less them 6
				comp1 = "";
				comp2 = comp;
			}
			len -= 6;
			if ( len < 0 )
				len = -len;
			if ( len > sizeof(comp) )
				len = 0;
			else
				::memset( comp, ' ', len );
			comp[len] = 0;
			
			if ( !mi.Value(A3) ) 
				*((WORD*)(W3)) = MAKEWORD( '0', 0 );
			
			PutFmtString( osect, 2, "tERROR%s error = errNOT_IMPLEMENTED;", comp1 );
			PutFmtString( osect, 2, "%s%s ret_val = %s;", W2, comp2, W3 );
		}
		else
			rtype = tid_VOID;
	}
	else
		PutString( osect, 2, "tERROR error = errNOT_IMPLEMENTED;" );
	
	PutFmtString( osect, 2, "%s_A2( 0, \"Enter %s::%s method for object %%p (iid=%%u) \", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );", g_trace_def, m_iface.Name(), mi.Name() );
	PutString( osect, 0, 0 );
	PutString( osect, 2, "// Place your code here" );
	PutString( osect, 0, 0 );
	if ( HAS_RET_VAL(rtype,tot) ) {
		PutString( osect, 2, "if ( result )" );
		PutString( osect, 4, "*result = ret_val;" );
		PutFmtString( osect, 2, "%s_A3( 0, \"Leave %s::%s method for object %%p (iid=%%u), ret %s = %%%s\", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID), ret_val );", g_trace_def, m_iface.Name(), mi.Name(), W2, fs(rtype) );
	}
	else
		PutFmtString( osect, 2, "%s_A2( 0, \"Leave %s::%s method for object %%p (iid=%%u) \", _that, CALL_SYS_PropertyGetDWord(_that,pgINTERFACE_ID) );", g_trace_def, m_iface.Name(), mi.Name() );
	
	PutString( osect, 2, "return error;" );
	return S_OK;
}



// ---
HRESULT CCodeGen::Body_of_the_ObjectClose( COutputSect& osect, DWORD rtype, const CMethodInfo& method ) {
	
	PutString( osect, 0, 0 );
	PutString( osect, 2, "tERROR error = errOK;" );
	if (m_desc_id == 2)
		PutFmtString( osect, 2, "PR_TRACE_FUNC_FRAME( \"%s::ObjectClose method\" );", m_iface.Name() );
	else
		PutFmtString( osect, 2, "%s_A0( %s, \"Enter %s::ObjectClose method\" );", g_trace_def, m_this, m_iface.Name() );
	PutString( osect, 0, 0 );
	PutString( osect, 2, "// Place your code here" );
	PutString( osect, 0, 0 );
	if (m_desc_id == 2)
		;
	else
		PutFmtString( osect, 2, "%s_A1( %s, \"Leave %s::ObjectClose method, ret %%terr\", error );", g_trace_def, m_this, m_iface.Name() );
	PutString( osect, 2, "return error;" );
	
	return S_OK;
}



// ---
HRESULT CCodeGen::Body_of_the_ERROR_returned_method( COutputSect& osect, DWORD rtype, const CMethodInfo& method, bool internal_func ) {
	return Body_of_the_ordinary_method( osect, rtype, method, internal_func );
}



// ---
HRESULT CCodeGen::Body_of_the_ordinary_method( COutputSect& osect, DWORD rtype, const CMethodInfo& mi, bool internal_func ) {
	
	int len;
	DWORD tot = mi.TypeOfType();
	if ( HAS_RET_VAL(rtype,tot) ) {
		CObjTypeInfo  oti( m_types );
		if ( mi && (1<(len=mi.TypeName(&oti,A2))) && ::memcmp(W2,"tVOID",6) && ::memcmp(W2,"void",5) ) {
			char comp[30];
			const char* comp1;
			const char* comp2;
			int   len = ::lstrlen( W2 );
			
			if ( (len == 6) && (len > (sizeof(comp)-1+6)) )
				comp1 = comp2 = "";
			else if ( len > 6 ) {
				comp1 = comp;
				comp2 = "";
			}
			else { // less them 6
				comp1 = "";
				comp2 = comp;
			}
			len -= 6;
			if ( len < 0 )
				len = -len;
			if ( len > sizeof(comp) )
				len = 0;
			else
				::memset( comp, ' ', len );
			comp[len] = 0;
			
			if ( !mi.Value(A3) ) 
				*((DWORD*)(W3)) = MAKELONG( MAKEWORD('0',0), MAKEWORD(0,0) );
			
			PutFmtString( osect, 2, "tERROR%s error = errNOT_IMPLEMENTED;", comp1 );
			PutFmtString( osect, 2, "%s%s ret_val = %s;", W2, comp2, W3 );
		}
		else
			rtype = tid_VOID;
	}
	else
		PutString( osect, 2, "tERROR error = errNOT_IMPLEMENTED;" );
	
	if (m_desc_id == 2)
		PutFmtString( osect, 2, "PR_TRACE_FUNC_FRAME( \"%s::%s method\" );", m_iface.Name(), mi.Name() );
	else
		PutFmtString( osect, 2, "%s_A0( %s, \"Enter %s::%s method\" );", g_trace_def, m_this, m_iface.Name(), mi.Name() );
	PutString( osect, 0, 0 );
	if ( mi.IsInternal() && m_iface.Static() && (!::lstrcmp("ObjectInit",mi.Name()) || !::lstrcmp("ObjectInitDone",mi.Name())) ) {
		PutString( osect, 2, "if ( 1 == CALL_SYS_PropertyGetDWord(_this,pgOBJECT_USAGE_COUNT) ) "BRACE_OS );
		PutString( osect, 4,   "// Code for initialize the static object" );
		PutString( osect, 2, BRACE_CS );
		PutString( osect, 2, "else "BRACE_OS );
		PutString( osect, 4,   "// Code for connect a new satellite for the static object" );
		PutString( osect, 2, BRACE_CS );
	}
	else
		PutString( osect, 2,   "// Place your code here" );
	PutString( osect, 0, 0 );
	
	if (m_desc_id == 2)
		;
	else if ( HAS_RET_VAL(rtype,tot) ) {
		PutString( osect, 2, "if ( result )" );
		PutString( osect, 4, "*result = ret_val;" );
		PutFmtString( osect, 2, "%s_A2( %s, \"Leave %s::%s method, ret %s = %%%s, %%terr\", ret_val, error );", g_trace_def, m_this, m_iface.Name(), mi.Name(), W2, fs(rtype) );
	}
	else {
		PutFmtString( osect, 2, "%s_A1( %s, \"Leave %s::%s method, ret %%terr\", error );", g_trace_def, m_this, m_iface.Name(), mi.Name() );
	}
	
	PutString( osect, 2, "return error;" ); // IDS_METHOD_RETURN_FMT
	return S_OK;
}



// ---
static void AddDelayedPropName( CPArray<char>& p, const char* n ) {
	bool a = true;
	
	for( uint i=0,c=p.Count(); a && i<c; i++ ) 
		a = !!::lstrcmp( p[i], n );
	
	if ( a ) {
		int l = ::lstrlen( n ) + 1;
		p.Add( (char*)::memcpy(new char[l],n,l) );
	}
}



// ---
static PropMethod* fpm( CPArray<PropMethod>& methods, const char* name ) {
	
	for( uint i=0,c = methods.Count(); i<c; i++ ) {
		PropMethod* pm = methods[i];
		if ( *pm == name ) 
			return pm;
	}
	
	return 0;
	
}



// ---
PropMethod::PropMethod( DWORD mode, bool global, const char* name, const char* prop ) : m_mode( mode ) {
	m_name = ::lstrcpy( new char[::lstrlen(name)+1],name );
	AddPropName( prop, global );
}


// ---
PropMethod::~PropMethod() {
	delete [] m_name;
}


// ---
void PropMethod::AddPropName( const char* prop, bool global ) {
	DWORD len = ::lstrlen( prop ) + 1;
	m_props.Add( (char*)::memcpy(new char[len],prop,len) );
	m_globals.Add( global );
}


// ---
HRESULT CCodeGen::PrivateDecl( COutputFile& ofile )
{
	COutputSect* sect = new COutputSect( ofile, private_defs, 0, false );
	PutFmtString( *sect, 0, "#define %s_PRIVATE_DEFINITION", m_iface.Name() );
	return S_OK;
}

// ---
HRESULT CCodeGen::HeaderToSource( COutputFile& ofile, const char* src_file_name ) {
	
	PrivateDecl(ofile);
	
	COutputSect* sect = new COutputSect( ofile, if_includes, 0, false );
	PutString( *sect, 0, "#include <prague.h>" ); // IDS_INCLUDE_HDR_FMT
	
	if ( 1 < m_iface.SourceFileName(A3) )
		PutFmtString( *sect, 0, "#include \"%s.h\"", W3 );
	
	return S_OK;
}



// ---
HRESULT CCodeGen::ObjDescription( COutputFile& ofile ) {
	char i_al[40];
	char s_al[40];
	char d_al[40];
	
	::memset( i_al, 0, sizeof(i_al) );
	::memset( s_al, 0, sizeof(s_al) );
	::memset( d_al, 0, sizeof(d_al) );
	
	int i_len = ::lstrlen( m_iface.Name() ) + 13; // sizeof( "const iFACENAMEVtbl" )
	int s_len = 19;                             // sizeof( "const iSYSTEMVtbl" )
	int d_len = m_data_name_len + 1;
	int m_len = max( max(s_len,i_len), d_len );
	
	::memset( i_al, ' ', min(sizeof(i_al)-1, m_len-i_len) );
	::memset( s_al, ' ', min(sizeof(s_al)-1, m_len-s_len) );
	::memset( d_al, ' ', min(sizeof(d_al)-1, m_len-d_len) );
	
	COutputSect* sect = new COutputSect( ofile, obj_decl, 0, false ); // IDS_OBJECT_DECL_STAMP
	//PutString( *sect, 0, 0 );
	//PutString( *sect, 0, "#if defined( __cplusplus )" );
	//PutString( *sect, 0, "extern \"C\" "BRACE_OS );
	//PutString( *sect, 0, "#endif" );
	PutString( *sect, 0, 0 );
	PutFmtString( *sect, 0, "typedef struct tag_%s "BRACE_OS, (const char*)m_int_type_name );     // doesn't need anymore -- VTBL -- capital letters( another implementation )
	PutFmtString( *sect, 2, "const i%sVtbl* %svtbl; // pointer to the \"%s\" virtual table" , m_iface.Name(), i_al, m_iface.Name() );
	PutFmtString( *sect, 2, "const iSYSTEMVTBL* %ssys;  // pointer to the \"SYSTEM\" virtual table", s_al );
	if ( m_data_name_len )
		PutFmtString( *sect, 2, "%s* %sdata; // pointer to the \"%s\" data structure", (const char*)m_data_name, d_al, m_iface.Name() );
	PutFmtString( *sect, 0, BRACE_CS" *%s;", (const char*)m_int_type_name );
	//PutString( *sect, 0, 0 );
	//PutString( *sect, 0, "#if defined( __cplusplus )" );
	//PutString( *sect, 0, BRACE_CS );
	//PutString( *sect, 0, "#endif" );
	PutString( *sect, 0, 0 );
	return S_OK;
}



// ---
HRESULT CCodeGen::InterfaceToSource( COutputFile& ofile, const char* inc_file_name ) {
	
	bool root = !::lstrcmp("Root",m_iface.Name());
	
	PutIFaceComment( ofile );
	
	if ( !root ) {
		COutputSect& sect = *new COutputSect( ofile, data_str, 0, false ); // IDS_DATA_STRUCT_STAMP
		if ( m_opt.private_structure() )
			DataStructsToSource( sect );
		else
			PutFmtString( sect, 0, "  // data structure \"%s\" is described in \"%s\" header file", m_data_name, inc_file_name );
	}
	
	ObjDescription( ofile );
	
	CPArray<char> defines;
	CIntMethodInfo imi( m_iface );
	CPubMethodInfo pmi( m_iface );
	
	IFaceMethodTableToSource( ofile, imi );
	VtblToHdr( ofile, defines, true );  // interface vtbl declaration
	IFaceMethodTableToSource( ofile, pmi );
	
	PropertiesToSource( ofile );
	if ( !root )
		RegisterCallToSource( ofile );
	
	uint c = m_methods.Count();
	for( uint i=0; i<c; i++ )
		PropMethodToSource( ofile, m_methods[i] );
	
	imi.GoFirst();
	while( imi ) {
		if ( imi.Selected() )
			IFaceMethodToSource( ofile, imi, true );
		imi.GoNext();
	}
	
	pmi.GoFirst();
	while( pmi ) {
		IFaceMethodToSource( ofile, pmi, false );
		pmi.GoNext();
	}
	
	return S_OK;
}



// ---
HRESULT CCodeGen::FooterToSource( COutputFile& ofile, const char* file_name ) {
	return S_OK;
}



// ---
HRESULT CCodeGen::StructToSource( COutputSect& sect, const CDataStructInfo& str, DWORD shift, tBOOL bMakeMemberPrefix) {
	
	DWORD l;
	
	// save struct name
	if ( str.Name(A2) < 1 ) 
		::lstrcpy( W2, "unknown struct name" );
	
	if ( shift )
		PutFmtString( sect, shift, "struct tag_%s "BRACE_OS, W2 );
	
	// save inner structures
	CDataStructInfo inner( str );
	while( inner ) {
		StructToSource( sect, inner, shift+2 );
		PutString( sect, 0, 0 );
		inner.GoNext();
	}
	
	AVP_dword w[2] = { 0 };
	char a[2][20] = { ' ' };
	
	// save members of struct
	
	CObjTypeInfo oti( m_types );
	CDataMemberInfo member( str );
	while( member ) {
		l = member.TypeName( &oti, 0, 0 );
		if ( w[0] < l )
			w[0] = l;
		
		l = member.Name( 0, 0 );
		if ( l && member.IsArray() ) 
			l += 1 + member.ArrayBound( 0, 0 );
		if ( w[1] < l )
			w[1] = l;
		member.GoNext();  
	}
	
	CAPointer<char> string( new char[0x200] );
	CAPointer<char> a0( new char[0x200] );
	CAPointer<char> a1( new char[0x200] );
	
	member.Init( str.FirstMember() );
	while( member ) {
		DWORD tl = member.TypeName( &oti, A0 );
		DWORD ml = member.Name( A1 );
		if ( tl && ml ) {
			if ( member.IsArray() ) {
				DWORD al = member.ArrayBound( A3 );
				if ( al > 1 ) {
					::wsprintf( W2, "[%s]", W3 );
					ml += 1 + al;
				}
				else {
					::lstrcpy( W2, "[?]" );
					ml += 3;
				}
			}
			else
				W2[0] = 0;
			
			if ( member.ShortComment(A3) < 1 )
				::lstrcpy( W3, "--" );
			
			tBOOL bAddPrefix = (m_desc_id == 2 && bMakeMemberPrefix && memcmp(W1, "m_", 2));
			if (bAddPrefix)
				ml += 2;
			if ( tl < w[0] ) {
				::memset( a0, ' ', w[0]-tl);
				*(a0+(w[0]-tl)) = 0;
			}
			else
				*a0 = 0;
			
			if ( ml < w[1] ) {
				::memset( a1, ' ', w[1]-ml );
				*(a1+(w[1]-ml)) = 0;
			}
			else
				*a1 = 0;
			
			::wsprintf( string, "%s%s %s%s%s;%s // %s", W0, (const char*)a0, (bAddPrefix ? "m_" : ""), W1, W2, (const char*)a1, W3 );
			PutString( sect, shift+2, string );
		}
		member.GoNext();
	}
	
	// save footer of struct
	if( shift ) {
		if ( str.Name(A2) < 1 ) 
			::lstrcpy( W2, "unknown struct name" );
		PutFmtString( sect, shift, BRACE_CS" %s;", W2 );
	}
	
	return S_OK;
}



// ---
HRESULT CCodeGen::DataStructsToSource( COutputSect& sect ) {
	
	// save inner data structure comment
	DWORD shift = m_data_single ? 0 : 2;
	CDataStructInfo ds( m_iface );
	
	PutFmtString( sect, 0, "// Interface ""%s"". Inner data structure", m_iface.Name() ); // IDS_INNER_DATA_STRUCTURE_COMMENT
	
	PutString( sect, 0, 0 );
	//PutString( sect, 0, "#if defined( __cplusplus )" );
	//PutString( sect, 0, "extern \"C\" "BRACE_OS );
	//PutString( sect, 0, "#endif" );
	//PutString( sect, 0, 0 );
	
	PutFmtString( sect, 0, "typedef struct tag_%s "BRACE_OS, (const char*)m_data_name );
	
	DWORD count = 0;
	while( ds.IsOK() ) {
		if ( SUCCEEDED(StructToSource(sect,ds,shift)) ) {
			ds.GoNext();
			count++;
		}
		else
			break;
	}
	
	if ( !m_data_single && !count )
		PutString( sect, 2, "tBYTE dummy;" );
	PutFmtString( sect, 0, BRACE_CS" %s;", (const char*)m_data_name ); // IDS_STRUCT_FTR_FMT
	
	PutString( sect, 0, 0 );
	//PutString( sect, 0, "#if defined( __cplusplus )" );
	//PutString( sect, 0, BRACE_CS );
	//PutString( sect, 0, "#endif" );
	
	return S_OK;
}

// ---
HRESULT CCodeGen::CautionsToSource( COutputFile& ofile ) {
	if ( !m_delayed_props.Count() && !m_prop_errors.Count() )
		return S_OK;
	
	COutputSect* sect = new COutputSect( ofile, caution, 0, false ); // IDS_CAUTION_STAMP
	PutString( *sect, 0, 0 );
	PutString( *sect, 0, "#error Caution !!!" );
	
	if ( m_delayed_props.Count() ) {
    /*
    if ( !::LoadString(_Module.GetModuleInstance(),IDS_HAVE_TO_IMPL_PROP_FMT,A3) )
	::lstrcpy( W3, "You have to implement %s propetries for: " );
    ::wsprintf( W2, W3, "*GET* or *SET*" );
		*/
		PutErrors( *sect, m_delayed_props, "You have to implement *GET* or *SET* propetries for: " );
		PutString( *sect, 0, 0 );
	}
	
	if ( m_prop_errors.Count() )
		PutErrors( *sect, m_prop_errors, "Some error(s)" );
	
	PutString( *sect, 0, 0 );
	PutString( *sect, 0, "#error Caution !!!" );
	PutString( *sect, 0, 0 );
	return S_OK;
}

// ---
HRESULT CCodeGen::PropTableToSource( COutputFile& ofile ) {
	
	COutputSect* sect = new COutputSect( ofile, pr_table, 0, false ); // IDS_PROPERTY_TABLE_STAMP
	//PutFmtString( *sect, 0, "// Interface \"%s\". Property table", m_iface.Name() ); // (const char*)IDS_PROP_TBL_COMMENT_FMT
	PutFmtString( *sect, 0, "m%sPROPERTY_TABLE(%s_PropTable)", m_prop_prefix, m_iface.Name() ); // (const char*)IDS_PROP_TBL_START_FMT
	
	for(CPropInfo pi( m_iface ); pi; pi.GoNext())
		PropToSource( *sect, pi );
	
	bool last_call = LastPropToSource( *sect );
	
	PutFmtString( *sect, 0, "m%sPROPERTY_TABLE_END(%s_PropTable)", m_prop_prefix, m_iface.Name() ); // (const char*)IDS_PROP_TBL_STOP_FMT
	
	m_prop_tbl_count = sect->Count();
	if ( m_prop_tbl_count <= (uint)(last_call ? 3 : 2) ) {
		m_prop_tbl_count = 0;
		delete sect;
		sect = 0;
	}
	return S_OK;
}

// ---
HRESULT CCodeGen::PropertiesToSource( COutputFile& ofile ) {
	
	CollectPropMethods();
	
	COutputSect* sect = new COutputSect( ofile, pr_meth_decl, 0, false ); // IDS_FORWARDED_PROP_METHODS_STAMP
	
	uint c = m_methods.Count();
	for( uint i=0; i<c; i++ )
		PropMethodDefsToSource( *sect, m_methods[i], true );
	
	GlobalPropVarsToSource( ofile );
	
	if ( !m_version )
		VersionDecl( ofile );
	
	PropTableToSource(ofile);
	
	CautionsToSource(ofile);
	return S_OK;
}



// ---
// VE_PID_IFP_PROPNAME						  avpt_str      property name PROP
// VE_PID_IFP_DIGITALID				      avpt_dword    digital ID PROP
// VE_PID_IFP_TYPE									avpt_dword    type PROP
// VE_PID_IFP_TYPENAME						  avpt_str      type name PROP
// VE_PID_IFP_SCOPE								  avpt_dword    scope PROP
// VE_IFP_LOCAL	  0
// VE_IFP_SHARED  1

// VE_PID_IFPLS_REQUIRED						avpt_bool     local scope req PROP
// VE_PID_IFPLS_MEMBER							avpt_str      local scope member PROP
// VE_PID_IFPLS_MODE								avpt_dword    local scope mede PROP
// VE_IFPLS_READ			0
// VE_IFPLS_WRITE			1
// VE_IFPLS_READWRITE	2
// VE_PID_IFPLS_READFUNC						avpt_str      local scope read func PROP
// VE_PID_IFPLS_WRITEFUNC					  avpt_str      local scope write func PROP

// VE_PID_IFPSS_VARIABLENAME				avpt_str      shared scope variable name PROP
// VE_PID_IFPSS_VARIABLEDEFVALUE		avpt_str      shared scope variable defvalue PROP
// VE_PID_IFPSS_POINTER	            avpt_bool     shared scope pointer PROP

// VE_PID_IFP_NATIVE	              avpt_bool     native PROP
HRESULT CCodeGen::PropToSource( COutputSect& sect, const CPropInfo& pi ) {
	CPropInfo::ScopeType  scope;
	CAPointer<char>       name;
	
#define alignment ""
	
	if ( PutSeparator(sect,pi) )
		return S_OK;
	
	DWORD len = pi.Name( A3 );
	
	if ( len > 1 ) {
		AVP_bool native = pi.Predefined() || pi.Native();
		AVP_char* fmt = native ? "pg%s" : "pl%s";
		// W3 = prObjectName ?
		//      ^^
		len = ::wsprintf( W1, fmt, W3 );
		name = (char*)::memcpy( new char[len+1], W1, len+1 );
	}
	else
		return PutFmtString( sect, 0, "#error CodeGen: Property name is not defined for pid 0x%08x", pi.DigitalID() );
	
	scope = pi.Scope();
	if ( scope == CPropInfo::local )
		return LocalPropToSource( sect, name, pi );
	else if ( scope == CPropInfo::shared )
		return StaticPropToSource( sect, name, pi );
	else {
		return PutFmtString( sect, 0, "#error CodeGen: Scope of property \"%s\" is not defined", name );
		//return CG_E_PROP_DEFINITION;
	}
}


bool CCodeGen::LastPropToSource( COutputSect& sect ) {
	if( !m_iface.LastCallProp() )
		return false;
	
	uint lr = m_iface.LastCallPropRFunc(A2);
	uint lw = m_iface.LastCallPropWFunc(A3);
	
	CAPointer<char> last_call_get;
	CAPointer<char> last_call_set;
	
	MakeTablePropFuncName(last_call_get, lr > 1 ? W2 : NULL);
	MakeTablePropFuncName(last_call_set, lw > 1 ? W3 : NULL);
	
	PutFmtString( sect, 2, "m%sLAST_CALL_PROPERTY( %s, %s )", m_prop_prefix, last_call_get, last_call_set );
	return true;
}


// ---
bool CCodeGen::isMemberAhString( const char* member ) {

	if ( !member || !*member )
		return false;

	CDataStructInfo ds( m_iface );
	CObjTypeInfo    oti( m_types );
	CDataMemberInfo struct_member( ds );

	for( ; struct_member; struct_member.GoNext() ) {
		if ( strcmp(member,struct_member.Name()) )
			continue;
		if ( struct_member.IsArray() )
			return false;
		struct_member.TypeName( &oti, A0 );
		if ( !strcmp(W0,"hSTRING") || !strcmp(W0,"cString*") || !strcmp(W0,"cStringObj") )
			return true;
		break;
	}
	return false;
	
}


// ---
HRESULT CCodeGen::LocalPropToSource( COutputSect& sect, const tCHAR* prop_name, const CPropInfo& prop ) {
	CAPointer<char> member;
	CAPointer<char> fn_read;
	CAPointer<char> fn_write;
	CPropInfo::Mode dam = prop.MemberMode(); // data access mode
	CPropInfo::Mode fam = prop.FuncMode();   // function access mode
	AVP_bool        woi = prop.WritableOnInit();  // wratable on init
	AVP_bool        req = prop.Required();
	tUINT           len;
	
	len = prop.MemberName(A3);
	if ( len > 1 ) {
		if ( !::_strnicmp(m_data_name,W3,m_data_name_len-1) ) {
			if (m_desc_id==2 && strncmp(W3+m_data_name_len, "m_", 2) != 0)
			{
				::lstrcpy( W3, "m_" );
				::lstrcpy( W3+2, W3+m_data_name_len );
				len += 2;
			}
			else
			{
				::lstrcpy( W3, W3+m_data_name_len );
			}
			len -= m_data_name_len;
		}
		member = (char*)::memcpy( new char[len], W3, len );
	}
	
	bool rfn = ((fam == CPropInfo::read)  || (fam == CPropInfo::read_write)) && (prop.ReadFuncName(A3)>1);
	bool wfn = ((fam == CPropInfo::write) || (fam == CPropInfo::read_write)) && (prop.WriteFuncName(A2)>1);
	
	MakeTablePropFuncName(fn_read, rfn ? W3 : NULL);
	MakeTablePropFuncName(fn_write, wfn ? W2 : NULL);
	
	switch( dam ) {
    default                        : 
    case VE_IFPLS_MEMBER_READ      : ::lstrcpy( W2, "cPROP_BUFFER_READ" );       break;
    case VE_IFPLS_MEMBER_WRITE     : ::lstrcpy( W2, "cPROP_BUFFER_WRITE" );      break;
    case VE_IFPLS_MEMBER_READWRITE : ::lstrcpy( W2, "cPROP_BUFFER_READ_WRITE" ); break;
	}
	
	len = ::lstrlen( W2 ); // 
	
	if ( woi ) {
		if ( wfn || (dam == CPropInfo::write) || (dam == CPropInfo::read_write) ) {
			::lstrcpy( W2+len, " | cPROP_WRITABLE_ON_INIT" );
			len = ::lstrlen( W2 ); // 
		}
		else {
			PutFmtString( sect, 0, "#error  Property:\"%s\" - flag \"cPROP_WRITABLE_ON_INIT\" is set but property is not writable", prop_name );
			return S_OK;
		}
	}
	
	if ( wfn || (dam == CPropInfo::write) || (dam == CPropInfo::read_write) ) {
		if ( req ) 
			::lstrcpy( W2+len, " | cPROP_REQUIRED" );
	}
	else
		req = false;
	
	AVP_bool fn;
	
	if ( fam == VE_IFPLS_NONE ) 
		fn = false;
	else if ( fam == VE_IFPLS_READ ) 
		fn = rfn;
	else if ( fam == VE_IFPLS_WRITE ) 
		fn = wfn;
	else if ( fam == VE_IFPLS_READWRITE ) 
		fn = rfn || wfn;
	else
		fn = false; //return CG_E_PROP_DEFINITION;
	
	if ( member && fn ) 
		return PutFmtString( sect, 2, "m%sLOCAL_PROPERTY%s( %s, %s, %s, %s, %s, %s )", m_prop_prefix, alignment, prop_name, m_data_name, member, W2, fn_read, fn_write );
	else if ( member ) {
		if ( isMemberAhString(member) )
			::lstrcpy( W2+len, " | cPROP_BUFFER_HSTRING" );
		return PutFmtString( sect, 2, (const char*)IDS_PROP_DEF_LOCAL_BUF_FMT, m_prop_prefix, alignment, prop_name, m_data_name, member, W2 );
	}
	else if ( fn ) 
		return PutFmtString( sect, 2, req_write(req,woi), m_prop_prefix, alignment, prop_name, fn_read, fn_write );
	else { // нет ни member'a ни функций
		AddDelayedPropName( m_delayed_props, prop_name );
		return S_OK;
	}
}




// ---
static tUINT copy_and_calc( tCHAR* dst, const tCHAR* src, tUINT len, tTYPE_ID tid, tBOOL customize ) {
	if ( ((MAKEWORD('L','\"') == *((tWORD*)src)) || ('\"' == src[0])) && ('\"' == src[len-2]) ) {
		tUINT clen = len;
		len -= 2; // quotas;
		if ( src[0] == 'L' ) {
			(--len) *= sizeof(tWCHAR);
			if ( customize && (tid == tid_STRING) ) {
				::memcpy( dst, "(tSTRING)", 9 );
				dst += 9;
			}
		}
		else if ( tid == tid_WSTRING )
			len *= sizeof(tWCHAR);
		::memcpy( dst, src, clen );
	}
	else if ( tid == tid_WSTRING ) {
		len *= sizeof(tWCHAR);
		::wsprintf( dst, "L\"%s\"", src );
	}
	else
		::wsprintf( dst, "\"%s\"", src );
	return len;
}




// ---
HRESULT CCodeGen::StaticPropToSource( COutputSect& sect, const tCHAR* prop_name, const CPropInfo& prop ) {
	
	tUINT    value_len;
	tTYPE_ID type_id;
	
	type_id = (tTYPE_ID)ResolveTypeID( prop );
	if ( !type_id ) 
		return PutFmtString( sect, 2, "GodeGen: property \"%s\" - can't extract property type ", prop_name );
	
	value_len = prop.Value(A1);
	if ( 1 >= value_len )
		return PutFmtString( sect, 2, "GodeGen: property \"%s\" - can't extract default static value", prop_name );
	
	if ( (type_id != tid_STRING) && (type_id != tid_WSTRING) ) {
		prop.TypeName( 0, A0 );
		::wsprintf( W2, "((%s)(%s))", W0, W1 );
	}
	else 
		value_len = ::copy_and_calc( W2, W1, value_len, type_id, cFALSE );
	
	tUINT name_len = prop.VarName( A3 );
	AVP_bool var_name = (name_len > 1) && W3[0];
	
	if ( !var_name && ((type_id == tid_QWORD) || (type_id == tid_LONGLONG) || (type_id == tid_DATETIME) || (type_id == tid_BINARY)) ) {
		var_name = true;
		name_len = prop.Name( A3 );
		if ( name_len < 2 ) 
			::lstrcpy( W3, "unknown_property" );
		name_len = ::wsprintf( W1, "%s_%s", m_iface.Name(), W3 ) + 1;
		::lstrcpy( W3, W1 );
	}
    
	if ( var_name || (type_id == tid_STRING) || (type_id == tid_WSTRING) ) {
		
		char buff[12];
		const char* size;
		const char* val;
		const char* fmt;
		CAPointer<char> size_str;
		
		if ( !var_name ) {
			val = W2;
			fmt = "mSHARED_PROPERTY_PTR%s( %s, %s, %s )";
		}
		else if ( (type_id == tid_STRING) || (type_id == tid_WSTRING) ) {
			val = W3;
			fmt = "mSHARED_PROPERTY_VAR%s( %s, %s, %s )";
		}
		else {
			val = W3;
			fmt = "mSHARED_PROPERTY_PTR%s( %s, %s, %s )";
		}
		
		if ( (type_id == tid_STRING) || (type_id == tid_WSTRING) ) {
			::wsprintf( buff, "%d", value_len ); 
			size = buff;
		}
		else { // var_name != false
			::wsprintf( (size_str=new char[name_len+20]), "sizeof(%s)", W3 );
			size = size_str;
		}
		return PutFmtString( sect, 2, fmt, alignment, prop_name, val, size );
	}
	else if ( (type_id == tid_VERSION) && ((pgINTERFACE_VERSION & pNUMBER_MASK) == prop.DigitalID()) )
		return PutFmtString( sect, 2, "mSHARED_PROPERTY%s( %s, %s )", alignment, prop_name, m_version );
	else
		return PutFmtString( sect, 2, "mSHARED_PROPERTY%s( %s, %s )", alignment, prop_name, W2 );
}



// ---
HRESULT CCodeGen::GlobalPropVarsToSource( COutputFile& ofile ) {
	
	COutputSect &sect = *new COutputSect( ofile, pr_gvars, 0, false ); // IDS_GLOBAL_PROP_VARS_STAMP
	// Put comment of global propety variables table
	PutFmtString( sect, 0, "// Interface \"%s\". Static(shared) property table variables", m_iface.Name() ); // (const char*)IDS_PROP_TBL_GLOBAL_VARS_COMMENT_FMT
	
	for(CPropInfo pi( m_iface ); pi; pi.GoNext()) {
		CPropInfo::ScopeType scope = pi.Scope();
		
		if ( scope == CPropInfo::local )
			;
		
		else if ( (scope == CPropInfo::shared) ) {
			AVP_bool request_var = (pi.VarName(A2)>1) && W2[0];
			tTYPE_ID type_id = (tTYPE_ID)ResolveTypeID( pi );
			
			if ( pi.Name(A3) < 2 ) 
				::lstrcpy( W3, "unknown_property" );
			
			if ( !type_id ) {
				PutFmtString( sect, 2, "property \"%s\" - can't extract property type id ", W3 );
				continue;
			}
			else if ( type_id == tid_VOID ) {
				PutFmtString( sect, 2, "property \"%s\" - type_id VOID is not permitted", W3 );
				continue;
			}
			/*
			else if ( type_id == tid_PTR )
			return PutFmtString( sect, 2, "property \"%s\" - type_id PTR is not permitted", W3 );
			*/
			else if ( type_id == tid_IFACEPTR ) {
				PutFmtString( sect, 2, "property \"%s\" - type_id IFACEPTR is not permitted", W3 );
				continue;
			}
			
			if ( !request_var && ((type_id == tid_QWORD) || (type_id == tid_LONGLONG) || (type_id == tid_DATETIME) || (type_id == tid_BINARY)) ) {
				request_var = true;
				::wsprintf( W2, "%s_%s", m_iface.Name(), W3 );
			}
			
			if ( request_var ) {
				
				AVP_dword len;
				CPropTypeInfo pti(m_types);
				const char* arr = "";
				
				if ( type_id == tid_BINARY ) {
					::lstrcpy( W1, "tBYTE" );
					arr = "[]";
				}
				else if ( 1 >= pi.TypeName(&pti,A1) )
					::lstrcpy( W1, "unknown_property_type" );
				
				if ( type_id == tid_BINARY ) {
					AVP_dword prop_id = 0;
					len = pi.BinValue(pti,A0,0);
					
					if ( !len )
						::lstrcpy( W3, "you have to initialize BINARY value and set properly size in PropTable" );
					
					else if ( len > (sizeof(W0)/8) ) 
						::lstrcpy( W3, "{ too length binary value }" );
					
					else {
						AVP_dword i, c;
						
						::lstrcpy( W3, BRACE_OS );
						
						for( c=1,i=0; i<len && c<(sizeof(W3)-5); i++ ) {
							if ( i > 0 )
								W3[c++] = ',';
							c += ::wsprintf( W3+c, " 0x%02x", (BYTE)W0[i] );
						}
						
						W3[c++] = ' ';
						W3[c++] = BRACE_C;
						W3[c++] = 0;
					}
					
				}
				else if ( 1 >= (len=pi.Value(A3)) )
					::lstrcpy( W3, "place the default value here" );
				else if ( (type_id == tid_STRING) || (type_id == tid_WSTRING) ) {
					::copy_and_calc( W0, W3, len, type_id, cTRUE );
					::lstrcpy( W3, W0 );
				}
				
				if ( (type_id == tid_VERSION) && ((pgINTERFACE_VERSION & pNUMBER_MASK) == pi.DigitalID()) ) {
					len = ::lstrlen(W2) + 1;
					m_version = (char*)::memcpy( new char[len], W2, len );
				}
				
				::wsprintf( W0, "const %s %s%s = %s; // must be READ_ONLY at runtime", W1, W2, arr, W3 );
				int i = sect.find_by( 0, W2, cmp_by_substr );
				if ( -1 == i )
					PutString( sect, 0, W0 );
				else if ( cmp_ignore_spaces(sect[i],W0) )
					;
				else
					PutFmtString( sect, 0, "// %s", W0 );
			}
		}
		else
			PutFmtString( sect, 0, "#error Scope of property \"%s\" is not defined", pi.Name() );
	}
	
	
	return S_OK;
}


// ---
HRESULT CCodeGen::CollectPropMethods() {
	
	CPropInfo pi(m_iface);
	for( ; pi; pi.GoNext()) {
		DWORD len = pi.Name(A1);
		
		if ( 1 > len ) 
			len = ::lstrlen( ::lstrcpy(W1,"unknown prop name") ) + 1;
		
		CPropInfo::ScopeType scope = pi.Scope();
		
		if ( scope == CPropInfo::local ) {
			
			DWORD lr, lw;
			CPropInfo::Mode mode = pi.FuncMode();
			
			if ( (mode == CPropInfo::read) || (mode == CPropInfo::read_write) ) {
				lr = pi.ReadFuncName(A2);
				if ( 1 > lr ) 
					lr = ::lstrlen( ::lstrcpy(W2,"read prop method name is not specified") ) + 1;
			}
			else
				lr = 0;
			
			if ( (mode == CPropInfo::write) || (mode == CPropInfo::read_write) ) {
				lw = pi.WriteFuncName(A3);
				if ( 1 > lw )
					lw = ::lstrlen( ::lstrcpy(W3,"write prop method name is not specified") ) + 1;
			}
			else
				lw = 0;
			
			if ( lr || lw ) {
				PropMethod* pm;
				
				bool native = pi.Predefined() || pi.Native();
				
				if ( lr ) {
					pm = fpm( m_methods, W2 );
					if ( pm ) {
						if ( pm->Mode() != CPropInfo::read ) {              /*GET_*/
							len = 1 + ::wsprintf( W1, "Property function \"%s\" is used as *GET* and *SET* simultaneously", MakePropFuncName(W2) );
							m_prop_errors.Add( (char*)::memcpy(new char[len],W1,len) );
							continue;
						}
						else
							pm->AddPropName( W1, native );
					}
					else 
						m_methods.Add( new PropMethod(VE_IFPLS_READ,native,W2,W1) );
				}
				
				if ( lw ) {
					pm = fpm( m_methods, W3 );
					if ( pm ) {
						if ( pm->Mode() != CPropInfo::write ) {             /*SET_*/
							len = 1 + ::wsprintf( W1, "Property function \"%s\" is used as *GET* and *SET* simultaneously", MakePropFuncName(W2) );
							m_prop_errors.Add( (char*)::memcpy(new char[len],W1,len) );
							continue;
						}
						else
							pm->AddPropName( W1, native );
					}
					else 
						m_methods.Add( new PropMethod(VE_IFPLS_WRITE,native,W3,W1) );
				}
			}
		}
		else if ( scope != CPropInfo::shared ) {
			len = 1 + ::wsprintf( W0, "Property \"%s\" has error(s) in definition", W1 );
			m_prop_errors.Add( (char*)::memcpy(new char[len],W1,len) );
			continue;
		}
	}
	return CollectLastPropMethods( pi );
}

// ---
HRESULT CCodeGen::CollectLastPropMethods( const CPropInfo& pi ) {
	
	if( !m_iface.LastCallProp() )
		return S_OK;
	
	PropMethod* pm;
	uint len;
	
	uint lr = m_iface.LastCallPropRFunc(A2);
	if ( lr > 1 ) {
		pm = fpm( m_methods, W2 );
		if ( pm ) {
			if ( pm->Mode() != VE_IFPLS_READ ) {
				len = 1 + ::wsprintf( W1, "Property function \"%s\" is used as *GET* and *SET* simultaneously", MakePropFuncName(W2) );
				m_prop_errors.Add( (char*)::memcpy(new char[len],W1,len) );
			}
			else
				pm->AddPropName( PLC, true );
		}
		else 
			m_methods.Add( new PropMethod(VE_IFPLS_READ,true,W2,PLC) );
	}
	
	uint lw = m_iface.LastCallPropWFunc(A2);
	if ( lw > 1 ) {
		pm = fpm( m_methods, W2 );
		if ( pm ) {
			if ( pm->Mode() != VE_IFPLS_WRITE ) {
				len = 1 + ::wsprintf( W1, "Property function \"%s\" is used as *GET* and *SET* simultaneously", MakePropFuncName(W2) );
				m_prop_errors.Add( (char*)::memcpy(new char[len],W1,len) );
			}
			else
				pm->AddPropName( PLC, true );
		}
		else 
			m_methods.Add( new PropMethod(VE_IFPLS_WRITE,true,W2,PLC) );
	}
	
	if( lw <= 1 && lw <= 1 ) {
		len = 1 + ::lstrlen( lstrcpy(W1,"Last call property is checked, but neither *GET* nor *SET* function names are not defined") );
		m_prop_errors.Add( (char*)::memcpy(new char[len],W1,len) );
	}
	return S_OK;
}

HRESULT CCodeGen::PropMethodDefsToSource( COutputSect &sect, PropMethod* method, bool forward_decl ) {
	
	const char* func_name_fmt;
	HRESULT hr;
	
	if ( method->Mode() == VE_IFPLS_READ )
		func_name_fmt = "tERROR pr_call %s( %s _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )"; //IDS_PROP_GET_METHOD_FMT;
	else
		func_name_fmt = "tERROR pr_call %s( %s _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )"; //IDS_PROP_SET_METHOD_FMT;
	
	AVP_dword len = ::wsprintf( W1, func_name_fmt, MakePropFuncName(method->Name()), (const char*)m_int_type_name, (const char*)m_data_name );
	
	if ( forward_decl ) {
		*((WORD*)(W1+len)) = MAKEWORD( ';', 0 );
		hr = PutString( sect, 0, W1 );
	}
	else {
		//::lstrcpy( W1+len, " {\r\n  return 0;\r\n}" );
		*((DWORD*)(W1+len)) = MAKELONG( MAKEWORD(SPACE,BRACE_O), MAKEWORD(0,0) );
		hr = PutString( sect, 0, W1 );
	}
	return hr;
}


// ---
HRESULT CCodeGen::PropMethodToSource( COutputFile& ofile, PropMethod* method ) {
	
	const char* prop_name_fmt;
	char* mode;
	
	if ( method->Mode() == VE_IFPLS_READ ) {
		/*GET_*/
		prop_name_fmt = "// Interface \"%s\". Method \"Get\" for property(s):";           //IDS_PROP_GET_METHOD_COMMENT_FMT;
		mode = "GET";
	}
	else {                     /*SET_*/
		prop_name_fmt = "// Interface \"%s\". Method \"Set\" for property(s):";           //IDS_PROP_SET_METHOD_COMMENT_FMT;
		mode = "SET";
	}
	
	uint i;
	uint pc = method->Props().Count();
	
	// comment func name
	COutputSect* sect = new COutputSect( ofile, pr_meth_impl, method->Name(), false ); // IDS_PROP_METHOD_HEADER_STAMP
	PutFmtString( *sect, 0, prop_name_fmt, m_iface.Name() );
	// get func for the sake of props
	for( i=0; i<pc; i++ ) 
		PutFmtString( *sect, 0, (const char*)IDS_PROP_GET_METHOD_PROP_FMT, method->Props()[i] );
	
	PropMethodDefsToSource(*sect, method, false);
	
	if ( pc ) {
		const char* p_name;
		if ( pc == 1 ) {
			p_name = method->Props()[0];
			if ( !::lstrcmp(p_name,PLC) ) {
				PutString( *sect, 2, "// for the sake of \"*LAST CALL*\" property" );
				PutString( *sect, 2, 0 );
				PutString( *sect, 2, "tERROR error = errOK;" );
				PutFmtString( *sect, 2, "%s_A0( %s, \"Enter *%s* method for *LAST CALL* property \" );", g_trace_def, m_this, mode );
				PutString( *sect, 2, 0 );
				PutString( *sect, 4, "*out_size = 0;" );
				PutString( *sect, 2, 0 );
				PutFmtString( *sect, 2, "%s_A2( %s, \"Leave *%s* method for property *LAST CALL*, ret tDWORD = %%u, %%terr\", *out_size, error );", g_trace_def, m_this, mode );
				PutString( *sect, 2, "return error;" );
			}
			else {
				::wsprintf( W2, method->Globs()[0] ? "pg%s" : "pl%s", p_name );
				
				PutString( *sect, 2, "tERROR error = errOK;" );
				PutFmtString( *sect, 2, "%s_A0( %s, \"Enter *%s* method %s for property %s\" );", g_trace_def, m_this, mode, method->Name(), W2 );
				PutString( *sect, 2, 0 );
				PutString( *sect, 2, "*out_size = 0;" );
				PutString( *sect, 0, 0 );
				PutFmtString( *sect, 2, "%s_A2( %s, \"Leave *%s* method %s for property %s, ret tDWORD = %%u(size), %%terr\", *out_size, error );", g_trace_def, m_this, mode, method->Name(), W2 );
				PutString( *sect, 2, "return error;" );
			}
		}
		else {
			AVP_bool def = true;
			PutString( *sect, 2, "tERROR error = errOK;" );
			PutFmtString( *sect, 2, "%s_A0( %s, \"Enter *%s* multyproperty method %s\" );", g_trace_def, m_this, mode, method->Name() );
			PutString( *sect, 0, 0 );
			PutString( *sect, 2, "switch ( prop ) "BRACE_OS );
			PutString( *sect, 0, 0 );
			for( i=0; i<pc; i++ ) {
				p_name = method->Props()[i];
				if ( !::lstrcmp(p_name,PLC) ) {
					PutString( *sect, 4, "default: // for the sake of \"Last Call\" property" );
					PutString( *sect, 6, "*out_size = 0;" );
					PutString( *sect, 6, "break;" );
					PutString( *sect, 0, 0 );
					def = false;
				}
				else {
					::wsprintf( W2, method->Globs()[i] ? "pg%s" : "pl%s", p_name );
					PutFmtString( *sect, 4, "case %s:", W2 );
					PutString( *sect, 6, "*out_size = 0;" );
					PutString( *sect, 6, "break;" );
					PutString( *sect, 0, 0 );
				}
			}
			if ( def ) {
				PutString( *sect, 4, "default:" );
				PutString( *sect, 6, "*out_size = 0;" );
				PutString( *sect, 6, "break;" );
			}
			PutString( *sect, 2, BRACE_CS );
			PutString( *sect, 0, 0 );
			//PutString( *sect, 2, "*out_size = 0;" );
			//PutString( *sect, 0, 0 );
			PutFmtString( *sect, 2, "%s_A2( %s, \"Leave *%s* multyproperty method %s, ret tDWORD = %%u(size), %%terr\", *out_size, error );", g_trace_def, m_this, mode, method->Name() );
			PutString( *sect, 2, "return error;" );
		}
	}
	else {
		PutString( *sect, 2, "tERROR error = errOK;" );
		PutFmtString( *sect, 2, "%s_A0( %s, \"Enter *%s* property method %s\" );", g_trace_def, m_this, mode, method->Name() );
		PutString( *sect, 0, 0 );
		PutString( *sect, 2, "*out_size = 0;" );
		PutString( *sect, 0, 0 );
		PutFmtString( *sect, 2, "%s_A2( %s, \"Leave *%s* property method %s, ret tDWORD = %%u(size), %%terr\", size, error );", g_trace_def, m_this, mode, method->Name() );
		PutString( *sect, 2, "return size;" );
	}
	PutString( *sect, 0, BRACE_CS );
	
	return S_OK;
}




// --- save intrface methods to include file
HRESULT CCodeGen::IFaceMethodToSource( COutputFile& ofile, const CMethodInfo& mi, bool internal_func ) {
	
	if ( mi.IsSeparator() ) 
		return S_OK;
	
	ItemID sect_id;
	
	if ( mi.IsPublic() )
		sect_id = ex_meth_impl;
	
	else if ( mi.IsInternal() )
		sect_id = in_meth_impl;
	
	else {
		COutputSect& sect = *new COutputSect( ofile, caution, 0, false );
		PutString( sect, 0, "Unknown member method type" );
		return S_OK;
	}
	
	COutputSect& sect = *new COutputSect( ofile, sect_id, (char*)mi.Name(), false );
	//PutFmtString( sect, 0, "// --- Interface \"%s\". Method \"%s\"", m_iface.Name(), mi.Name() );
	if ( m_opt.ext_comments() )
		PutMethodExComment(sect, mi);
	
	IFaceMethodDefsToSource( sect, mi, false );
	
	AVP_dword type_id = mi.TypeID();
	
	if ( internal_func && (!::lstrcmp(mi.Name(),"Recognize") || !::lstrcmp(mi.Name(),"ObjectNew")) ) 
		Body_of_the_Recognize_and_ObjectNew( sect, type_id, mi );
	else if ( internal_func && !::lstrcmp(mi.Name(),"ObjectClose") ) 
		Body_of_the_ObjectClose( sect, type_id, mi );
	else if ( type_id == tid_ERROR ) 
		Body_of_the_ERROR_returned_method( sect, type_id, mi, internal_func );
	else 
		Body_of_the_ordinary_method( sect, type_id, mi, internal_func );
	PutString( sect, 0, BRACE_CS );
	
	if( !internal_func )
	{
		bool is_extended = mi.IsExtended();
	}
	
	return S_OK;
}



// ---
AVP_dword CCodeGen::MakeIFaceMethodPrefix( char* buffer, const CMethodInfo& mi, bool forward_decl ) {
	
	AVP_dword s=0, l=0;
	
	::memcpy( buffer, "tERROR pr_call ", s=15 );
	
	if ( 1 > mi.Name(A3) )
		::lstrcpy( W3, "unknown method name" );
	
	s += ::wsprintf( buffer+s, "%s_%s", m_iface.Name(), W3 );
	
	if ( (W3[0] == 'R' || W3[6] == 'N') && (!::lstrcmp(W3,"Recognize") || !::lstrcmp(W3,"ObjectNew")) ) {
		::memcpy( buffer+s, "( hOBJECT _that", 15 ); s += 15;
	}
	else {
		l = ::lstrlen( m_int_type_name );
		*((WORD*)(buffer+s)) = MAKEWORD( BRACKET_O, SPACE );  s += 2;
		::memcpy( buffer+s, (const char*)m_int_type_name, l );     s += l;
		::memcpy( buffer+s, " _this", 6 );                    s += 6;
	}
	return s;
}

AVP_dword CCodeGen::MakeIFaceMethodPosix( char* buffer, const CMethodInfo& mi, bool forward_decl ) {
	AVP_dword s=0, c=1, l=0;;
	CMethodParamInfo mpi( mi );
	while( mpi ) {
		
		if( *(buffer+s-2) != BRACKET_O )
			*((WORD*)(buffer+s)) = MAKEWORD( COMMA, SPACE ),  s += 2;
		
		if ( 1 > mpi.Name(A3) )
			::wsprintf( W3, "param%d", c );
		
		if ( !::lstrcmp(W3,"...") ) {
			::memcpy( buffer+s, "...", 3 );
			s += 3;
			break;
		}
		
		if ( 1 >= (l=GetParamType(mpi,buffer+s,REST(s))) ) {
			::memcpy( buffer+s, "???", 3 );
			l = 4;
		}
		
		s += l-1;
		
		buffer[s++] = SPACE;
		
		l = ::lstrlen( W3 );
		if (l < 2 || ::memcmp(W3, "p_", 2))
		{	
			::memcpy( buffer+s, "p_", 2 );
			s += 2;
		}
		
		::memcpy( buffer+s, W3, l );
		s += l;
		
		mpi.GoNext();
		c++;
	}
	
	if( *(buffer+s-2) != BRACKET_O )
		*((WORD*)(buffer+s)) = MAKEWORD( SPACE, BRACKET_C ), s += 2;
	else
		*(buffer+s-1) = BRACKET_C;
	
	*(buffer+s) = 0;
	
	if( forward_decl )
		::lstrcat( buffer, ";" );
	else
		::lstrcat( buffer, " "BRACE_OS );
	return s;
}

// ---
HRESULT CCodeGen::IFaceMethodDefsToSource( COutputSect& sect, const CMethodInfo& mi, bool forward_decl ) {
	
	AVP_dword s=0, l=0;
	AVP_dword type_id = mi.TypeID();
	AVP_dword tot     = mi.TypeOfType();
	
	CAPointer<char> mb( new char[BUFFS] );
	if ( !mb )
		return E_OUTOFMEMORY;
	
	char *buffer = mb;
	
	s += MakeIFaceMethodPrefix(buffer, mi, forward_decl);
	
	if ( HAS_RET_VAL(type_id,tot) ) {
		CObjTypeInfo oti( m_types );
		l = mi.TypeName( &oti, A3 );
		if ( 1 >= l ) 
			::memcpy( W3, UNKNOWN_RESULT_TYPE, l=::lstrlen(UNKNOWN_RESULT_TYPE)+1 );
		
		if ( ::lstrcmp(W3,"void") && ::lstrcmp(W3,"tVOID") ) {
			if( *(buffer+s-2) != BRACKET_O )
				*((WORD*)(buffer+s)) = MAKEWORD( COMMA, SPACE ),  s += 2;
			::memcpy( buffer+s, W3, l );                      s += l-1;
			::memcpy( buffer+s, "* result", 8 );              s += 8;
		}
	}
	
	s += MakeIFaceMethodPosix(buffer+s, mi, forward_decl);
	
	PutString( sect, 0, buffer );
	return S_OK;
}




// ---
HRESULT CCodeGen::IFaceMethodTableToSource( COutputFile& ofile, CMethodInfo& mi ) {
	
	uint          i, c;
	AVP_dword     type;
	COutputSect*  sect;
	ItemID        sect_id;
	ItemID        sect_id2;
	const char*   table_name;
	const char*   table_start;
	
	if ( mi.IsPublic() ) {
		type = 0;
		table_name = "e_%s_vtbl";
		table_start = "static i%sVtbl %s = "BRACE_OS;
		sect_id = ex_meth_prt;
		sect_id2 = ex_meth_tbl;
	}
	else if ( mi.IsInternal() ) {
		type = 1;
		table_name = "i_%s_vtbl";
		table_start = "static %s %s = "BRACE_OS;
		sect_id = in_meth_prt; 
		sect_id2 = in_meth_tbl;
	}
	else {
		sect = new COutputSect( ofile, caution, 0, false );
		PutString( *sect, 0, "Unknown method table type" );
		return S_OK;
	}
	
	c = 0;
	
	sect = new COutputSect( ofile, sect_id, 0, false );
	mi.GoFirst();
	while( mi ) {
		if ( ((type==0) && !PutSeparator(*sect,mi) ) || ((type!=0) && mi.Selected()) ) {
			c++;
			IFaceMethodDefsToSource(*sect,mi,true);
		}
		mi.GoNext();
	}
	
	
	sect = new COutputSect( ofile, sect_id2, 0, false );
	if ( c ) {
		const char* name;
		if ( type == 0 )
			name = m_iface.Name();
		else
			name = "iINTERNAL";
		
		::wsprintf( W3, table_name, m_iface.Name() );
		PutFmtString( *sect, 0, table_start, name, W3 );
		
		DWORD a = 0;
		CPArray<char> names;
		CPArray<char> types;
		
		mi.GoFirst();
		while( mi ) {
			DWORD l, t;
			if ( !mi.IsSeparator() ) {
				
				t = mi.Name( A3 );
				if ( 1 < t ) {
					if ( type == 0 ) {
						t = l = ::wsprintf( W1, "%s_%s", m_iface.Name(), W3 ) + 1;
						names.Add( (char*)::memcpy(new char[l],W1,l) );
						//types.Add( (char*)::memcpy(new char[l],W1,l) );
					}
					else if ( mi.Selected() ) {
						l = ::wsprintf( W1, "%s_%s", m_iface.Name(), W3 ) + 1;
						names.Add( (char*)::memcpy(new char[l],W1,l) );
						types.Add( (char*)::memcpy(new char[t],W3,t) );
					}
					else {
						names.Add( (char*)::memcpy(new char[5],"NULL",5) );
						types.Add( (char*)::memcpy(new char[t],W3,t) );
					}
					
				}
				else {
					names.Add( (char*)::memcpy(new char[20],"unknown_method_name",l=20) );
					types.Add( (char*)::memcpy(new char[20],"unknown_method_type",t=20) );
				}
				
				if ( a < t )
					a = t;
			}
			mi.GoNext();
		}
		
		c = names.Count();
		if ( type == 0 ) {
			for( i=0; i<c; i++ ) 
				PutFmtString( *sect, 2, (( i < c-1 ) ? "%s," : "%s"), (const char*)names[i] );
		}
		else {
			char* al = new char[a+1];
			::memset( al, ' ', a );
			al[a] = 0;
			for( i=0; i<c; i++ ) {
				DWORD last = i<c-1 ? ',' : ' ';
				const char* t = types[i];
				uint cl = ::lstrlen( t );
				al[ a-cl ] = 0;
				PutFmtString( *sect, 2, "(tIntFn%s) %s %s%c", t, al, (const char*)names[i], last );
				al[ a-cl ] = ' ';
			}
			delete [] al;
		}
		
		/*
		mi.GoFirst();
		while( mi ) {
		if ( !mi.IsSeparator() ) {
        if ( (type == 0) || mi.Selected() ) {
		if ( 1 < mi.Name(A3) ) {
		if ( type == 0 ) 
		::wsprintf( W0, "(fnp%s_%s)%s_%s,", m_iface.Name(), W3, m_iface.Name(), W3 );
		else 
		::wsprintf( W0, "(tIntFn%s)%s_%s,", W3, m_iface.Name(), W3 );
		PutString( *sect, 2, W0 );
		}
		else
		PutString( *sect, 2, "Method name not found" );
        }
        else if ( type == 1 ) 
		PutString( *sect, 2, "NULL," );
		}
		mi.GoNext();
		}
		*/
		
		PutString( *sect, 0, BRACE_C_SEMICOLON );
		//PutFmtString( *sect, 0, BRACE_C_SEMICOLON" // \"%s\" %s variable", m_iface.Name(), sect->name() );
	}
	return S_OK;
}



// ---
HRESULT CCodeGen::VersionDecl( COutputFile& file ) {
	
	if ( m_version ) {
		new COutputSect( file, version_id, 0, false );
		return S_OK;
	}
	
	int len = ::wsprintf( W3, "%s_VERSION", m_iface.Name() ) + 1;
	m_version = (char*)::memcpy( new char[len], W3, len );
	
	char* val = 0;
	CPropInfo pi( m_iface );
	while( pi ) {
		if ( pi.IsSeparator() )
			;
		else if ( (pgINTERFACE_VERSION & pNUMBER_MASK) != pi.DigitalID()  )
			;
		else if ( tid_VERSION != pi.TypeID() )
			;
		else {
			if ( CPropInfo::shared != pi.Scope() )
				;
			else if ( 1 < pi.Value(A3) ) 
				val = W3;
			break;
		}
		pi.GoNext();
	}
	
	if ( !val )
		::wsprintf( val=W3, "%d", m_plugin.Version() );
	
	COutputSect& sect = *new COutputSect( file, version_id, 0, false );
	PutFmtString( sect, 0, "#define %s ((tVERSION)%s)", (const char*)m_version, val );
	
	return S_OK;
}


HRESULT CCodeGen::RegisterCallGetDataSize( char *buffer ) {
    ::wsprintf( buffer, "sizeof(%s)", (const char*)m_data_name );
	return S_OK;
}

// ---
HRESULT CCodeGen::RegisterCallImplToSource( COutputSect& sect ) {
	CAPointer<char> iface_iid;
	
	PutString( sect, 0, 0 );
	if (m_desc_id == 2)
		PutFmtString( sect, 2, "PR_TRACE_FUNC_FRAME_( *root, \"%s::Register method\", &error );", m_iface.Name() );
	else
		PutFmtString( sect, 2, "PR_TRACE_A0( root, \"Enter %s::Register method\" );", m_iface.Name() );
	PutString( sect, 0, 0 );
	PutString( sect, 2, "error = CALL_Root_RegisterIFace(" );
	
	if ( FAILED(PutRegParam(sect,"root","root object")) ) 
		PutString( sect, 4, "Cannot put mnemonic interface iid" );
	
	// mnemonic interface iid
	int len = m_iface.MnemonicID(A3);
	if ( 1 >= len ) {
		::lstrcpy( W3, "Mnemonic_id_not_found" );
		len = 22;
	}
	iface_iid = (char*)::memcpy( new char[len], W3, len );
	
	if ( FAILED(PutRegParam(sect,W3,"interface identifier")) ) 
		PutString( sect, 4, "Cannot put mnemonic interface iid" );
	
	// plugin id
	if ( !m_plugin.MnemonicID(A3) ) 
		::lstrcpy( W3, "Plugin_id_not_found" );
	if ( FAILED(PutRegParam(sect,W3,"plugin identifier")) ) 
		PutString( sect, 4, "Cannot put plugin id" );
	
	// subtype
	if ( !m_iface.SubTypeStr(A3) ) 
		::lstrcpy( W3, "0x00000000" );
	if ( FAILED(PutRegParam(sect,W3,"subtype identifier")) ) 
		PutString( sect, 4, "Cannot put subtype id" );
	
	// version
	if ( FAILED(PutRegParam(sect,m_version,"interface version")) ) 
		PutString( sect, 4, "Cannot put version id" );
	
	// vendor
	if ( !m_plugin.VendorMnemonicID(A3) ) 
		::lstrcpy( W3, "Vendor_id_not_found" );
	if ( FAILED(PutRegParam(sect,W3,"interface developer")) ) 
		PutString( sect, 4, "Cannot put vendor id" );
	
	AVP_bool we_do = HasMethodTable( VE_PID_IF_SYSTEMMETHODS );
	
	// internal vtbl
	if ( we_do )
		::wsprintf( W3, "&i_%s_vtbl", m_iface.Name() );
	else 
		::memcpy( W3, "NULL", 5 );
	
	// internal vtbl
	if ( FAILED(PutRegParam(sect,W3,"internal(kernel called) function table")) ) 
		PutString( sect, 4, "Cannot put internal vtbl pointer" );
	
	// internal vtbl size
	if ( we_do ) 
		::wsprintf( W3, "(sizeof(i_%s_vtbl)/sizeof(tPTR))", m_iface.Name() );
	else
		*(DWORD*)W3 = MAKELONG( MAKEWORD('0',0), MAKEWORD(0,0) );
	if ( FAILED(PutRegParam(sect,W3,"internal function table size")) ) 
		PutString( sect, 4, "Cannot put internal vtbl size" );
	
	we_do = HasMethodTable( VE_PID_IF_METHODS );
	
	// external func table
	if ( we_do ) 
		::wsprintf( W3, "&e_%s_vtbl", m_iface.Name() );
	else
		::lstrcpy( W3, "NULL" );
	if ( FAILED(PutRegParam(sect,W3,"external function table")) ) 
		PutString( sect, 4, "Cannot put external vtbl pointer" );
	
	// external func table size
	if ( we_do )
		::wsprintf( W3, "(sizeof(e_%s_vtbl)/sizeof(tPTR))", m_iface.Name() );
	else
		::lstrcpy( W3, "0" );
	if ( FAILED(PutRegParam(sect,W3,"external function table size")) ) 
		PutString( sect, 4, "Cannot put external vtbl size" );
	
	if ( m_prop_tbl_count ) {
		::wsprintf( W2, "%s_PropTable", m_iface.Name() );
		// property table
		if ( FAILED(PutRegParam(sect,W2,"property table")) ) 
			PutString( sect, 4, "Cannot put property table" );
		::wsprintf( W3, "mPROPERTY_TABLE_SIZE(%s)", (const char*)W2 );
		// property table size
		if ( FAILED(PutRegParam(sect,W3,"property table size")) ) 
			PutString( sect, 4, "Cannot put property table size" );
	}
	else {
		// property table
		if ( FAILED(PutRegParam(sect,"NULL","property table")) ) 
			PutString( sect, 4, "Cannot put NULL pointer for property table" );
		// property table size
		if ( FAILED(PutRegParam(sect,"0","property table size")) ) 
			PutString( sect, 4, "Cannot put 0 for property table size" );
	}
	
	// memory size
	if ( m_data_name_len )
		RegisterCallGetDataSize(W3);
	else
		*((WORD*)W3) = MAKEWORD( '0', 0 );
	// memory size
	if ( FAILED(PutRegParam(sect,W3,"memory size")) ) 
		PutString( sect, 4, "Cannot put necessary memory size" );
	
	// interface flags
	AVP_bool static_iface = m_iface.Static();
	::lstrcpy( W3, static_iface ? "IFACE_STATIC" : "0" );
	
	AVP_bool system = m_iface.System();
	if ( system ) {
		if ( static_iface ) 
			::lstrcat( W3, " | IFACE_SYSTEM" );
		else
			::lstrcpy( W3, "IFACE_SYSTEM" );
	}
	
	AVP_bool transfer_props = m_iface.TransferPropUp();
	if ( transfer_props ) {
		if ( static_iface || system ) 
			::lstrcat( W3, " | IFACE_PROP_TRANSFER" );
		else
			::lstrcpy( W3, "IFACE_PROP_TRANSFER" );
	}
	
	AVP_bool unswappable = m_iface.UnSwapable();
	if ( unswappable ) {
		if ( static_iface || system || transfer_props ) 
			::lstrcat( W3, " | IFACE_UNSWAPPABLE" );
		else
			::lstrcpy( W3, "IFACE_UNSWAPPABLE" );
	}
	
	DWORD sync = m_iface.Synchronized();
	if ( sync ) {
		if ( static_iface || system || transfer_props || unswappable ) 
			::lstrcat( W3, " | IFACE_PROTECTED_BY_" );
		else
			::lstrcpy( W3, "IFACE_PROTECTED_BY_" );
		if ( sync == VE_IFF_PROTECTED_BY_MUTEX )
			::lstrcat( W3, "MUTEX" );
		else
			::lstrcat( W3, "CS" );
	}
	
	if ( FAILED(PutRegParam(sect,W3,"interface flags",false)) ) 
		PutString( sect, 4, "Cannot put interface flags" );
	
	PutString( sect, 2, BRACKET_C_SEMICOLON );
	PutString( sect, 0, 0 );
	PutString( sect, 2, "#ifdef _PRAGUE_TRACE_" );
	PutString( sect, 4, "if ( PR_FAIL(error) )" );
	PutFmtString( sect, 6, "PR_TRACE( (root,prtDANGER,\"%s(%s) registered [%%terr]\",error) );", m_iface.Name(), (const char*)iface_iid );
	PutString( sect, 2, "#endif // _PRAGUE_TRACE_" );
	PutString( sect, 0, 0 );
	if (m_desc_id == 2) // CPP
		;
	else
		PutFmtString( sect, 2, "PR_TRACE_A1( root, \"Leave %s::Register method, ret %%terr\", error );", m_iface.Name() );
	
	return S_OK;
}

// ---
HRESULT CCodeGen::RegisterCallToSource( COutputFile& ofile ) {
	
	COutputSect& sect = *new COutputSect( ofile, reg_call, 0, false ); // IDS_REGISTER_CALL_STAMP
	PutFmtString( sect, 0, (const char*)IDS_REGISTER_FUNC_COMMENT_FMT, m_iface.Name() );
	
	if ( !m_plugin )
		return CG_E_IFACE_DEF;
	
	::lstrcpy( W3, "tERROR pr_call %s_Register( hROOT root )" );
	
	int len = ::wsprintf( W2, W3, m_iface.Name() );
	*((DWORD*)(W2+len)) = MAKELONG( MAKEWORD(' ',BRACE_O), MAKEWORD(0,0) );
	PutString( sect, 0, W2 );
	
	PutString( sect, 2, "tERROR error;" );
	RegisterCallImplToSource(sect);
	PutString( sect, 2, "return error;" );
	
	PutString( sect, 0, BRACE_CS );
	return S_OK; // hr;
}



// ---
HRESULT CCodeGen::PutRegParam( COutputSect& sect, const char* def, const char* comment, bool comma ) {
	AVP_dword len;
	if ( def != 0 ) {
		if ( HIWORD((AVP_dword)def) ) {
			if ( def != W3 )
				::lstrcpy( W3, def );
			len = ::lstrlen( W3 );
		}
		else 
			len = ::LoadString( _Module.GetModuleInstance(), (AVP_dword)def, A3 );
	}
	else
		len = 0;
	
	if ( len ) {
		W3[len++] = comma ? ',' : ' ';
		if ( len < 40 ) {
			::memset( W3+len, ' ', 40-len );
			len = 40;
		}
		*((DWORD*)(W3+len)) = MAKELONG( MAKEWORD('/','/'), MAKEWORD(' ',0) );
		::lstrcpy( W3+len+3, comment );
		return PutString( sect, 4, W3 );
	}
	else
		return CG_E_IFACE_DEF;
}



// ---
const char* GetSizeString( AVP_dword type ) {
	switch ( type ) {
    case tid_BYTE             : return "sizeof(tBYTE)";
    case tid_WORD             : return "sizeof(tWORD)";
    case tid_DWORD            : return "sizeof(tDWORD)";
    case tid_QWORD            : return "sizeof(tQWORD)";
    case tid_BOOL             : return "sizeof(tBOOL)";
    case tid_CHAR             : return "sizeof(tCHAR)";
    case tid_WCHAR            : return "sizeof(tWCHAR)";
    case tid_STRING           : return "string length";
    case tid_WSTRING          : return "wstring length";
    case tid_ERROR            : return "sizeof(tERROR)";
    case tid_INT              : return "sizeof(tINT)";    // native signed interger tidype
    case tid_UINT             : return "sizeof(tUINT)";   // native unsigned interger tidype
    case tid_SBYTE            : return "sizeof(tSBYTE)";
    case tid_SHORT            : return "sizeof(tSHORT)";   
    case tid_LONG             : return "sizeof(tLONG)";   
    case tid_LONGLONG         : return "sizeof(tLONGLONG)";
    case tid_IID              : return "sizeof(tIID)";    // plugin interface identifier
    case tid_PID              : return "sizeof(tPID)";    // plugin identifier
    case tid_ORIG_ID          : return "sizeof(tORIG_ID)";// object origin identifier
    case tid_OS_ID            : return "sizeof(tOS_ID)";  // folder type identifier
    case tid_VID              : return "sizeof(tVID)";    // vendor ideftifier
    case tid_PROPID           : return "sizeof(tPROPID)"; // property identifier
    case tid_VERSION          : return "sizeof(tVERSION)";// version of any identifier: High word - version, Low word - subversion
    case tid_CODEPAGE         : return "sizeof(tCODEPAGE)";// codepage identifier
    case tid_DATA             : return "sizeof(tDATA)";   // universal data storage
    case tid_DATETIME         : return "sizeof(tDATETIME)";
    default                   : return "unknown size";
	}
	
}


// ---
const char* req_write( AVP_bool req, AVP_bool writable ) {
	if ( req && writable )
		return (const char*)IDS_PROP_DEF_LOCAL_REQ_WRITABLE_OI_FN_FMT;
	else if ( req )
		return (const char*)IDS_PROP_DEF_LOCAL_REQUIR_FN_FMT;
	else if ( writable )
		return (const char*)IDS_PROP_DEF_LOCAL_WRITABLE_OI_FN_FMT;
	else
		return (const char*)IDS_PROP_DEF_LOCAL_FN_FMT;
}



// ---
// format specifications
tSTRING fs( AVP_dword type ) {
	switch( type ) {
    case tid_BYTE             : return "c";
    case tid_WORD             : return "u";
    case tid_DWORD            : return "u";
    case tid_QWORD            : return "I64u";
    case tid_BOOL             : return "u";
    case tid_CHAR             : return "c";
    case tid_WCHAR            : return "C";
    case tid_STRING           : return "s";
    case tid_WSTRING          : return "S";
    case tid_ERROR            : return "u";
    case tid_PTR              : return "p";
    case tid_INT              : return "d";     // native signed interger tidype
    case tid_UINT             : return "u";     // native unsigned interger tidype
    case tid_SBYTE            : return "c";
    case tid_SHORT            : return "u";   
    case tid_LONG             : return "d";   
    case tid_LONGLONG         : return "I64d";
    case tid_IID              : return "u";     // plugin interface identifier
    case tid_PID              : return "u";     // plugin identifier
    case tid_ORIG_ID          : return "u";     // object origin identifier
    case tid_OS_ID            : return "u";     // folder type identifier
    case tid_VID              : return "u";     // vendor ideftifier
    case tid_PROPID           : return "u";     // property identifier
    case tid_VERSION          : return "u";     // version of any identifier: High word - version, Low word - subversion
    case tid_CODEPAGE         : return "u";     // codepage identifier
    case tid_LCID             : return "u";     // codepage identifier
    case tid_DATA             : return "u";     // universal data storage
    case tid_DATETIME         : return "I64u";
    default                   : return "p";
	}
}



