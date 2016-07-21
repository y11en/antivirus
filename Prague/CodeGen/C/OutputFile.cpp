// OutputFile.cpp: implementation of the COutputFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "codegen.h"
#include "OutputFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif


const char COutputSect::g_delimiter_line[] = "// --------------------------------------------------------------------------------";
const char COutputSect::g_sect_beg[] = "// AVP Prague stamp begin( ";   //"// AVPStb( "
const char COutputSect::g_sect_end[] = "// AVP Prague stamp end";       //"// AVPSte"

//* if_h_files       */ "Interface header files",

const char* COutputSect::g_names[] = {
  /* just_line        */ "",
  /* if_header        */ "Interface header",
  /* if_comment       */ "Interface comment",
  /* if_mn_id         */ "Interface menonic identificator",
  /* pl_def           */ "Plugin definitions",
  /* pl_id            */ "Plugin identifier",
  /* ven_id           */ "Vendor identifier",
  /* obj_decl         */ "Object declaration",
  /* data_str         */ "Data structure",
  /* pr_meth_decl     */ "Forwarded property methods declarations",
  /* pr_gvars         */ "Global property variable declaration",
  /* pr_table         */ "Property table",
  /* pr_meth_impl     */ "Property method implementation", 
  /* ex_meth_impl     */ "External (user called) interface method implementation", 
  /* in_meth_impl     */ "Internal (kernel called) interface method implementation",
  /* ex_meth_prt      */ "External method table prototypes",
  /* in_meth_prt      */ "Internal method table prototypes",
  /* ex_meth_tbl      */ "External method table",
  /* in_meth_tbl      */ "Internal method table",   
  /* reg_call         */ "Registration call",
  /* if_decl          */ "Interface declaration",
  /* if_forw_decl     */ "Interface forward declaration",
  /* if_props         */ "Interface declared properties",
  /* if_defines       */ "Defines to simplify interface method calls",
  /* if_includes      */ "Includes for interface",
  /* if_typedefs      */ "Interface typedefs",
  /* if_constants     */ "Interface constants",
  /* if_errors        */ "Interface defined errors",
  /* if_meth_forwards */ "Interface function forward declarations",
  /* private_defs     */ "Private definitions",
  /* empty            */ "",
  /* caution          */ "Caution !!!",
  /* h_if_def         */ "Header ifndef",
  /* h_includes       */ "Header includes",
  /* h_endif          */ "Header endif",
  /* h_regcall        */ "Registration call prototype",
  /* version_id       */ "Interface version",
  /* if_msg_cls       */ "Interface defined messages",
	/* if_rpc_def_first */ "RPC start declaration",
	/* if_rpc_def_last  */ "RPC_ end declaration",
	/* if_cpp_decl      */ "C++ interface declaration",
	/* if_cpp_defs      */ "C++ class declaration",
	/* if_cpp_defs_end  */ "C++ class declaration end",
	/* reg_call_cpp     */ "C++ class regitration",
	/* reg_call_cpp_end */ "C++ class regitration end",
	/* pl_definitons     */ "Plugin public definitions",
  /* private_defs_end   */ "Private definitions end",
  /* private_defs_begin   */ "Private definitions begin",
  /* pl_exports   */ "Plugin export methods",
  /* pl_exports_tbl   */ "Plugin export methods table",
  /* pl_ex_defs   */ "Plugin extern and export declaration",
  /* pl_ex_impl   */ "Plugin export method implementation",
};



// ---
COutputItem::COutputItem( COutputFile& file, char* str, bool own ) : m_str(0), m_file(&file) {
  if ( own )
    m_str = str;
  else if ( str && *str ) {
    int len = ::lstrlen( str ) + 1;
    m_str = (char*)::memcpy( new char[len], str, len );
  }
  else
    m_str = 0;
  file.Add(this); 
}

// ---
COutputItem::~COutputItem() {
  m_file->RemoveObj( this, noDelete );
  if ( m_str )
    delete [] m_str;

}

//////////////////////////////////////////////////////////////////////


// ---
COutputSect::COutputSect( COutputFile& file, ItemID id, const char* sub_name, bool own ) 
  : COutputItem( file, (char*)sub_name, own ),
    CPArray<char>(0,10,true), 
    m_id(id),
    m_new( true ) { 
}

// ---
COutputSect::COutputSect( COutputFile& file, const char* sect_name, char* sub_name, bool own ) 
  : COutputItem( file, sub_name, own ),
    CPArray<char>(0,10,true), 
    m_id(empty),
    m_new( false ) { 

  ItemID i;
  if ( sect_name && *sect_name ) {
    for( i=start; i<stop; (*(int*)&i)++ ) {
      if ( !::lstrcmpi(sect_name,g_names[i]) ) {
        m_id = i;
        return;
      }
    }
  }
  if ( sub_name && *sub_name ) {
    for( i=start; i<stop; (*(int*)&i)++ ) {
      if ( !::lstrcmpi(sub_name,g_names[i]) ) {
        sub( 0, true );
        m_id = i;
        break;
      }
    }
  }
}


// ---
COutputSect::~COutputSect() {
}


// ---
ItemID COutputSect::map_name_to_id( const char* name ) {
  for( ItemID i=start; i<=stop; (*(int*)&i)++ ) {
    if ( !::lstrcmpi(name,g_names[i]) )
      return i;
  }
  return no_sect;
}



// ---
COutputSect* COutputSect::catch_sect( COutputFile& o, const char* sect_name, char* sub_name, bool own ) {
  const char* sect = sect_name;
  char*       sub = sub_name;

  ItemID id = map_name_to_id( sect );
  if ( id == no_sect ) { // in old files first parameter is iface name and second parameter is section name
    sect = sub_name;
    sub = 0;
    id = map_name_to_id( sect );
  }
  return catch_sect( o, id, sub, own ); 
}



// ---
COutputSect* COutputSect::catch_sect( COutputFile& o, ItemID id, char* sub, bool own ) {
  if ( !is_sub_id(id) ) {
    for( uint i=0, c=o.Count(); i<c; i++ ) {
      COutputSect* s = dynamic_cast<COutputSect*>( o[i] );
      if ( s && (s->id() == id) && (!sub || !::lstrcmp(sub,s->sub())) ) {
        if ( own && sub )
          delete [] sub;
        return s;
      }
    }
  }

  return new COutputSect( o, id, sub, own );
}

// ---
bool COutputSect::is_sect( const char* line, char* name, char* sub_name ) {

  if ( line && *line ) {
    int len = ::lstrlen( line );
    if ( (len >= (sizeof(g_sect_beg)-1)) && !::strncmp(line,g_sect_beg,sizeof(g_sect_beg)-1) ) {
      const char* p = line + (sizeof(g_sect_beg) - 1);
    
      while( *p && (*p == ' ') )
        p++;
      while( *p && (*p != ',') )
        *name++ = *p++;
      *name = 0;

      if ( *p && (*p ==',') )
        p++;
      while( *p && (*p == ' ') )
        p++;
      if ( *p && (*p != BRACKET_C) ) {
        ::lstrcpy( sub_name, p );
        int   l = ::lstrlen( sub_name );
        while( l && (sub_name[l-1] == ' ') )
          sub_name[--l] = 0;
        if ( l && (sub_name[l-1] == BRACKET_C) )
          sub_name[--l] = 0;
        while( l && (sub_name[l-1] == ' ') )
          sub_name[--l] = 0;
      }
      else
        *sub_name = 0;
      return true;
    }
  }
  return false;
}



// ---
bool COutputSect::is_sect_end( const char* line ) {
  if ( line && *line ) {
    int len = ::lstrlen( line );
    if ( (len >= (sizeof(g_sect_end)-1)) && !::strncmp(line,g_sect_end,sizeof(g_sect_end)-1) ) 
      return true;
  }
  return false;
}



// ---
bool COutputSect::is_delimiter( const char* line, int len ) {
  static int del_len = ::lstrlen( COutputSect::g_delimiter_line );
  if ( (len == 1) && (line[0] == '\014') )
    return true;
  if ( (len>6) && !::strncmp(line,COutputSect::g_delimiter_line,min(len,del_len)) )
    return true;
  return false;
}


// ---
HRESULT COutputItem::flash( HANDLE file, COptions& opt ) {
  DWORD w = ::lstrlen(m_str);
	bool r;
	if ( m_str && *m_str )
		r = !!::WriteFile( file, m_str, w, &w, 0 );
	else
		r = true;
	if ( r )
		r = !!::WriteFile( file, "\r\n", 2, &w, 0 );
	if ( r )
		return S_OK;
	else {
		w = ::GetLastError();
		return HRESULT_FROM_WIN32( w );
	}
}



// ---
bool COutputItem::move_to( COutputFile* file, tINT ind ) {
  m_file->RemoveObj( this, noDelete );
  (m_file = file)->AddAfter( this, ind );
  return true;
}


	// ---
HRESULT COutputSect::flash( HANDLE file, COptions& o ) {
  DWORD l, w;
  bool  ok = true;
  uint  c = Count();
  bool  method;
  uint  first_line;

  if ( is_method_section() ) {
    first_line = find_first_not_commented_line();
    method = true;
  }
  else {
    first_line = -1;
    method = false;
  }

  if ( o.delimiter_page_break() )
    ::WriteFile( file, "\014", 1, &w, 0 );

  if ( o.delimiter_comment() ) {
    ::WriteFile( file, g_delimiter_line, sizeof(g_delimiter_line)-1, &w, 0 );
    ::WriteFile( file, "\r\n", 2, &w, 0 );
  }

  // section start
  ::WriteFile( file, g_sect_beg, sizeof(g_sect_beg)-1, &w, 0 );
  if ( m_id != empty )
    ::WriteFile( file, g_names[m_id], ::lstrlen(g_names[m_id]), &w, 0 );
  ::WriteFile( file, ", ", 2, &w, 0 );
  if ( m_str && *m_str )
    ::WriteFile( file, m_str, ::lstrlen(m_str), &w, 0 );
  ::WriteFile( file, " )\r\n", 4, &w, 0 );

	uint i;
  for( i=0; ok && i<c; i++ ) {
    bool ob = false;
    const char* p = (*this)[i];
    if ( p ) {
      l = ::lstrlen( p );
    
      while( l && ((p[l-1] == ' ') || (p[l-1] == '\r') || (p[l-1] == '\n')) )
        l--;
    
      if ( (!method || (i<=first_line)) && !is_comment(p) && (p[l-1] == BRACE_O) && o.brace_next_line() ) {
        ob = true;
        l--;
      }
      else
        ob = false;

      if ( (l && !(::WriteFile(file,p,l,&w,0) && (w == l))) || !(::WriteFile(file,"\r\n",2,&w,0) && (w == 2)) )
        ok = false;
      if ( ob ) {
        int c = (*(WORD*)p == MAKEWORD('/','/')) ? 2 : 0;
        const char* e = p + c;
        if ( ignore_spaces(e) ) {
          c += e - p;
          ::WriteFile( file, p, c, &w, 0 );
        }
        ::WriteFile( file, BRACE_OS"\r\n", 3, &w, 0 );
      }
    }
    else
      ok = ::WriteFile(file,"\r\n",2,&w,0) && (w == 2);
  }

  // section stop
  ::WriteFile( file, g_sect_end, sizeof(g_sect_end)-1, &w, 0 );
	for( i=0; i<o.linebreaks_number(); ++i)
		::WriteFile( file, "\r\n", 2, &w, 0 );
  
  if ( ok )
    return S_OK;
  else
    return HRESULT_FROM_WIN32( GetLastError() );
}



// ---
void COutputSect::sub( char* sub_name, bool own ) {
  if ( m_str )
    delete [] m_str;
  if ( own )
    m_str = sub_name;
  else if ( sub_name ) {
    int len = ::lstrlen( sub_name ) + 1;
    m_str = (char*)::memcpy( new char[len], sub_name, len );
  }
  else
    m_str = 0;
}



// ---
COutputSect* COutputSect::find_counterpart( COutputFile& src, int* ind ) const {
  COutputSect* s = 0;
  for ( uint i=0, c=src.Count(); i<c; i++ ) {
    s = dynamic_cast<COutputSect*>( src[i] );
    if ( s ) {
      if ( is_counterpart(*s) ) {
        if ( ind )
          *ind = i;
        break;
      }
      else
        s = 0;
    }
  }
  return s;
}

bool	COutputSect::is_sub_id( ItemID id )
{
  switch( id ) {
    case ex_meth_impl :
    case in_meth_impl :
    case pr_meth_impl :
		case pl_ex_impl:
		case pl_definitons:
			return true;
	}
	return false;
}

bool	COutputSect::is_method_section()
{
  switch( m_id ) {
    case ex_meth_impl :
    case in_meth_impl :
    case pr_meth_impl :
		case pl_ex_impl:
		case pl_def:
			return true;
	}
	return false;
}

// ---
bool COutputSect::is_counterpart( const COutputSect& src ) const {
  if ( m_id != src.m_id )
		return false;

	if( !is_sub_id(m_id) )
		return true;

	return (!m_str == !src.m_str) && (!m_str || !::lstrcmpi(m_str,src.m_str));
}

// ---
int COutputSect::find_by( uint start, const char* str, cmp_func cmp ) {
  for( uint i=start, c=Count(); i<c; i++ ) {
    if ( cmp(at(i),str) )
      return i;
  }
  return -1;
}


// ---
void COutputSect::comment_out( int pstart, int plen ) {
	int count = Count();
	int start = pstart;
	int stop;
	if ( !plen ) 
		stop = count;
	else if ( (count - start) > plen )
		stop = start + plen;
	else
		stop = count;
	for( ; start<stop; start++ )
		::comment_out( at(start) );
}


// ---
int COutputSect::find_first_not_commented_line() {
	int i, c;
  for( i=0,c=Count(); i<c; i++ ) { // find first not commented out line (method headline)
    const char* l = at(i);
    if ( !is_empty(l) && !is_comment(l) )
      return i;
  }
	return -1;
}

// ---
bool COutputSect::check_name() {
  if ( is_sub_id(m_id) && !sub()) {
    uint i = 0, c = Count();
    const char* str;
    if ( m_id == pr_meth_impl ) {
      for( ; i<c; i++ ) {
        str = at(i);
        if ( str && !is_comment(str) ) {
          str = ::strstr( str, "_PROP_" );
          if ( str ) {
            str += 6;
            const char* e = str;
            c = 0;
            while( *e && __iscsym(*e) )
              c++, e++;
            char* ssub = (char*)::memcpy( new char[c+1], str, c );
            *(ssub+c) = 0;
            sub( ssub, true );
            return true;
          }
        }
      }
    }
    else {
      for( ; i<c; i++ ) {
        str = at(i);
        if ( is_comment(str) ) {
          str = ::strstr( str, "Method" );
          if ( str ) {
            str += 6;
            while( *str && ((*str == ' ') || (*str == '\"')) )
              str++;
            const char* e = str;
            c = 0;
            while( *e && (*e != '\"') )
              c++, e++;
            char* ssub = (char*)::memcpy( new char[c+1], str, c );
            *(ssub+c) = 0;
            sub( ssub, true );
            return true;
          }
        }
      }
    }
  }
  return false;
}

// ---
bool COutputSect::add_open_brace( const char* line ) {
  if ( !is_comment(line) ) {
    const char* s = line;
    ignore_spaces( s );
    if ( *s == BRACE_O ) {
      int i = Count() - 1;
      for( ; i>=0; i-- ) {
        char* str = at(i);
        if ( !is_comment(str) ) {
          int l = ::lstrlen( str );
          s = (const char*)::memcpy( new char[l+2], str, l );
          *(WORD*)(s+l) = MAKEWORD( BRACE_O, 0 );
          delete [] str;
          at(i) = (char*)s;
          break;
        }
      }
      return true;
    }
  }
  return false;
}

//////////////////////////////////////////////////////////////////////

class CFileLiner
{
public:
	CFileLiner(HANDLE h) : m_h(h), m_rp(0), m_rc(0){ load_buff(); }

	// ---
	void load_buff() {
		DWORD w;
		if ( m_rc ) {
			if ( m_rp ) {
				::memmove( m_rb, m_rb+m_rp, m_rc );
				m_rp = 0;
			}
			if ( ::ReadFile(m_h,m_rb+m_rc,sizeof(m_rb)-m_rc,&w,0) )
				m_rc += w;
		}
		else if ( ::ReadFile(m_h,m_rb,sizeof(m_rb),&w,0) )
			m_rp = 0, m_rc = w;
		else
			m_rc = 0;
	}

	// ---
	int nc() {

		if ( m_rc ) {
			int r;
			const char* p = m_rb + m_rp++;
			m_rc--;
			if ( (*p == '\r') && (*(p+1) == '\n') ) {
				m_rp++, m_rc--;
				r = 0;
			}
			else if ( (*p == '\n') && (*(p+1) == '\r') ) {
				m_rp++, m_rc--;
				r = 0;
			}
			else if ( (*p == '\r') || (*p == '\n') )
				r = 0;
			else
				r = *p;
			if ( m_rc <= 1 ) 
				load_buff();
			return r;
		}
		else
			return 0;
	}

	// ---
	char* read_line( int& len ) {
		int i, c;
		char* s = 0;
  
		#define SS (0x100)

		for( i=0; 0 != (c=nc()); i++ ) {
			if ( 0 == (i % SS) ) {
				if ( i ) {
					char* tmp = new char[ ((i / SS)+1)*SS ];
					::memcpy( tmp, s, i );
					delete [] s;
					s = tmp;
				}
				else
					s = new char[SS];
			}
			s[i] = c;
		}
  
		if ( s )
			s[i] = 0;

		len = i;

		for( i=0; i<len; i++ ) // avoid page break;
			if ( s[i] != 014 )
				break;
		if ( i ) 
			::memmove( s, s+i, (len-=i)+1 );

		return s;
	}

	// ---
	bool end() { return m_rc == 0; }

private:
	HANDLE m_h;
	char m_rb[0x1000+1];
	int  m_rp;
	int  m_rc;
};


// ---
HRESULT COutputFile::load( const char* f_name ) {
  HANDLE file = ::CreateFile( f_name, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
  if ( file && (file!=INVALID_HANDLE_VALUE) ) {
    load( file );
    ::CloseHandle( file );
    return S_OK;
  }
  else
    return HRESULT_FROM_WIN32( GetLastError() );
}


// ---
HRESULT COutputFile::load( HANDLE file ) {
  COutputSect* sect = 0;
  char         sect_name[0x100];
  char         sect_sub_name[0x100];
  ItemID       sect_id = empty;
  char*        line;
  uint         line_num = 0;
  int          len;
  bool         skip_empty_line = false;
  int          empty_line = 0;

  RemoveAll();
	CFileLiner liner(file);
  
  while( !liner.end() ) {
    len = 0;
    line = liner.read_line( len );
    line_num++;

    if ( (len == 0) || ((len == 1) && (line[0] == 0)) ) {
      if ( sect )
        sect->Add( line );
      else if ( skip_empty_line ) {
        empty_line++;
        if ( empty_line >= 3 ) {
          empty_line = 0;
          skip_empty_line = false;
        }
      }
      else
        new COutputItem( *this, line, true );
      continue;
    }

    if ( skip_empty_line && empty_line ) {
      skip_empty_line = false;
      empty_line = 0;
    }

    if ( COutputSect::is_delimiter(line,len) ) {
      delete [] line;
      continue;
    }

		if ( sect ) {
			if ( COutputSect::is_sect_end(line) ) {
				sect = 0;
				skip_empty_line = true;
				empty_line = 0;
				continue;
			}
		}
    else if ( COutputSect::is_sect(line,sect_name,sect_sub_name) ) {
      sect = COutputSect::catch_sect( *this, sect_name, sect_sub_name, false );
      continue;
    }

    if ( sect ) {
      if ( sect->is_method_section() || !sect->add_open_brace(line) )
        sect->Add( line );
    }
    else
      new COutputItem( *this, line, true );
  }

  int pr_def = -1, includes = -1;
  uint i=0, c=Count();
  for( ; i<c; i++ ) {
    sect = dynamic_cast<COutputSect*>(operator [] (i));
    if ( sect ) {
      
      if ( (pr_def == -1) && (sect->id() == private_defs) )
        pr_def = i;

      if ( (includes == -1) && (sect->id() == if_includes) )
        includes = i;

      uint k = sect->Count();

      if ( k ) {
        line = (*sect)[0];

        while( k && COutputSect::is_delimiter(line,::lstrlen(line)) ) {
          sect->RemoveInd( 0, Delete );
          line = (*sect)[0];
          k--;
        }

        while( k ) {
          line = (*sect)[--k];
          if ( !line || !*line || COutputSect::is_delimiter(line,::lstrlen(line)) ) 
            sect->RemoveInd( k, Delete );
          else
            break;
        }
        sect->check_name();
      }
    }
  }

  // place private definitions before include section
  if ( (pr_def != -1) && (includes != -1) && (pr_def > includes) ) {
    COutputItem* tmp = (*this)[pr_def];
    (*this)[pr_def] = (*this)[includes];
    (*this)[includes] = tmp;
  }


  return S_OK;
}



// ---

bool COutputFile::flash( HANDLE file, ItemID id, COptions& opt ) {
	int i = 0;
	int c = Count();
	for( ; i<c; i++ ) {
    COutputSect* s = dynamic_cast<COutputSect*>( (*this)[i] );
		if ( !s )
			continue;
		ItemID sid = s->id();
		if ( id == sid ) {
			s->flash( file, opt );
			delete s;
			return true;
		}
	}
	return false;
}




// ---
HRESULT COutputFile::flash( const char* f_name, COptions& o ) {
  HANDLE file = ::CreateFile( f_name, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
  if ( file && (file!=INVALID_HANDLE_VALUE) ) {
    HRESULT h = S_OK;
    int     i, j, c;
  
		// check for include duplicates
		COutputSect* sect = find( 0, h_includes );
		if ( !sect )
			sect = find( 0, if_includes );
		if ( sect ) {
			for( i=0,c=sect->Count(); i<(c-1); i++ ) {
				const char* si = (*sect)[i];
				for( j=i+1; j<c; j++ ) {
					const char* sj = (*sect)[j];
					if ( !::lstrcmp(si,sj) ) {
						sect->RemoveInd( j--, Delete );
						c--;
					}
				}
			}
		}

		if ( flash(file,if_header,o) )
			flash( file, h_if_def, o );

    c = Count();
    for( i=0; SUCCEEDED(h) && i<c; i++ ) 
      h = (*this)[i]->flash( file, o );
    
    ::CloseHandle( file );
    return h;
  }
  else
    return HRESULT_FROM_WIN32( GetLastError() );
}



// ---
void COutputFile::merge( COutputFile& src ) {
  int prev_ind = 0;
	bool caution_happen = false;
	COutputItem* t;
	COutputSect* n;
	COutputSect* o;
	int          i, c;
  for( i=0,c=src.Count(); i<c; i++ ) {
    t = src.RemoveInd( 0, noDelete );
    n = dynamic_cast<COutputSect*>( t );
    if ( n ) {
			if ( caution == n->id() )
				caution_happen = true;
      o = n->find_counterpart( *this, &prev_ind );
      if ( o ) {
          (m_merge->do_merge)( *o, *n );
      }
      else {
        t = 0;
		n->move_to( this, prev_ind++ );
      }
    }
    if ( t )
      delete t;
  }
	for( i=Count()-1; i>=0; i-- ) {
		o = dynamic_cast<COutputSect*>( (*this)[i] );
		if ( o && ( caution==o->id() && !caution_happen || !m_merge->check_put_old(*o) ) )
			delete o;
	}
}

// ---
COutputSect* COutputFile::last_sect() {
  int c = Count();
  COutputSect* s = 0;
  for( c--; !s && c>=0; c-- )
    s = dynamic_cast<COutputSect*>( (*this)[c] );
  return s;
}

// ---
COutputSect* COutputFile::find( int* from, ItemID id ) const {
	int f;
	int c = Count();

	if ( from )
		f = *from;
	else
		f = 0;

	for( ; f<c; f++ ) {
    COutputItem* s = (*this)[f];
    if ( s->id() == id ) {
			if ( from )
				*from = f + 1;
      return dynamic_cast<COutputSect*>( s );
		}
	}
	if ( from )
		*from = c;
	return 0;
}
