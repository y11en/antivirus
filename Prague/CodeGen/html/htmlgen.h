// CodeGen.h : Declaration of the CCodeGen

#ifndef __SRCGEN_H_
#define __SRCGEN_H_

#include "resource.h"       // main symbols
#include "..\PCGInterface.h"
#include <ifaceinfo.h>
#include <stuff\parray.h>
#include "html.h"


struct tdescriptor {
	tDWORD      type;
	bool        on;
	bool        inlude_imp_mode;
	ifEntity    en_type;
	const char* name;
};

class CComment;
typedef enum { l_rus=1, l_eng, l_any	} tLang;
typedef enum { t_short=1, t_large=2, t_behav=4, t_sl=t_short|t_large, t_sb=t_short|t_behav, t_lb=t_large|t_behav, t_all=t_short|t_large|t_behav } tType;

tBOOL CheckFile( const char* name );


class ATL_NO_VTABLE CCodeGen;

/////////////////////////////////////////////////////////////////////////////
// CCodeGen
class ATL_NO_VTABLE CCodeGen : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCodeGen, &CLSID_PragueCodeGen>,
	public IPragueCodeGen
{

public:
	CCodeGen();
	~CCodeGen();

DECLARE_REGISTRY_RESOURCEID(IDR_SRCGEN)
DECLARE_NOT_AGGREGATABLE(CCodeGen)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CCodeGen)
	COM_INTERFACE_ENTRY(IPragueCodeGen)
END_COM_MAP()

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

private:
  CStdTypeInfo    m_types;
  CPluginInfo     m_plugin;
  CTreeInfo       m_tree;
  CFaceInfo       m_iface;
	CAPointer<char> m_output_folder;
	bool            m_full;
	bool            m_brick;
	tLang           m_lang;
	bool            m_plugin_done;
	
private:
	HRESULT     ExtractTemplate( UINT id, DWORD* addr, DWORD* len, BYTE** output );
	HRESULT     ExtractString( UINT id, BYTE** output );
  HRESULT     LoadContents( DWORD len, BYTE* contents );
  HRESULT     LoadOptions( DWORD len, BYTE* options );
	
	HRESULT     IfaceHTML( CHtml* body, bool prototype );
	HRESULT     ListEntities( CHtml* parent, CHtml* dir, HDATA elem, const tdescriptor* descr, const char* ename );
	HRESULT     MethodHTML( CHtml* parent, CMethodInfo& method );
	
	HRESULT     ImpInterfaceSourceTable( CHtml* body );
	DWORD       PrepareMethodString( const CMethodInfo& mi, CAPointer<char>& str );
	DWORD       GetParamType( CMethodParamInfo& param, char* buffer, tDWORD ilen );
	tDWORD      ResolveTypeID( const CBaseInfo& item );
	bool        CommentExist( CBaseInfo& list, bool large, bool shrt, bool behav );
	bool        ListCommentExist( CBaseInfo& list, bool property, bool large, bool shrt, bool behav );
	bool        GroupCommentExist( CBaseInfo& list );
	bool        comment( CHtml* body, CComment& comm, tLang lang, tType type, bool para=true );
	bool        name_gen( CBaseInfo& module, bool prototype, char* full, char* name );
	HDATA       find_el( CBaseInfo& dad, const tdescriptor* descr );
	void        msg_info( CBaseInfo& bi, CHtml* dad, tDWORD id, const char* name );

  typedef HRESULT ( CCodeGen::*tfnc)( bool prototype, const char* src_name );
  HRESULT     MakePluginHTML( bool prototype, const char* src_name );
  HRESULT     MakeIfaceHTML( bool prototype, const char* src_name );
  HRESULT     GenFile( tfnc fn, bool prototype, const char* src_name, const char* new_name, const char* old_name );
    
  void        make_file_names( const char* name, const char* ext, CAPointer<char>& fn, CAPointer<char>& fnew, CAPointer<char>& fold );
  HRESULT     SwitchFile( const char* new_file_name, const char* real_file_name, const char* back_file_name );
};



// ---
class CComment {

protected:
	bool            m_title;
	CBaseInfo       m_info;
	CAPointer<char> sr;
	CAPointer<char> se;
	CAPointer<char> lr;
	CAPointer<char> le;
	CAPointer<char> br;
	CAPointer<char> be;
	
public:
	CComment( CBaseInfo& item );
	bool exist( tLang lang, tType type ) const;
	const char* str( tLang lang, tType type ) const;
	CBaseInfo& item() { return m_info; }
	bool title() const { return m_title; }
};


// ---
class CCommentGroup : public CPArray<CComment>, public CComment {
	
	CAPointer<char> rtopic;
	CAPointer<char> etopic;
	
public:
	CCommentGroup( CBaseInfo& group, tLang lang );
	bool exist( tLang lang, tType type ) const;
	const char* str( int index, tLang lang, tType type ) const;
	const char* topic( tLang lang ) const { return (lang == l_rus) ? rtopic : ((lang==l_eng) ? etopic : (rtopic?rtopic:etopic)); }
};



// ---
class CCommentList : public CPArray<CCommentGroup> {

public:
	CCommentList( CBaseInfo& list, tLang lang );
	bool exist( tLang lang, tType type ) const;
	bool combine_topics( tLang lang );
};



extern char      g_buff[];
extern const int g_buffs;
extern char      g_work[];
extern const int g_works;


extern const tdescriptor g_descr[];
extern const int         g_descr_count;


#define BRACKET_O ('(')
#define BRACKET_C (')')
#define BRACKET_C_SEMICOLON  ");"

#define BRACE_O   ('{')
#define BRACE_C   ('}')
#define BRACE_OS  "{"
#define BRACE_CS  "}"
#define BRACE_C_SEMICOLON  "};"

#define SPACE     (' ')
#define COMMA     (',')

#define HAS_RET_VAL( tid, tot )  (((tot)!=VE_NTT_GLOBALTYPE) || !(((tid)==tid_VOID) || ((tid)==tid_ERROR) || ((tid)==tid_BOOL)))

#define UNKNOWN_RESULT_TYPE "Unknown_Result_Type"
#define UNKNOWN_METHOD_NAME "Unknown_Method_Name"
#define UNKNOWN_PARAM_TYPE  "Unknown_Param_Type"


extern const char* g_ec;
extern const char* g_esc;
extern const char* g_elc;
//extern const char* g_ebc;
extern const char* g_ebc;


extern const char* gst_co;
extern const char* gst_sc;
extern const char* gst_lc;
extern const char* gst_bc;
extern const char* gst_nc;
extern const char* gst_item_id;
extern const char* gst_syntax;
extern const char* gst_source;
extern const char* gst_topic;

const char* get_pl_name( CPluginInfo& pl );
void divide( CAPointer<char>& r, CAPointer<char>& e, char* buff );

BOOL CheckOutFile(LPCTSTR name);

#endif //__SRCGEN_H_
