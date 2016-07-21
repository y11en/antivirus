// OutputFile.h: interface for the COutputFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTFILE_H__C048C7C5_DBD9_11D4_B767_00D0B7170E50__INCLUDED_)
#define AFX_OUTPUTFILE_H__C048C7C5_DBD9_11D4_B767_00D0B7170E50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stuff/parray.h>
#include <stuff/cpointer.h>

class COutputFile;
class COutputSect;

typedef CAPointer<char> CStr;
typedef bool (*cmp_func)( const char*, const char* );

// ---
enum ItemID { 
  no_sect,
  just_line = no_sect,
  start,
  if_header=start, if_comment, if_mn_id,  
  pl_def, pl_id, ven_id,
  obj_decl, data_str, 
  pr_meth_decl, pr_gvars, pr_table, pr_meth_impl, 
  ex_meth_impl, in_meth_impl,
  ex_meth_prt,  in_meth_prt,
  ex_meth_tbl,  in_meth_tbl,
  reg_call,
  if_decl, if_forw_decl, if_props, if_defines, if_includes, 
  if_typedefs, if_constants, if_errors, if_meth_forwards,
  private_defs,
  empty,
  caution,
  h_if_def,
  h_includes,
  h_endif,
  h_regcall,
	version_id,
	if_msg_cls,
	if_rpc_def_first,
	if_rpc_def_last,
	if_cpp_decl,
	if_cpp_defs,
	if_cpp_defs_end,
	reg_call_cpp,
	reg_call_cpp_end,
	pl_definitons,
	private_defs_end,
	private_defs_begin,
	pl_exports,
	pl_exports_tbl,
	pl_ex_defs,
	pl_ex_impl,
  stop
};


// ---
class COutputItem {
protected:
  char*        m_str;
  COutputFile* m_file;

public:
  COutputItem( COutputFile& file, char* content, bool own );
  virtual ~COutputItem();

  COutputFile&          file            ()                                    { return *m_file; }
  virtual HRESULT       flash           ( HANDLE file, COptions& opt );
  virtual ItemID        id              ()                              const { return just_line; }
  virtual const char*   name            ()                              const { return m_str; }
  virtual const char*   sub             ()                                    { return 0; }
  virtual void          sub( char* sub_name, bool own )                       { if ( own ) delete [] sub_name; }
  virtual COutputSect*  find_counterpart( COutputFile& src, int* ind )  const { return 0; }
  virtual bool          is_counterpart  ( const COutputSect& src )      const { return false; }
          bool          move_to         ( COutputFile* file, tINT ind );
};

// ---
class COutputMerge
{
public:
	virtual void do_merge( COutputSect& old, COutputSect& anew ){};
	virtual bool check_put_old(COutputSect &sect){ return true; };
};

// ---
class COutputSect : public COutputItem, public CPArray<char> {

  friend class COutputFile;

protected:
  ItemID              m_id;
  bool                m_new;
  
public:
  COutputSect( COutputFile& file, ItemID id, const char* sub_name, bool own );
  COutputSect( COutputFile& file, const char* sect_name, char* sub_name, bool own ); //
  virtual ~COutputSect();

  char *&								at(int i){ return operator [](i); }
  virtual HRESULT       flash( HANDLE file, COptions& opt );
  virtual ItemID        id()                                  const  { return m_id; }
  virtual const char*   name()                                const  { return g_names[m_id]; }
  virtual const char*   sub()                                        { return m_str; }
  virtual void          sub( char* sub_name, bool own );
  virtual COutputSect*  find_counterpart( COutputFile& src, int* ind ) const;

  bool									is_counterpart( const COutputSect& src ) const;
	bool									is_method_section();

	bool									check_name();
	void									comment_out( int pstart=0, int plen=0 );
	bool									cmp_range( int p1, const COutputSect& s2, int p2, int len );
	int										find_by( uint start, const char* str, cmp_func cmp );
	int										find_first_not_commented_line();
	bool									add_open_brace( const char* line );

public:
  static const char*  g_names[];
  static const char   g_delimiter_line[];
  static const char   g_sect_beg[];
  static const char   g_sect_end[];

public:
  static ItemID       map_name_to_id( const char* name );
  static COutputSect* catch_sect( COutputFile& o, const char* sect_name, char* sub_name, bool own );
  static COutputSect* catch_sect( COutputFile& o, ItemID id, char* sub_name, bool own );

	static bool					is_sub_id( ItemID id );
	static bool					is_sect( const char* line, char* name, char* sub_name );
	static bool					is_sect_end( const char* line );
	static bool					is_delimiter( const char* line, int len );
};



// ---
class COutputFile : protected CPArray<COutputItem> {
  friend class COutputItem;
public:
  COutputFile(COutputMerge *merge) : CPArray<COutputItem>(), m_merge(merge) {}
  COutputSect*    last_sect();
  HRESULT         load( const char* f_name );
  HRESULT         load( HANDLE file );
  HRESULT         flash( const char* f_name, COptions& opt );
  bool            flash( HANDLE file, ItemID id, COptions& opt );
  void            merge( COutputFile& anew );
  COutputSect*    find( int* from, ItemID id ) const;
  uint            Count() const { return CPArray<COutputItem>::Count(); }
  COutputItem*&   operator []( uint loc ) const { return CPArray<COutputItem>::operator []( loc ); }

private:
	COutputMerge *	m_merge;
};


// ---
// auxiliary routines (mergers.cpp)
bool is_comment( const char* s );
bool is_empty( const char* s );
bool is_method( const char* s );

bool ignore_spaces( const char*& p );
void comment_out( char*& s );

bool cmp_ignore_spaces( const char* s1, const char* s2 );
bool cmp_by_substr( const char* s1, const char* s2 );

#endif // !defined(AFX_OUTPUTFILE_H__C048C7C5_DBD9_11D4_B767_00D0B7170E50__INCLUDED_)
