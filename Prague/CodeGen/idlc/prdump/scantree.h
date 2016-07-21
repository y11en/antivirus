#if !defined(__scantree_h)
#define __scantree_h

#include <property\property.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <stuff\parray.h>
#include <stuff\sarray.h>
#include <stuff\cpointer.h>
#include "../../Ifaceinfo.h"
#include <windows.h>

typedef unsigned uint;

#define DSC_END_POINT  (-2)
#define DSC_LINK_POINT (-3)

#define DEF_OUT_DIR "o:\\prague.old\\sdk\\dsc\\"
#define DEF_PRT_EXT ".idef"
#define DEF_DSC_EXT ".pdsc"
#define DEF_IMP_EXT ".pdef"

#define n_PROTO     "#import"
#define n_PLUG      "component"
#define n_IFACE     "interface"
#define n_IMP       "implementation"
#define n_DSC       "dsc"
#define n_TYPE      "type"
#define n_CONST     "const"
#define n_PROP      "prop"
#define n_ERR       "error"
#define n_MSG_CL    "message_cls"
#define n_MSG       "message"
#define n_METH      "method"
#define n_SMETH     "sysmeth"
#define n_OSMETH    "sys_method_old"
#define n_METH_P    "" //"param"
#define n_STRUCT    "struct"
#define n_MEMB      "member"

// ---
struct tEnumDsc {
	uint        m_id;
	const char* m_name;
};


// ---
struct tSymbDsc {
	const char* m_node_type;
	uint m_type_name;
	uint m_type_id;
	uint m_const;
	uint m_pointer;
	uint m_dbl_pointer;
	uint m_value;
	uint m_array;
	uint m_array_bound;
	uint m_show;
	uint m_not_show;
};




// ---
struct tPropDsc {
	uint        m_id;
	const char* m_name;
	tEnumDsc*   m_enum;
	bool        m_ignore;
	bool        m_quotation;
	bool        m_show_if_empty;
};


// ---
typedef enum { ecp_b, ecp_h, ecp_a } tCommPlace;

// ---
struct tNodeDsc {
	uint        m_id;
	const char* m_static_type_name;
	uint        m_name_prop_id;
	uint        m_type_prop_id;
	tPropDsc*   m_props;
	tNodeDsc*   m_nodes;
	bool        m_hidden;
	bool        m_array;
	bool        m_last_no_open;
	tCommPlace  m_comment_placement;
	tSymbDsc*   m_symb_dsc;
	const char* m_open;
	const char* m_fin;
};



typedef CAPointer<char> cAutoChar;

// ---
struct cItem : public CPArray<cItem> {
protected:
	bool            m_section;
	CPArray<char>   m_attr;
	cAutoChar	      m_hdr;
	const char*     m_open;
	const char*     m_close;
	cAutoChar	      m_comment;
	tCommPlace      m_comment_placement;
	bool            m_no_open;
	bool            m_hidden;
	cAutoChar	      m_name;
	cAutoChar       m_type;
	uint            m_nt; // nodetype
	bool            m_print_name;
	CFaceInfo       m_base;

public:
	cItem() 
		: m_section(false), 
			m_open(" {"), m_close("}"), 
			m_comment_placement(ecp_h), 
			m_no_open(false), 
			m_hidden(false), 
			m_nt(-1), m_print_name(false),
			m_base(0,false) {
	}
	cItem( const char* header, const char* name, const char* type, uint nt, const char* open=" {", const char* close="}", bool no_open=false ) 
		: m_section(false), 
			m_open(open), m_close(close), 
			m_comment_placement(ecp_h), 
			m_no_open(no_open), 
			m_hidden(false), 
			m_nt(nt), m_print_name(false),
			m_base(0,false) {

		if ( name )
			cItem::name( name );
		if ( type )
			cItem::type( type );
		if ( header )
			hdr( header );
	}

	void hdr( const char* hdr )      { m_hdr = _strdup(hdr); }
	const char* hdr() const          { return m_hdr; }

	void name( const char* name )    { m_name = _strdup(name); }
	const char* name()         const { return m_name; }
	void type( const char* type )    { m_type = _strdup(type); }
	const char* type()         const { return m_type; }

	void open( const char* open )    { m_open = open; }
	void close( const char* close )  { m_close = close; }
	void comment( const char* comm ) { m_comment = _strdup(comm); }
	bool comment()           const   { return m_comment && *m_comment; }
	void cplace( tCommPlace place )  { m_comment_placement = place; }
	void hidden( bool hidden )       { m_hidden = hidden; }
	void no_open( bool set )         { m_no_open = set; }
	void section( bool set )         { m_section = set; }
	bool section()           const   { return m_section; }
	uint nt()                const   { return m_nt; }
	void nt( uint nt )               { m_nt = nt; }

	void print_name( bool set )      { m_print_name = set; }

	virtual bool out( HANDLE file, uint level, bool no_comment, bool no_attrib, bool new_line, bool sp );
	bool comment( HANDLE file, uint level, bool printspace, bool lf, tCommPlace place );
	bool open( HANDLE file, uint level );
	bool close( HANDLE file, uint level, bool lf );
	bool content() const;

	operator bool ()             const { return (m_name && *m_name) || content(); }

	CFaceInfo&  base()                 { return m_base; }
	void        base( HDATA base )     { m_base.Init( base, true ); }

	uint        attr_count()     const { return m_attr.Count(); }
	const char* get_attr( uint i )     { if ( i < m_attr.Count() ) return m_attr[i]; return 0; }
	void        attr( const char* attr );
	void        attr_insert( const char* attr );
};



// ---
class cRootItem : public cItem {

public:
	static const char* s_imp_excludes[];
	static const char* s_tdef_excludes[];

	cRootItem() : cItem(), m_iface(0,false) {}
	void  curr_iface( HDATA iface )  { m_iface = iface; }
	bool  tdef( const char* iface, const char* tdef );
	void  imp( const char* imp );
	char* check_import_type( const char* tname, uint& tid );

	bool       inc( HDATA iface, bool own, bool make_imp, bool check_excld );
	CFaceInfo* inc( uint ind ) { if ( ind < m_incs.Count() ) return m_incs[ind]; return 0; }
	uint       inc_count() const { return m_incs.Count(); }

	virtual bool out( HANDLE file, uint level, bool no_comment, bool no_attrib, bool new_line, bool sp );

protected:
	struct tdef_t {
		tdef_t() : _iface(0), _tdef(0) {}
		tdef_t( const char* iface, const char* tdef ) : _iface(_strdup(iface)), _tdef(_strdup(tdef)) {}
		~tdef_t() { clean();}
		const char* iface() const { return _iface; }
		const char* tdef() const { return _tdef; }

		void set( const char* iface, const char* tdef ) {
			clean();
			_iface = _strdup( iface );
			_tdef = _strdup( tdef );
		}
		void clean() {
			if ( _iface ) {
				delete [] _iface;
				_iface = 0;
			}
			if ( _tdef ) {
				delete [] _tdef;
				_tdef = 0;
			}
		}

		char* _iface;
		char* _tdef;
	};

	CFaceInfo           m_iface;
	CPArray<char>       m_imp;
	CPArray<tdef_t>     m_tdef;
	CPArray<CFaceInfo>  m_incs;
	CPArray<char>       m_unk_types;
	
	struct type_t {
		cAutoChar _tname;
		cAutoChar _resolved;
		uint      _tid;

		type_t() : _tname(), _resolved(), _tid(0) {}
		void set( const char* tname, cAutoChar& resolved, uint tid ) {
			_tname = _strdup(tname);
			_resolved = resolved.Relinquish();
			_tid = tid;
		}
		bool check( const char* tname ) const { return !strcmp(_tname,tname); }
	};
	CSArray<type_t>     m_knwn;

	type_t* find_resolved_type( const char* type );
	type_t* add_resolved_type( const char* type, cAutoChar& resolved, uint tid );
	char*   dup_type( type_t* type );
};



// ---
class cOutput {

protected:
	char* beg;
	char* cp;
	int   m_size;

public:
	
	cOutput() : beg(0), cp(0), m_size(0) {}

	~cOutput() {
		if ( beg )
			free( beg );
	}

	operator bool () const { return cp > beg; }
	cOutput& operator << ( cOutput& s )    { write( s.beg, s.cp-s.beg ); return *this; }
	cOutput& operator << ( const char* s ) { write( s ); return *this; }

	int   size()  const { return m_size; }
	int   avail() const { return cp-beg; }
	char* str()         { return beg; }
	char* reset()       { cp = beg; return str(); }

	bool  write( const char* content, int len = -1 );
	bool  write( uint val, uint base=10 );
	bool  output( HANDLE file );
};



// ---
typedef enum tag_mode { em_prt, em_imp, em_dsc } tMode;

// ---
bool        space( HANDLE file, int c );
bool        printout( HANDLE file, const char* str, int len, int level );


// ---
bool        ScanTree( cItem& node, cItem* parent, tMode mode, HDATA tree, tNodeDsc* dsc, bool ret_convert, bool verbose );
bool        WritePropContent( cOutput& buff, tNodeDsc* dsc, HPROP hProp, tMode mode, const char* prefix, bool open, bool verbose, bool type_details, bool quotation );
HDATA       _find_proto( bool (*checkf)(HDATA data,void* param), void* param );
bool        _known_prop( const char* prop );
const char* _extract_fname( char* path );
bool        _check_keyword( const char* token );
bool        _check_type( const char* type );
CFaceInfo*  _get_proto( HDATA data );
char*       _get_type_by_id_prop( HDATA data, tNodeDsc* dsc, uint& type, uint prop_id );
char*       _clean_str( char* s1 );
bool        _is_datastr_hdl( HDATA data );
HDATA       _data_struct_hdl( HDATA data, bool* multi );
bool        _is_parent_datastr( HDATA data );
bool        _data_struct_name( HDATA data, const char* prfx, cOutput& buff );
cItem*      _find_memb( cItem& data, const char* member );
const char* _get_enum_name( tEnumDsc* en, uint id );
tPropDsc*   _find_pdsc( AVP_dword id, tPropDsc* pdsc );
char*       _check_quot( const char* str );
bool        _get_bool( HDATA data, uint prop_id );
uint        _get_uint( HDATA data, uint prop_id );
bool        _is_native( HDATA data, uint native_id );
char*       _get_data_as_str( HDATA data, uint prop_id );
HDATA       _iface_hdl( HDATA data );
HDATA       _check_base_iface( cItem& node, cOutput& buff, HDATA data, tNodeDsc* dsc );
bool        _check_entity_defined( const CFaceInfo& base, HDATA ent );
bool        _check_defined_in( CBaseInfo& src, CBaseInfo& what );
bool        _check_version_comment_props( cItem& node, tMode mode, HDATA data, tNodeDsc* dsc );
bool        _make_iface_imp_def( cRootItem& node, tMode mode, HDATA data );
void        _make_plg_imp_def( cRootItem& node, HDATA data );
bool        _make_typedef( cRootItem& node, HDATA data );

// ---
struct ctdparam {
	const char*   tname;
	uint&         tid;
	cAutoChar     combined;
	ctdparam( const char* tn, uint& ti ) : tname(tn), tid(ti), combined(0) {}
};

bool _check_type_defined( HDATA iface, ctdparam* p );
bool _check_iface_by_guid( HDATA iface, void* guid );

extern tNodeDsc   prt_root;
extern tNodeDsc   imp_root;
extern tNodeDsc   dsc_root;
extern bool       g_no_lower_fn;
extern bool       g_no_comment;
extern cRootItem* g_root_item;


// ---
inline bool cItem::open( HANDLE file, uint level ) {
	return printout( file, m_open, 0, 0 );
}




#endif