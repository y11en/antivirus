// CodeGen.h : Declaration of the CCodeGen

#ifndef __SRCGEN_H_
#define __SRCGEN_H_

#include "resource.h"       // main symbols
#include <property/property.h>
#include "..\PCGInterface.h"
#include "..\PGIServerInterface.h"
#include "..\IFaceInfo.h"
#include <stuff\parray.h>
#include <stuff\sarray.h>
#include <stuff\cpointer.h>
#include "Options.h"
#include "OutputFile.h"

#include <set>
#include <string>

// ---
class PropMethod {
	
	DWORD         m_mode;
	char*         m_name;
	CPArray<char> m_props;
	CSArray<bool> m_globals;
	
public:
	
	PropMethod( DWORD mode, bool global, const char* name, const char* prop );
	~PropMethod();
	void AddPropName( const char* prop, bool global );
	
	DWORD                 Mode()                         const { return m_mode; }
	char*                 Name()                         const { return m_name; }
	const CPArray<char>&  Props()                        const { return m_props; }
	const CSArray<bool>&  Globs()                        const { return m_globals; }
	bool                  operator==( const char* name ) const { return !::lstrcmp(m_name,name); }
};



// ---
struct CIfaceParam {
	CAPointer<char> m_type;
	CAPointer<char> m_name;
	
	CIfaceParam( const char* type, const char* name );
	~CIfaceParam();
};



// ---
struct CIfaceMethod {
	CAPointer<char>      m_name;
	CPArray<CIfaceParam> m_params;
	CAPointer<char>      m_comment;
	
	CIfaceMethod( CMethodInfo& mi, CObjTypeInfo& types );
	~CIfaceMethod();
	
	bool add( CIfaceParam* param ) { m_params.Add(param);	return true; }
};

#define ENUM_IF_SELECTED		1
#define ENUM_IF_NEXT				2
#define ENUM_IF_INIT_EXPORT	4
#define ENUM_IF_INIT_INFO		8

/////////////////////////////////////////////////////////////////////////////
// CCodeGen
class CCodeGen : public COutputMerge, public IPragueCodeGen
{
	
	friend class SectionStamp;
	
public:
	CCodeGen();
	~CCodeGen();
	
protected:
	static const char*  m_desriptions[];
	uint						m_desc_id;
	
	COptions				m_opt;
	CStdTypeInfo    m_types;
	CPluginInfo     m_plugin;
	CTreeInfo       m_tree;
	CFaceInfo       m_iface;
	CAPointer<char> m_int_type_name;
	bool            m_data_single;
	CAPointer<char> m_data_name;
	int             m_data_name_len;
	bool            m_kernel;
	bool            m_start_project;
	bool			m_change_version;
	CAPointer<char> m_version;
	
	CAPointer<char> m_output_folder;
	CAPointer<char> m_public_folder;
	const char*     m_inc_prefix;
	const char*     m_prop_prefix;
	const char*     m_source_ext;
	const char*     m_this;
	
	CPArray<char>		m_iface_names;
	bool						m_plugin_mode;
	bool						m_plugin_public;
	
	bool			m_prototype;
	const char*     m_src_name;
	const char*     m_hdr_name;
	
	uint						m_prop_tbl_count;
	CPArray<char>		m_prop_errors;
	CPArray<char>		m_delayed_props;
	
	CPArray<PropMethod> m_methods;
	CPubMethodInfo *	m_ex_methods;
	bool						m_is_ex_methods;

	CComPtr<IPragueIServer> m_additional_ifaces;
	HPROP                   m_additional_includes;

	
	// IPragueCodeGen
public:
	STDMETHOD (GetDescription)                  ( BYTE** description );
	STDMETHOD (GetVendor)                       ( BYTE** vendor );
	STDMETHOD (GetVersion)                      ( BYTE** version );
	STDMETHOD (GetInfo)                         ( DWORD* len, BYTE** info );
	STDMETHOD (GetPrototypeOptionTemplate)      ( DWORD* len, BYTE** options );
	STDMETHOD (GetImplementationOptionTemplate) ( DWORD* len, BYTE** options );
	
	STDMETHOD (GetProjectOptions)               ( DWORD* len, BYTE** buffer );
	STDMETHOD (SetObjectTypes)                  ( DWORD len, BYTE* objtypes );
	STDMETHOD (StartProject)                    ( DWORD len, BYTE* buffer );
	STDMETHOD (EndProject)                      (  );
	STDMETHOD (PrototypeGenerate)               ( DWORD clen, BYTE* cont, DWORD olen, BYTE* options );
	STDMETHOD (ImplementationGenerate)          ( DWORD clen, BYTE* contents, DWORD olen, BYTE* options );
	STDMETHOD (EditNode)                        ( HWND dad, DWORD* len, BYTE** node );
	STDMETHOD (GetErrorString)                  ( HRESULT err, BYTE** err_str );
	
protected:
	
	//HRESULT BackupFile( const char* file_name, COutputFile* old );
	HRESULT LoadFile( const char* file_name, COutputFile* old );
	HRESULT SwitchFile( const char* new_file_name, const char* real_file_name, const char* back_file_name );
	tBOOL		CheckFile( const char* name );
	tBOOL		CheckVersion( COutputFile &ofile, const char* file_name );
	
	// GenFiles.cpp
	enum GenType{ Plugin, Source, Prototype };
	
	virtual HRESULT GenProtoFiles();
	virtual HRESULT GenPluginFiles();
	virtual HRESULT GenSourceFiles();
	
	virtual HRESULT GenPluginIncludeFile( COutputFile& of, bool v2_public = false );
	virtual HRESULT GenPluginSourceFile( COutputFile& of );
	virtual HRESULT GenPluginSourceInclude( COutputSect& sect );
	virtual HRESULT GenPluginSourceImpl( COutputFile& of );
	virtual int			GenPluginRegCall( COutputSect& sect, const char* iface_name, int where=32767 );
	virtual HRESULT GenPluginIncludeDefs( COutputFile& of );
	
	virtual bool		IsGenType( GenType type );
	virtual HRESULT GenFileName( GenType type, bool prototype, char* name, int size );
	virtual HRESULT GenFile( GenType type, bool prototype );
	virtual HRESULT GenIncludeFile( COutputFile& of );
	virtual HRESULT GenSourceFile( COutputFile& of );
	virtual HRESULT GenProtoFile( COutputFile& of );
	
	// header file - means common include file for this interface
	HRESULT HeaderToHdr( COutputFile& file, const char* file_name, const char* def_str, bool header );
	HRESULT IncludesToHdr( COutputFile& file );
	
	HRESULT TypeToHdr( COutputSect &sect, CTypeInfo &ti, bool is_comment );
	HRESULT ConstToHdr( COutputSect &sect, CConstantInfo &ci, bool for_prototype, bool is_comment );
	HRESULT MsgClassInfoToHdr( COutputSect& sect, CMsgClassInfo& mci, bool for_prototype, bool is_comment );
	HRESULT ErrorToHdr( COutputSect &sect, CErrInfo &ei, AVP_dword &cur_id, bool is_comment, bool from_iface );
	
	HRESULT TypesToHdr( COutputFile& file );
	HRESULT ConstsToHdr( COutputFile& file, bool for_prototype );
	HRESULT MsgClassesToHdr( COutputFile& file, bool for_prototype );
	HRESULT ErrorsToHdr( COutputFile& file );
	
	HRESULT InterfaceToHdr( COutputFile& file );
	HRESULT VtblToHdr( COutputFile& file, CPArray<char>& defines, bool internal );
	HRESULT ParseIFaceMethods( CPArray<char>&names, CPArray<char>&typedefs, CPArray<char>& methods, CPArray<char>& defines, bool internal, bool extended=false );
	HRESULT CalcMethodsWidths( const CMethodInfo& mi, DWORD widths[5], bool internal ); // , CPArray<char>& forwards );
	HRESULT FooterToHdr( COutputFile& file, const char* def_str );
	HRESULT ObjDescription( COutputFile& ofile );
	HRESULT CollectMethods( CPArray<CIfaceMethod>& methods, bool extend=false );
	HRESULT CppMethodToHdr( COutputSect& sect, CIfaceMethod& meth, uint shift );
	HRESULT CppPropToHdr( COutputSect& sect, CPropInfo& prop, CPropTypeInfo& types, uint shift );
	HRESULT CppDefinesToHdr( COutputFile& ofile );
	
	// include file - means include file for this implementation
	HRESULT HeaderToInclude( COutputFile& file, const char* file_name );
	HRESULT InterfaceHdrsToInclude( COutputFile& file );
	HRESULT InterfaceToInclude( COutputFile& file, const char* src_file_name );
	HRESULT FooterToInclude( COutputFile& file, const char* file_name );
	
	// source file
	HRESULT HeaderToSource( COutputFile& file, const char* src_file_name );
	virtual HRESULT InterfaceToSource( COutputFile& file, const char* inc_file_name );
	HRESULT FooterToSource( COutputFile& file, const char* file_name );
	
	// internal data structure 
	HRESULT DataStructsToSource( COutputSect& sect );
	HRESULT StructToSource( COutputSect& sect, const CDataStructInfo& str, DWORD shift, tBOOL bMakeMemberPrefix = cFALSE );
	
	virtual HRESULT PrivateDecl( COutputFile& ofile );
	
	// version declaration
	virtual HRESULT VersionDecl( COutputFile& file );
	
	// properties
	virtual const char * MakePropFuncName( const char *func_name );
	virtual HRESULT MakeTablePropFuncName( CAPointer<char> &name, const char *func_name );
	
	HRESULT CollectPropMethods();
	HRESULT CollectLastPropMethods( const CPropInfo& pi );
	HRESULT PropertiesToSource( COutputFile& file );
	HRESULT PropTableToSource( COutputFile& ofile );
	virtual HRESULT PropMethodToSource( COutputFile& file, PropMethod* method );
	virtual HRESULT PropMethodDefsToSource( COutputSect &sect, PropMethod* method, bool forward_decl );
	HRESULT GlobalPropVarsToSource( COutputFile& ofile );
	HRESULT PropToSource( COutputSect& sect, const CPropInfo& prop );
	bool		isMemberAhString( const char* member );
	HRESULT LocalPropToSource( COutputSect& sect, const tCHAR* prop_name, const CPropInfo& prop );
	HRESULT StaticPropToSource( COutputSect& sect, const tCHAR* prop_name, const CPropInfo& prop );
	bool		LastPropToSource( COutputSect& sect );
	virtual HRESULT CautionsToSource( COutputFile& ofile );
	
	// registration
	virtual HRESULT RegisterIFaceBegin( COutputFile& file ){ return S_OK; };
	virtual HRESULT RegisterCallToSource( COutputFile& file ); // save one intrface method to include file
	virtual HRESULT RegisterCallImplToSource( COutputSect& sect );
	virtual HRESULT RegisterCallGetDataSize( char *buffer );
	
	// utilities
	HRESULT		PutString( COutputSect& sect, DWORD shift, const char* s, int pos=(int)(((unsigned(-1))/2)-1) );  // place string to output file with CR/LF
	HRESULT   PutFmtString( COutputSect& f, DWORD shift, const char* fmt, ... ); // load format from resource and PutString. Uses W0 and W1. All pars MUST be convertible to DWORD
	
	AVP_dword   ResolveTypeID( const CBaseInfo& item );
	
	virtual AVP_dword	MakeIFaceMethodPrefix( char* buffer, const CMethodInfo& mi, bool forward_decl );
	virtual AVP_dword	MakeIFaceMethodPosix( char* buffer, const CMethodInfo& mi, bool forward_decl );
	
	HRESULT     IFaceMethodDefsToSource( COutputSect& sect, const CMethodInfo& method, bool forward_decl );
	HRESULT     IFaceMethodTableToSource( COutputFile& file, CMethodInfo& mi );
	HRESULT     IFaceMethodToSource( COutputFile& file, const CMethodInfo& method, bool internal_func );
	HRESULT     ExtractTemplate( UINT id, AVP_dword* addr, DWORD* len, BYTE** output, bool hdr );
	HRESULT     ExtractString( UINT id, BYTE** output );
	//HRESULT     AddFmtString( CPArray<char>& strings, DWORD shift, const char* fmt, ... ); // load format from resource and PutString. Uses W0 and W1. All pars MUST be convertible to DWORD
	HRESULT     PutHeader( COutputFile& file, const char* file_name );
	HRESULT     PutRegParam( COutputSect& file, const char* def, const char* comment, bool comma=true );
	HRESULT     CalcPropWidths( CPropInfo& prop, bool native, DWORD widths[2] );
	HRESULT     PropToInclude( COutputSect& file, CPropInfo& prop, AVP_bool native, DWORD widths[2], bool is_comment = false );
	HRESULT     CreateIFaceDefString( CAPointer<char>& def_str );
	DWORD       GetParamType( CMethodParamInfo& param, char* buffer, AVP_dword ilen ); // , CPArray<char>* forwards );
	HRESULT     PutExtendedComment( COutputSect& sect, DWORD shift, const char* header, const CBaseInfo& node, DWORD id, bool new_line = true );
	HRESULT     PutErrors( COutputSect& sect, CPArray<char>& p, const char* header );
	const char* ExtractFileName( const char* path_name, char* file_name );
	HRESULT     PutIFaceComment( COutputFile& file, COutputSect* to_sect = NULL );
	HRESULT     PutIFaceIdentifier( COutputFile& file, AVP_dword* out_id );
	HRESULT			PutMethodExComment( COutputSect& sect, const CMethodInfo& mi );
	HRESULT     LoadContents( DWORD len, BYTE* contents );
	HRESULT     LoadOptions( DWORD len, BYTE* options );
	bool        HasMethodTable( DWORD table_id );
	bool        PutSeparator( COutputSect& sect, const CBaseInfo& item );
	
	bool				EnumIFaces(int &flags);
	bool        InitIFaceInfo();
	virtual void	InitDataStructName(CDataStructInfo &ds);
	
	HRESULT     Body_of_the_Recognize_and_ObjectNew( COutputSect& sect, DWORD rtype, const CMethodInfo& method );
	HRESULT     Body_of_the_ObjectClose( COutputSect& sect, DWORD rtype, const CMethodInfo& method );
	HRESULT     Body_of_the_ERROR_returned_method( COutputSect& sect, DWORD rtype, const CMethodInfo& method, bool internal_func );
	HRESULT     Body_of_the_ordinary_method( COutputSect& sect, DWORD rtype, const CMethodInfo& method, bool internal_func );
	
	// merge functions
public:
	void	m_meth_template( COutputSect& old, COutputSect& anew, const char* tmpl );
	void	m_merge( COutputSect& old, COutputSect& anew );
	void	m_replace( COutputSect& old, COutputSect& anew );
	void	m_if_header( COutputSect& old, COutputSect& anew );
	void	m_if_includes( COutputSect& old, COutputSect& anew );
	void	m_if_comment( COutputSect& old, COutputSect& anew );
	void	m_priv_defs( COutputSect& old, COutputSect& anew );
	void	m_reg_call( COutputSect& old, COutputSect& anew );
	void	m_method( COutputSect& old, COutputSect& anew );
	void	m_pl_def( COutputSect& old, COutputSect& anew );
	void	m_pr_table( COutputSect& old, COutputSect& anew );
	void	m_move( COutputSect& old, COutputSect& anew );
	void	m_drop( COutputSect& old, COutputSect& anew );
	void	m_remove_cpp( COutputSect& old, COutputSect& anew );
	void	m_meth_forw_decl( COutputSect& old, COutputSect& anew );
	void	m_meth_ex_prt( COutputSect& old, COutputSect& anew );
	void	m_meth_table( COutputSect& old, COutputSect& anew );
	void	m_pl_or_ven_id( COutputSect& old, COutputSect& anew );
	void	m_msg( COutputSect& old, COutputSect& anew );
	
private:
	void	do_merge( COutputSect& old, COutputSect& anew );
	bool	check_put_old(COutputSect &sect);
};            

//---
class ATL_NO_VTABLE CCodeGenC : 
public CComObjectRootEx<CComSingleThreadModel>,
public CComCoClass<CCodeGenC, &CLSID_PragueCodeGen>,
public CCodeGen
{
	
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_SRCGEN)
		DECLARE_NOT_AGGREGATABLE(CCodeGenC)
		
		DECLARE_PROTECT_FINAL_CONSTRUCT()
		
		BEGIN_COM_MAP(CCodeGenC)
		COM_INTERFACE_ENTRY(IPragueCodeGen)
		END_COM_MAP()
};

class CCodeGen2 : public CCodeGen
{
private:
	// Generation
	STDMETHOD (GetVersion)                      ( BYTE** version );
	
protected:
	CCodeGen2();
	
	HRESULT PrivateDecl( COutputFile& ofile ){ return S_OK; }
	
	HRESULT GenPluginSourceInclude( COutputSect& sect );
	HRESULT GenPluginSourceImpl( COutputFile& of );
	HRESULT GenPluginIncludeDefs( COutputFile& of );
	HRESULT GenPluginIncludeFile( COutputFile& of, bool v2_public = false );
	HRESULT GenPluginExportMethods( COutputFile& of );
	HRESULT GenPluginSourceExportImpl( COutputFile& of, CPubMethodInfo &mi );
	HRESULT GenPluginIncludeInterfaceDefs( COutputSect& sect );
	
	bool		IsGenType( GenType type );
	
	HRESULT GenIncludeFile( COutputFile& of );
	HRESULT GenSourceFile( COutputFile& of );
	
	HRESULT GenIncludeDefinition( COutputFile& of, bool prototype );
	virtual HRESULT GenClassDefinition( COutputFile& of, bool prototype );
	virtual HRESULT InternalTableToSource( COutputFile& ofile, CIntMethodInfo &imi );
	virtual HRESULT ExternalTableToSource( COutputFile& ofile, CPubMethodInfo &pmi );
	
	HRESULT CautionsToSource( COutputFile& ofile );
	HRESULT InterfaceToSource( COutputFile& ofile, const char* inc_file_name );
};

//---
class ATL_NO_VTABLE CCodeGenC2 : 
public CComObjectRootEx<CComSingleThreadModel>,
public CComCoClass<CCodeGenC2, &CLSID_PragueCodeGen2>,
public CCodeGen2
{
	
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_SRCGEN2)
		DECLARE_NOT_AGGREGATABLE(CCodeGenC2)
		
		DECLARE_PROTECT_FINAL_CONSTRUCT()
		
		BEGIN_COM_MAP(CCodeGenC2)
		COM_INTERFACE_ENTRY(IPragueCodeGen)
		END_COM_MAP()
};

//---
class ATL_NO_VTABLE CCodeGenCPP : 
public CComObjectRootEx<CComSingleThreadModel>,
public CComCoClass<CCodeGenCPP, &CLSID_PragueCodeGenCPP>,
public CCodeGen2
{
	
public:
	CCodeGenCPP();
	
	DECLARE_REGISTRY_RESOURCEID(IDR_SRCGENCPP)
		DECLARE_NOT_AGGREGATABLE(CCodeGenCPP)
		
		DECLARE_PROTECT_FINAL_CONSTRUCT()
		
		BEGIN_COM_MAP(CCodeGenCPP)
		COM_INTERFACE_ENTRY(IPragueCodeGen)
		END_COM_MAP()
		
		// IPragueCodeGen
public:
	STDMETHOD (GetDescription)                  ( BYTE** description );
	STDMETHOD (GetVersion)                      ( BYTE** version );
	
	// Generation
	HRESULT GenClassDefinition( COutputFile& of, bool prototype );
	
	HRESULT InternalTableToSource( COutputFile& ofile, CIntMethodInfo &imi );
	HRESULT ExternalTableToSource( COutputFile& ofile, CPubMethodInfo &pmi );
	
	HRESULT RegisterIFaceBegin( COutputFile& file );
	HRESULT RegisterCallToSource( COutputFile& file );
	HRESULT RegisterCallGetDataSize( char *buffer );
	
	const char * MakePropFuncName( const char *func_name );
	HRESULT MakeTablePropFuncName( CAPointer<char> &name, const char *func_name );
	AVP_dword	MakeIFaceMethodPrefix( char* buffer, const CMethodInfo& mi, bool forward_decl );
	
	HRESULT PropMethodDefsToSource( COutputSect &sect, PropMethod* method, bool forward_decl );
	
	void		InitDataStructName(CDataStructInfo &ds);
};

typedef CAPointer<char> CStr;

#define	g_trace_def		"PR_TRACE"

extern char	work[4][0x1000];

#define BUFFS   (1000)
#define REST(l) (BUFFS-l)

#define W0 (work[0]) 
#define W1 (work[1]) 
#define W2 (work[2]) 
#define W3 (work[3]) 

#define A0 work[0], sizeof(work[0])
#define A1 work[1], sizeof(work[1])
#define A2 work[2], sizeof(work[2])
#define A3 work[3], sizeof(work[3])

#define B0(s) work[0]+(s), sizeof(work[0])-(s)
#define B1(s) work[1]+(s), sizeof(work[1])-(s)
#define B2(s) work[2]+(s), sizeof(work[2])-(s)
#define B3(s) work[3]+(s), sizeof(work[3])-(s)

#define BRACKET_O ('(')
#define BRACKET_C (')')
#define BRACKET_C_SEMICOLON  ");"

#define BRACE_O   ('{')
#define BRACE_C   ('}')
#define BRACE_OS  "{"
#define BRACE_CS  "}"
#define BRACE_C_SEMICOLON  "};"

#define TERM      (0)
#define SPACE     (' ')
#define COMMA     (',')
#define SEMICOLON (';')
#define SLASH     ('/')
#define STAR      ('*')
#define COLON     (':')

#define PLC  "PROP_LAST_CALL"

#define UNKNOWN_RESULT_TYPE "Unknown_Result_Type"
#define UNKNOWN_METHOD_NAME "Unknown_Method_Name"
#define UNKNOWN_PARAM_TYPE  "Unknown_Param_Type"

#define HAS_RET_VAL( tid, tot )  (((tot)!=VE_NTT_GLOBALTYPE) || !(((tid)==tid_VOID) || ((tid)==tid_ERROR))) //  || ((tid)==tid_BOOL)

#define SET_STRING(str, val)			{ uint l = strlen(val)+1; str = (char*)::memcpy( new char[l], val, l ); }

BOOL CheckOutFile(LPCTSTR pContainerName);

#endif //__SRCGEN_H_
