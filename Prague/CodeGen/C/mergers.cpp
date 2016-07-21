// OutputFile.cpp: implementation of the COutputFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "codegen.h"
#include "OutputFile.h"
#include <stuff/sarray.h>


//////////////////////////////////////////////////////////////////////

// ---
bool ignore_spaces( const char*& p ) {
  if ( !p )
    return false;
  bool ok = false;
  while( *p && ((*p == ' ') || (*p == '\t')) ) {
    p++;
    ok = true;
  }
  return ok;
}

// ---
bool is_comment( const char* s ) {
  
  if ( !s || !*s ) 
    return false;

  ignore_spaces( s );
  return *s && (*s == '/') && *(s+1) && (*(s+1) == '/' );
}

// ---
bool is_empty( const char* s ) {
  if ( !s || !*s )
    return true;
  ignore_spaces( s );
  return !s || !*s;
}

bool skip_comment( const char*& s ) {
	if ( (*s == SLASH) ) {
		if ( *(s+1) == SLASH ) {
			s += lstrlen(s);
			return true;
		}
		else if ( *(s+1) == STAR ) {
			s += 2;
			while( *s ) {
				if ( (*s != STAR) && (*(s+1)==SLASH) ) {
					s += 2;
					break;
				}
				else
					s++;
			}
			return true;
		}
	}
	return false;
}

// ---
bool get_token( const char*& s, CStr& t ) {

	if ( !s )
		return false;

	while( isspace(*s) )
		s++;
	while( skip_comment(s) )
		;

	switch( *s ) {
		case TERM: 
			return false;
		case STAR      :
		case BRACKET_O : 
		case BRACKET_C :
		case BRACE_O   :
		case COMMA		 : 
		case SEMICOLON :
		case COLON :
			((char*)memcpy( t=new char[2], s++, 1 ))[1] = 0;
			return true;
	}

	const char* b = s;
	while( iscsym(*s) )
		s++;
	if ( s > b ) {
		((char*)memcpy( t=new char[s-b+1], b, s-b ))[s-b] = 0;
		return true;
	}
	else {
		t = 0;
		return false;
	}
}

// ---
void uncomment( char* s ) {
	ignore_spaces( (const char*&)s );
	char* b = s;
	if ( (*((WORD*)s) != MAKEWORD('/','/')) )
		return;
	s += 2;
	if ( *s == '!' ) {
		s++;
		if ( *s == ' ' )
			s++;
	}
	::memmove( b, s, ::lstrlen(s)+1 );
}

// ---
void comment_out( char*& s ) {
  if ( !is_comment(s) ) {
    int l = ((s && *s) ? ::lstrlen(s) : 0) + 1;
    char* n = new char[ l+4 ];
    *(DWORD*)n = MAKELONG( MAKEWORD('/','/'), MAKEWORD('!',' ') );
    if ( s && *s ) {
      ::memcpy( n+4, s, l );
      delete [] s;
    }
		else
			*(n+4) = 0;
    s = n;
  }
}

// ---
static int move( CPArray<char>& to, CPArray<char>& from, int c ) {
	int i;
	for( i=0; i<c; i++ ) {
		char* os = from.RemoveInd( 0, noDelete );
		comment_out( os );
		to.Add( os );
	}
	return i;
}

//////////////////////////////////////////////////////////////////////

// ---
bool cmp_by_substr_ignore_comment( const char* s1, const char* s2 ) {
	
  if ( !s1 || !*s1 ) 
    return !s2 || !*s2;
  else if ( !s2 || !*s2 )
    return false;
	
  return !!::strstr( s1, s2 );
}

// ---
bool cmp_str( const char* s1, const char* s2 ) {
  if ( !s1 || !*s1 ) 
    return !s2 || !*s2;
  else if ( !s2 || !*s2 )
    return false;
  else
    return !::_stricmp( s1, s2 );
}

// ---
bool cmp_ignore_spaces_no_comment( const char* s1, const char* s2 ) {

  if ( !s1 || !*s1 ) 
    return !s2 || !*s2;
  else if ( !s2 || !*s2 )
    return false;

  ignore_spaces( s1 );
  ignore_spaces( s2 );
  while( *s1 && (*s1 == *s2) && !is_comment(s1) && !is_comment(s2) ) {
    ignore_spaces( ++s1 );
    ignore_spaces( ++s2 );
  }
  return ((*s1 == 0) || (*s1 == '/')) && ((*s2 == 0) || (*s2 == '/'));
}

// ---
bool cmp_function( const char* s1, const char* s2 ) {
	
	CStr t1;
	CStr t2;
	bool b1;
	bool b2;
	bool complete = true;
	bool call = true;
	enum { it_type, it_name, it_comma } counter = it_type;
  
	b1 = get_token( s1, t1 );
	if ( !::lstrcmp(t1,"static") )
		b1 = get_token( s1, t1 );
	b2 = get_token( s2, t2 );
	if ( !::lstrcmp(t2,"static") )
		b2 = get_token( s2, t2 );
	
	while( 1 ) {
		if ( b1 != b2 )
			return false;
		if ( !b1 )
			return true;
		if ( counter == it_name )
			;
		else if ( lstrcmp(t1,t2) )
			return false;
		if ( complete ) {
			if ( *t1 == BRACKET_O ) 
			{
				if (*s1 == BRACKET_C) // has no parameters
				{
					if (*s2 == BRACKET_C)
						return true;
					return false;
				}
				complete = false;								  // switch to parameter mode
			}
		}
		else {
			switch( counter ) {
				case it_type:
					if ( call && !lstrcmp(t1,"pr_call") ) {
						call = false;
						complete = true;							// switch to complete mode
					}
					else if ( !lstrcmp(t1,"const") )
						; // do not switch to "it_name: mode
					else
						counter = it_name;
					break;
				case it_name:
					if ( ((*t1 == STAR) || (*t2 == STAR)) ) {
						if ( *t1 != *t2 )
							return false;
					}
					else
						counter = it_comma;
					break;
				case it_comma:
					counter = it_type;
					if ( *t1 == BRACKET_C )				 // we got it !!
						return true;								 
					break;
			}
		}
		b1 = get_token( s1, t1 );
		b2 = get_token( s2, t2 );
	}
}

// ---
bool cmp_range( const COutputSect& s1, int p1, const COutputSect& s2, int p2, int len ) {
	int i;
	if ( p1 == -1 ) {
		if ( p2 != -1 )
			return false;
		else
			return true;
	}
	else if ( p2 == -1 )
		return false;
	if ( len <= 0 )
		return false;
	int c1 = s1.Count();
	if ( (p1+len) > c1 )
		return false;
	int c2 = s2.Count();
	if ( (p2+len) > c2 )
		return false;
	for( i=0; i<len; i++ ) {
		const char* str1 = s1[p1+i];
		const char* str2 = s2[p2+i];
		if ( !cmp_ignore_spaces_no_comment(str1,str2) )
			return false;
	}
	return true;
}

// ---
bool cmp_ignore_spaces( const char* s1, const char* s2 ) {
  
  if ( !s1 || !*s1 ) 
    return !s2 || !*s2;
  else if ( !s2 || !*s2 )
    return false;

  ignore_spaces( s1 );
  ignore_spaces( s2 );
  while( *s1 && (*s1 == *s2) ) {
    ignore_spaces( ++s1 );
    ignore_spaces( ++s2 );
  }
  return (*s1 == 0) && (*s2 == 0);
}


// ---
bool cmp_by_substr( const char* s1, const char* s2 ) {

  if ( !s1 || !*s1 ) 
    return !s2 || !*s2;
  else if ( !s2 || !*s2 )
    return false;

  return !is_comment(s1) && ::strstr( s1, s2 );
}

//////////////////////////////////////////////////////////////////////

static char g_moved[] = "//! Section moved to correspondent \"plugin_*.c\" or \"plugin_*.h\" file";

// ---
void CCodeGen::m_pl_def( COutputSect& old, COutputSect& anew ) {
  int i, c;
	
	if ( !m_plugin_mode ) {
		i = old.find_by( 0, g_moved, cmp_ignore_spaces );
		if ( -1 == i ) {
			for( i=0,c=old.Count(); i<c; i++ )
				comment_out( old[i] );
			old.Insert( 0, (char*)::memcpy(new char[sizeof(g_moved)], g_moved, sizeof(g_moved)) );
		}
		return;
	}

  // find all ..._Registar calls
  // if some new calls are not present in old -- add their at the end
  // if some old calls are not present in new -- comment them out
  // if all calls are matched -- just relax

  CSArray<int> new_has; // new section has unmatched register calls (need to add)

  {
	int i, c;
	for( i=0,c=m_iface_names.Count(); i<c; i++ ) {
		::lstrcpy( W3, m_iface_names[i] );
		::lstrcat( W3, "_Register" );
		int pos = old.find_by( 0, W3, cmp_by_substr );
		if ( -1 == pos )
			new_has.Add( i );
	}
  }

  c = new_has.Count();
  if ( c ) {
    // find the end of an old procedure
    int end;
    int count = old.Count();
    int cb_pos = 0; // close bracket line position
    for( end=count-1; end>=0; end-- ) {
      if ( !cb_pos ) {
        if ( cmp_ignore_spaces(old[end],"case PRAGUE_PLUGIN_UNLOAD :") )
          cb_pos = end;
      }
      else if ( cmp_by_substr(old[end],"break;") ) { // founded second close bracket w/o return
        end++;
        break;
      }
      else if ( cmp_by_substr(old[end],"return") ) {
        end++;
        break;
			}
    }

    for( i=0; i<c; i++ )
      end = GenPluginRegCall( old, m_iface_names[new_has[i]], end );
  }
}


// ---
void CCodeGen::m_pl_or_ven_id( COutputSect& old, COutputSect& anew ) {
	int i;
	if ( m_plugin_mode ) {
		CStr token;
		char* str = anew.RemoveInd( 0, noDelete );
		i = old.find_by( 0, str, cmp_ignore_spaces );
		if ( -1 != i ) 
			delete [] str;
		else if ( !get_token((const char*&)str,token) || !get_token((const char*&)str,token) ) 
			old.Add( str );
		else if ( -1 == (i=old.find_by(0,token,cmp_by_substr_ignore_comment)) )
			old.Add( str );
		else if ( is_comment(old[i]) ) {
			uncomment( old[i] );
			delete [] str;
		}
		else
			delete [] str;
	}
	else {
		i = old.find_by( 0, g_moved, cmp_ignore_spaces );
		if ( -1 == i ) {
			old.comment_out();
			old.Insert( 0, (char*)::memcpy(new char[sizeof(g_moved)], g_moved, sizeof(g_moved)) );
		}
		return;
	}
}


// ---
void CCodeGen::m_replace( COutputSect& old, COutputSect& anew ) {
  int j, nc, oc;
	CPArray<char> result;

	j = old.find_by( 0, "// PLEASE REGENERATE HEADER FILE FOR THIS IMPLEMENTATION", cmp_ignore_spaces );
	if ( -1 != j )
		old.RemoveInd( j, Delete );
	
	j = old.find_by( 0, "SECTION TRANSFERRED TO CORRESPONDING HEADER FILE", cmp_by_substr_ignore_comment );
	if ( -1 != j )
		old.RemoveInd( j, Delete );

	oc = old.Count();
  nc = anew.Count();
	for( int i=0; i<nc; i++ ) {
		cmp_func fnc;
		char* s = anew.RemoveInd( 0, noDelete ); 
		if ( is_empty(s) ) {
			if ( oc && is_empty(old[0]) ) {
				oc--;
				old.RemoveInd( 0 );
			}
		}
		else {
			if ( is_comment(s) )
				fnc = cmp_str;
			else
				fnc = cmp_ignore_spaces_no_comment;
			j = old.find_by( 0, s, fnc );
			if ( j != -1 ) {
				if ( j )
					oc -= move( result, old, j );
				result.Add( old.RemoveInd(0,noDelete) );
				oc--;
				delete [] s;
				s = 0;
			}
		}
		if ( s )
			result.Add( s );
	}
	if ( oc )
		oc -= move( result, old, oc );
	old.EatAway( result );
}


// ---
void CCodeGen::m_method( COutputSect& old, COutputSect& anew ) {
  bool done = false;
  const char* new_name;
  const char* old_name;
  int nh, oh;

	nh = anew.find_first_not_commented_line(); // method headline
	if ( nh == -1 ) { // what does it mean ?!
		anew.comment_out();
		old.EatAway( anew );
		return;
	}
	else
		new_name = anew[nh];
	
	oh = old.find_first_not_commented_line(); // method headline
	if ( oh == -1 ) {
		old.comment_out();
		old.EatAway( anew );
		return;
	}
	else
		old_name = old[oh];

	if ( !cmp_function(old_name,new_name) ) { // so, make a new headline string
		const char* old_close_brace = strrchr( old_name, BRACKET_C );
		const char* new_close_brace = strrchr( new_name, BRACKET_C );
		char* composed_name;
		if ( old_close_brace && new_close_brace ) {
			int nl = new_close_brace - new_name; // take from a new name symbols till closed brace
			int ol = lstrlen( old_close_brace ); // all the rest symbols
			composed_name = new char[ nl + ol + 1 ];
			memcpy( composed_name, new_name, nl );
			memcpy( composed_name+nl, old_close_brace, ol+1 );
		}
		else
			composed_name = anew.RemoveInd( nh, noDelete );
		comment_out( old[oh] );
		old.AddAfter( composed_name, oh );
	}
	anew.RemoveAll( Delete );
		
}


// ---
void CCodeGen::m_reg_call( COutputSect& old, COutputSect& anew ) {
  int i;
  int beg  = old.find_by( 0, "RegisterIFace", cmp_by_substr ); // find reistration call range (CALL_Root_RegisterIFace)
  int begN = anew.find_by( 0, "RegisterIFace", cmp_by_substr ); // find reistration call range (CALL_Root_RegisterIFace)
	
  if ( beg >= 0 ) { // if call range found
    
    bool the_same;
    int  end;
		int  len;
		int  endN = anew.find_by( begN, BRACKET_C_SEMICOLON, cmp_ignore_spaces_no_comment );
		int  lenN = endN - begN + 1;
		
    if ( ::strrchr(old[beg],BRACKET_C) ) {
      end = beg;
      the_same = false;
    }
    else {
      end = old.find_by( beg, BRACKET_C_SEMICOLON, cmp_ignore_spaces_no_comment );
      the_same = true;
    }
    
		len = end - beg + 1;
		the_same = cmp_range( old, beg, anew, begN, lenN );

    if ( the_same ) { 
      // if the same -- remove new one to save comments
      for( i=endN; i>=begN; i-- )
        anew.RemoveInd( i );
    }
    else { // if not the same -- comment out previous one
			old.comment_out( beg, len );
			for( i=0; i<lenN; i++ )
				old.AddAfter( anew.RemoveInd(begN,noDelete), end++ );
		}
	}
	anew.RemoveAll();

  // find all PR_TRACE... or PR_K_TRACE calls and comment it out if they not found in new section
	// i = -1;
	// while( -1 != (i=find_by(old,i+1,g_trace_def,cmp_by_prefix)) ) {
	//   char*& s = old[i];
	//   if ( -1 == find_by(anew,0,s,cmp_ignore_spaces_and_comment_no_case) )
	//     comment_out( s );
	// }
	//
	// m_merge( old, anew );
}

// ---
void CCodeGen::m_move( COutputSect& old, COutputSect& anew ) {
  old.RemoveAll( Delete );
  old.EatAway( anew );
}


// ---
void CCodeGen::m_drop( COutputSect& old, COutputSect& anew ) {
  anew.RemoveAll( Delete );
}

//////////////////////////////////////////////////////////////////////

void CCodeGen::do_merge( COutputSect& old, COutputSect& anew )
{
	switch( old.id() )
	{
	case pl_def : m_pl_def(old, anew); break;
	case pl_id:
	case ven_id:  m_pl_or_ven_id(old, anew); break;
	case if_includes:
	case data_str: m_replace(old, anew); break;
	case pl_ex_impl:
	case pr_meth_impl:
	case ex_meth_impl:
	case in_meth_impl: m_method(old, anew); break;
	case reg_call: m_reg_call(old, anew); break;
	case h_if_def: m_move(old,anew); /*m_drop(old, anew);*/ break;
	default: m_move(old, anew);
	}
}

bool CCodeGen::check_put_old(COutputSect &sect)
{
	if( sect.id() == private_defs && m_prototype )
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////




/*
void advance( const char*& p, char* bracket );
bool skip_comment( const char*& s );
bool get_token( const char*& s, CStr& t );
int  get_word( const char* str, CStr* word, const char** next );
void uncomment( char* s );
void prefix_line( char*& s, const char* prefix );
const char* find_whole_word( const char* s1, const char* word );


bool cmp_method_name( const char* n1, const char* n2 );
bool cmp_ignore_spaces_w_braces( const char* s1, const char* s2 );
bool cmp_ignore_spaces_and_comment( const char* s1, const char* s2 );
bool cmp_ignore_spaces_no_comment( const char* s1, const char* s2 );
bool cmp_ignore_spaces_and_comment( const char* s1, const char* s2 );
bool cmp_ignore_spaces_and_comment_no_case( const char* s1, const char* s2 );	
bool cmp_str( const char* s1, const char* s2 );

bool cmp_by_prefix( const char* s1, const char* s2 );
bool cmp_prefix_ignore_spaces( const char* s1, const char* s2 );
bool cmp_by_prefix_ignore_spaces_and_advance( const char*& s1, const char* s2 );

bool cmp_by_substr_ignore_comment( const char* s1, const char* s2 );
bool cmp_function( const char* s1, const char* s2 );
*/


/*
// ---
void advance( const char*& p, char* bracket ) {
  if ( p ) {
    p++;
    if ( bracket && *bracket ) {
      if ( *p == *bracket )
        *bracket = 0;
    }
    else {
      ignore_spaces( p );
      if ( bracket ) {
        if ( *p == '"' )
          *bracket = '"';
        else if ( *p == '<' )
          *bracket = '>';
      }
    }
  }
}


// ---
bool is_method( const char* s ) {
	CStr t;
	bool b = get_token( s, t );
	if ( !b )
		return false;
	if ( !::lstrcmp(t,"typedef") )
		return true;
	if ( !::lstrcmp(t,"static") && !get_token(s,t) )
		return false;
	if ( !::lstrcmp(t,"tERROR") )
		return true;
	return false;
}

// ---
const char* find_whole_word( const char* s1, const char* s2 ) {
	
  if ( !s1 || !*s1 ) 
    return 0;
  else if ( !s2 || !*s2 )
    return 0;
	
	int l = strlen( s2 );
	const char* w = ::strstr( s1, s2 );
	while( w ) {
		if ( ((w == s1) || !__iscsym(*(w-1))) && !__iscsym(*(w+l)) )
			return w;
		s1 = w+l;
		w = ::strstr( s1, s2 );
	}
  return 0;
}

// ---
int get_word( const char* str, CStr* word, const char** next ) {
	while( *str && !__iscsym(*str) )
		str++;
	const char* end = str;
	while( *end && __iscsym(*end) )
		end++;
	if ( next ) {
		const char* the_end = end;
		while( *the_end && !__iscsym(*the_end) )
			the_end++;
		*next = the_end;
	}
	if ( word ) {
		if ( end > str )
			((char*)memcpy( *word= new char[end-str+1], str, end - str ))[end - str] = 0;
		else
			*word = 0;
	}
	return end - str;
}


// ---
void prefix_line( char*& s, const char* prefix ) {
  int pr_len = ::lstrlen( prefix );
  int l = (s && *s) ? ::lstrlen(s) : 0;
  char* n = new char[ l+pr_len+1 ];
  ::memcpy( n, prefix, pr_len+1 );
  if ( s && *s ) {
    ::memcpy( n+pr_len, s, l+1 );
    delete [] s;
  }
  s = n;
}

// ---
static int extract_word( const char* str, const char* sub_word, char* word_found ) {
  int end;
  const char* f = ::strstr( str, sub_word );
  if ( f ) {
    int len = ::lstrlen( sub_word );
    int beg = f - str;
    end = beg + len;
    while( (beg > 0) && __iscsym(*(str+beg-1)) )
      beg--;
    while( *(str+end) && __iscsym(*(str+end)) )
      end++;
    end -= beg;
    if ( end )
      ::memcpy( word_found, str+beg, end );
  }
  else
    end = 0;
  *(word_found + end) = 0;
  return end;
}

// ---
static bool is_prop( const char* s, char* p ) {
  ignore_spaces( s );

  if ( !::strncmp(s,"mSHARED_PROPERTY",16) )
    s += 16;
  else if ( !::strncmp(s,"mLOCAL_PROPERTY",15) )
    s += 15;
  else if ( !::strncmp(s,"mLAST_CALL_PROPERTY",19) ) {
    ::lstrcpy( p, "last_call" );
    return true;
  }
  else
    return false;

  while( *s && (*s != BRACKET_O) )
    s++;

  if ( *s ) {
    s++;
    while( *s && (*s == ' ') )
      s++;
    if ( p ) {
      while( __iscsym(*s) )
        *p++ = *s++;
      *p = 0;
    }
    return true;
  }
  else
    return false;

}

// ---
static bool find_prop_table_beg( const char* s1, const char* s2 ) {
  if ( !s1 || !*s1 || is_comment(s1) || !find_whole_word(s1,"mPROPERTY_TABLE") ) 
    return false;
  else
    return true;
}



// ---
static bool find_prop_table_end( const char* s1, const char* s2 ) {
  if ( !s1 || !*s1 || is_comment(s1) || !find_whole_word(s1,"mPROPERTY_TABLE_END") ) 
    return false;
  else
    return true;
}

// ---
inline bool cmp_include_symbol( char c1, char c2 ) {
  return (c1 == c2) || (((c1 == '/') || (c1 == '\\')) && ((c2 == '/') || (c2 == '\\')));
}

// ---
bool cmp_ignore_spaces_w_braces( const char* s1, const char* s2 ) {
  
  if ( !s1 || !*s1 ) 
    return !s2 || !*s2;
  else if ( !s2 || !*s2 )
    return false;

  char bs1 = 0, bs2 = 0;
  while( *s1 && *s2 && cmp_include_symbol(*s1,*s2) ) {
    advance( s1, &bs1 );
    advance( s2, &bs2 );
  }
  return (*s1 == 0) && (*s2 == 0);
}



// ---
bool cmp_prefix_ignore_spaces( const char* s1, const char* s2 ) {
  
  if ( !s1 || !*s1 ) 
    return !s2 || !*s2;
  else if ( !s2 || !*s2 )
    return false;

  ignore_spaces( s1 );
  ignore_spaces( s2 );
  while( *s1 && (*s1 == *s2) ) {
    ignore_spaces( ++s1 );
    ignore_spaces( ++s2 );
  }
  return *s2 == 0;
}



// ---
bool cmp_by_prefix_ignore_spaces_and_advance( const char*& s1, const char* s2 ) {
  
	const char* p = s1;
  if ( !p || !*p ) 
    return !s2 || !*s2;
  else if ( !s2 || !*s2 )
    return false;
	
  ignore_spaces( p );
  ignore_spaces( s2 );
  while( *p && (*p == *s2) ) {
    ignore_spaces( ++p );
    ignore_spaces( ++s2 );
  }
	if ( *s2 == 0 ) {
		s1 = p;
		return true;
	}
	else
		return false;
}



// ---
bool cmp_ignore_spaces_and_comment( const char* s1, const char* s2 ) {
	
  if ( !s1 || !*s1 ) 
    return !s2 || !*s2;
  else if ( !s2 || !*s2 )
    return false;
	
	bool nis1 = true;
	bool nis2 = true;

  ignore_spaces( s1 );
  ignore_spaces( s2 );
	if ( *((WORD*)s1) == MAKEWORD('/','/') ) {
		nis1 = false;
		s1 += 2;
		if ( *s1 == '!' )
			s1++;
		ignore_spaces( s1 );
	}
	if ( *((WORD*)s2) == MAKEWORD('/','/') ) {
		nis2 = false;
		s2 += 2;
		if ( *s2 == '!' )
			s2++;
		ignore_spaces( s2 );
	}
	
  while( *s1 && (*s1 == *s2) ) {
    s1++; 
		ignore_spaces( s1 );
		if ( nis1 && (*((WORD*)s1) == MAKEWORD('/','/')) ) {
			nis1 = false;
			s1 += 2;
			if ( *s1 == '!' )
				s1++;
			ignore_spaces( s1 );
		}
    s2++; 
		ignore_spaces( s2 );
		if ( nis2 && (*((WORD*)s2) == MAKEWORD('/','/')) ) {
			nis2 = false;
			s2 += 2;
			if ( *s2 == '!' )
				s2++;
			ignore_spaces( s2 );
		}
  }
  return (*s1 == 0) && (*s2 == 0);
}


// ---
bool cmp_ignore_spaces_and_comment_no_case( const char* s1, const char* s2 ) {

  if ( !s1 || !*s1 ) 
    return !s2 || !*s2;
  else if ( !s2 || !*s2 )
    return false;

  ignore_spaces( s1 );
  ignore_spaces( s2 );
  while( *s1 ) {
    char c1 = tolower( *s1 );
    char c2 = tolower( *s2 );
    
    if ( (c1 != c2) || is_comment(s1) || is_comment(s2) ) 
      break;

    ignore_spaces( ++s1 );
    ignore_spaces( ++s2 );
  }
  return (tolower(*s1) == tolower(*s2)) || (is_comment(s1) && is_comment(s2));
}



// ---
bool cmp_by_prefix( const char* s, const char* p ) {

  if ( !s || !*s ) 
    return !p || !*p;
  else if ( !p || !*p )
    return false;

  ignore_spaces( s );
  ignore_spaces( p );

  if ( is_comment(p) ) {
    if ( is_comment(s) ) {
      s += 2;
      p += 2;
      ignore_spaces( s );
      ignore_spaces( p );
    }
    else
      return false;
  }

  int pl = ::lstrlen( p );
  if ( pl > ::lstrlen(s) ) // + " -- "
    return false;
  
  return !::strnicmp( s, p, pl );
}



// ---
bool cmp_method_name( const char* n1, const char* n2 ) {
  ignore_spaces( n1 );
  ignore_spaces( n2 );
  return cmp_ignore_spaces( n1, n2 );
}


// ---
static bool cmp_by_prop_id( const char* s1, const char* s2 ) {
  if ( !s1 || !*s1 ) 
    return !s2 || !*s2;
  else if ( !s2 || !*s2 )
    return false;
  else {
    char prop_id[MAX_PATH];
    return is_prop(s1,prop_id) && !::lstrcmp(prop_id,s2);
  }
}


// ---
static bool cmp_by_pid( const char* s1, const char* s2 ) {

	if ( !s1 || !*s1 )
		return false;

	CStr word;
	const char* next;
	if ( is_comment(s1) )
		return false;
	else if ( !get_word(s1,0,&next) )
		return false;
	else if ( !get_word(next,&word,0) )
		return false;
	else if ( lstrcmp(word,s2) )
		return false;
	else
		return true;

}

//////////////////////////////////////////////////////////////////////

// ---
void CCodeGen::m_merge( COutputSect& old, COutputSect& anew ) {
  int i, c = anew.Count();
  if ( c ) {
    bool      tr = old.is_impl() || (old.id() == reg_call);
    int       cp = 0;
    char*     sp = anew.RemoveInd( 0, noDelete );
    char*     sn = sp;
    cmp_func  cf = (tr && cmp_by_prefix(sp,g_trace_def)) ? cmp_ignore_spaces_and_comment_no_case : cmp_ignore_spaces_no_comment;
    int       op = old.find_by( 0, sp, cf );
    int       on = op;

    for( i=0, c--; i<c; i++ ) {
      sn = anew.RemoveInd( 0, noDelete );
      cf = (tr && cmp_by_prefix(sn,g_trace_def)) ? cmp_ignore_spaces_and_comment_no_case : cmp_ignore_spaces_no_comment;
      on = old.find_by( cp, sn, cf );
      if ( op >= 0 ) {
        if ( (on == -1) || (on >= op) ) {
          if ( (op+1) > cp )
            cp = op + 1;
          else
            cp++;
          delete [] sp;
        }
        else {
          comment_out( old[op] );
          if ( on >= 0 )
            cp = on;
          old.Insert( cp++, sp );
        }
      }
      else {
        if ( on >= 0 )
          cp = on;
        old.Insert( cp++, sp );
      }
      op = on;
      sp = sn;
    }
    if ( on >= cp )
      delete [] sn;
    else
      old.Insert( cp, sn );
  }
  else {
    for( i=0, c--; i<c; i++ ) {
      char*& s = old[i];
      if ( !is_comment(s) ) 
        comment_out( s );
    }
  }
}



// ---
void CCodeGen::m_meth_template( COutputSect& old, COutputSect& anew, const char* tmpl ) {
  int c = anew.Count();
  for( int i=c-1; i>=0; i-- ) {
		int j;
		CStr  word;
    char* s = anew[i];
		char* name = s;

		if ( is_empty(s) )
			continue;
		
		else if ( is_comment(s) ) {
			int oc = old.Count();
			if ( oc ) {
				while( !isalpha(*s) )
					advance( s, 0 );
				char* e = s;
				while( isalnum(*e) || isspace(*e) )
					advance( e, 0 );
				while( *(e-1) == ' ' )
					e--;
				if ( e > s ) {
					char* cont = (char*)memcpy( new char[e-s+1], s, e-s );
					cont[e-s] = 0;

					for( j=0; j<oc; j++ ) {
						const char* line = old[j];
						if ( is_comment(line) && ::strstr(line,cont) ) {
							old.RemoveInd( j, Delete );
							break;
						}
					}
					delete [] cont;
				}
			}
			continue;
		}
		else if ( is_method(s) )
			j = old.find_by( 0, s, cmp_function );
		else 
			j = old.find_by( 0, s, cmp_ignore_spaces_no_comment ); // cmp_str

		if ( j != -1 ) {
			delete [] s;
			s = old.RemoveInd( j, noDelete );
			anew[i] = s;
		}
		
		else if ( (!tmpl || cmp_by_prefix_ignore_spaces_and_advance(name,tmpl)) && get_word(name,&word,0) ) {
			int j = old.find_by( 0, word, cmp_by_substr );
			if ( j != -1 ) {
				s = old.RemoveInd( j, noDelete );
				comment_out( s );
				anew.Insert( i, s );
			}
		}
  }

	for( c=old.Count(); i>0; c-- ) {
		char* s = old.RemoveInd( c-1, noDelete );
    if ( !s )
      ;
    else if ( is_empty(s) )
      delete [] s;
    else {
      if ( !is_comment(s) )
        comment_out( s );
      anew.Insert( 0, s );
    }
	}

	old.EatAway( anew );
}



// ---
void CCodeGen::m_meth_forw_decl( COutputSect& old, COutputSect& anew ) {
	m_meth_template( old, anew, "typedef tERROR (pr_call *" );
}



// ---
void CCodeGen::m_meth_ex_prt( COutputSect& old, COutputSect& anew ) {
	m_meth_template( old, anew, "tERROR pr_call " );
}




// ---
void CCodeGen::m_meth_table( COutputSect& old, COutputSect& anew ) {
	m_meth_template( old, anew, 0 );
}



// ---
void CCodeGen::m_if_header( COutputSect& old, COutputSect& anew ) {

// ---
static const char* exclude_strs[] = {
	"// -------------------------------------------",
	"// Copyright (c) Kaspersky Labs. ",
};

  int   i;
	int   oi, nc;
	char* p;
  uint  oc = old.Count();
	
  if ( oc > 1 ) 
    old.RemoveInd( 0 );
  old.Insert( 0, anew.RemoveInd(0,noDelete) );
	
	for( i=0; i<countof(exclude_strs); i++ ) {
		while( -1 != (oi=old.find_by(0,exclude_strs[i],cmp_by_substr_ignore_comment)) )
			old.RemoveInd( oi );
	}

  for( i=0, nc=anew.Count(); (i<3) && (i<nc); i++ ) {
		p = anew.RemoveInd( 0, noDelete );
    oi = old.find_by( i+1, p, cmp_str );
    if ( oi > 0 ) 
			delete [] p;
    else
      old.Insert( i+1, p );
  }

  for( i=0, nc=anew.Count(); (i<5) && (i<nc); i++ ) {
    char prf[50];
    char* s = p = anew.RemoveInd( 0, noDelete );
    for( int j=0; (*s != '-') && j<(sizeof(prf)-1); j++, s++ )
      prf[j] = *s;
    for( prf[j--]=0; prf[j] == ' '; j-- );
			prf[++j] = 0;
		
		oi = old.find_by( 0, prf, cmp_by_prefix );
		if ( -1 == oi )
			oi = i+4;
		else if ( cmp_str(p,old[oi]) ) {
			delete [] p;
			p = 0;
		}
		else
			old.RemoveInd( oi );
		if ( p )
			old.Insert( oi, p );
  }

  for( i=0, nc=anew.Count(); i<nc; i++ ) {
		p = anew.RemoveInd( 0, noDelete );
    oi = old.find_by( i+1, p, cmp_ignore_spaces_and_comment );
    if ( oi > 0 ) 
			delete [] p;
    else
      old.Add( p );
  }

}



// ---
void CCodeGen::m_if_includes( COutputSect& old, COutputSect& anew ) {
  uint oc = old.Count();
  for( uint c=anew.Count(); c; ) {
		char* s = anew.RemoveInd( --c, noDelete );
    int oi = old.find_by( 0, s, cmp_ignore_spaces_w_braces );
    if ( oi >= 0 )
			delete [] s;
    else
      old.Insert( oc+1, s ); 
  }
}



// ---
void CCodeGen::m_if_comment( COutputSect& old, COutputSect& anew ) {
  for( uint i=0, op=0, c=anew.Count(); i<c; i++ ) {
    char* s = anew.RemoveInd( 0, noDelete );
    int oi = old.find_by( i, s, cmp_ignore_spaces );
    if ( oi >= 0 ) {
      op = oi;
      delete [] s;
    }
    else
      old.Insert( op++, s );
  }
}



// ---
void CCodeGen::m_priv_defs( COutputSect& old, COutputSect& anew ) {
  int oi = old.find_by( 0, anew[0], cmp_ignore_spaces );
  if ( oi >= 0 )
    anew.RemoveInd( 0 );
  else if ( (0<=(oi=old.find_by(0,"_PRIVATE_DEFINITION",cmp_by_substr))) ) {
    old.RemoveInd( oi );
    old.Insert( oi, anew.RemoveInd(0,noDelete) );
  }
  else
    old.Insert( 0, anew.RemoveInd(0,noDelete) );
}



typedef enum tag_otype { o_ins, o_add, o_adda } otype;

// ---
void to_old( COutputSect& old, int f, otype type, char* ns, int& co, unsigned char* indexes, int c ) {
	if ( type == o_ins ) {
		old.Insert( f, ns );
		f--;
	}
	else if ( type == o_add ) {
		old.Add( ns );
		f = co - 1;
	}
	else {
		if ( !co )
			f = -1;
		old.AddAfter( ns, f );										   // insert a new line after old one
	}
	co++;
	if ( f+2 < c )
		memmove( indexes+f+2, indexes+f+1, c-f-2 );	 // free space for new line comment marker
	indexes[f+1] = 0;
}


	
// ---
void CCodeGen::m_pr_table( COutputSect& old, COutputSect& anew ) {
  
  int i, j, c, f, tbn, ten=-1, cn, co, tbo=-1, teo=-1;
	char* ns;

	cn = anew.Count();
	co = old.Count();

	c = cn + co;
	unsigned char* indexes = new unsigned char[2*c+100];
	memset( indexes, 1, 2*c+100 );

	// проверим есть ли в старой таблице комментарии сначала
	if ( (-1 != (f=old.find_by(0,0,find_prop_table_beg))) && (f > 0) && is_comment(old[0]) ) {
		for( j=0; (j<cn) && is_comment(anew[0]); j++ )
			anew.RemoveInd( 0, Delete );
	}
	else {				 // нет! тогда наш коммент добавим в старую
		for( j=0; (j<cn) && is_comment(anew[0]); j++ )
			to_old( old, j, o_ins, anew.RemoveInd(0,noDelete), co, indexes, c );
	}
	cn -= j;
  
	tbn = anew.find_by( 0, 0, find_prop_table_beg );
	if ( -1 != tbn ) {
		tbo = old.find_by( 0, 0, find_prop_table_beg );
		ns = anew.RemoveInd( tbn, noDelete );
		if ( -1 == tbo )
			to_old( old, 0, o_ins, ns, co, indexes, c );
		else if ( cmp_ignore_spaces(ns,old[tbo]) ) {
			indexes[tbo] = 0;
			delete [] ns;
		}
		else
			to_old( old, tbo, o_adda, ns, co, indexes, c );

		ten = anew.find_by( 0, 0, find_prop_table_end );
		if ( -1 != ten ) {
			teo = old.find_by( 0, 0, find_prop_table_end );
			ns = anew.RemoveInd( ten, noDelete );
			if ( -1 == teo )
				to_old( old, co, o_add, ns, co, indexes, c );
			else if ( cmp_ignore_spaces(ns,old[teo]) ) {
				indexes[teo] = 0;
				delete [] ns;
			}
			else
				to_old( old, teo, o_adda, ns, co, indexes, c );
		}
	}
	
	if ( teo > 0 )
		co = teo - 1;

	// найти в старой по prop_id, 
	// а потом сравнить обе строки, 
	// если одинаковые, то оставить старую,
	// если разные, старую закоментарить, а новую добавить после старой
	// после этого все старые незаюзанные закоментарить
	for( i=anew.Count()-1; i>=0; i-- ) {
		CStr word;
		const char* next;

		ns = anew.RemoveInd( i, noDelete );
		if ( is_comment(ns) )
			continue;

		if ( get_word(ns,0,&next) && get_word(next,&word,0) ) {
			f = old.find_by( 0, word, cmp_by_pid ); // find it by prop id
			if ( -1 == f ) {
				f = old.find_by( 0, ns, cmp_ignore_spaces_and_comment );
				if ( -1 != f ) {
					uncomment( old[f] );
					indexes[f] = 0;
					delete [] ns;
				}
				else
					to_old( old, co-1, o_adda, ns, co, indexes, c );
			}
			else if ( cmp_ignore_spaces(ns,old[f]) ) {
				indexes[f] = 0;
				delete [] ns;															 // the same
			}
			else if ( -1 != (j=old.find_by(0,ns,cmp_ignore_spaces_and_comment)) ) {
				uncomment( old[j] );
				indexes[j] = 0;
				delete [] ns;															 // the same but commented out
			}
			else
				to_old( old, f, o_adda, ns, co, indexes, c );
		}
		else if ( -1 != (f=old.find_by(0,ns,cmp_ignore_spaces)) ) {
			indexes[f] = 0;
			delete [] ns;
		}
	}

	co = old.Count();
	tbo = old.find_by( 0, 0, find_prop_table_beg );
	teo = old.find_by( 0, 0, find_prop_table_end );
	for( i=0; i<co; i++ ) {
		char*& s = old[i];
		if ( indexes[i] && (i != tbo) && (i != teo) && !is_comment(s) )
			comment_out( s );
	}
	delete [] indexes;
}


// ---
void CCodeGen::m_remove_cpp( COutputSect& old, COutputSect& anew ) {
	int c = old.Count();
	if ( !c ) {
		old.EatAway( anew );
		return;
	}

	int i = old.find_by( 0, "#if defined( __cplusplus )", cmp_ignore_spaces );
	if ( 
		(i != -1) && 
		((i+2) < c) && 
		cmp_ignore_spaces(old[i+1],"extern \"C\" "BRACE_OS) && 
		cmp_ignore_spaces(old[i+2],"#endif") 
	) {
		comment_out( old[i+0] );
		comment_out( old[i+1] );
		comment_out( old[i+2] );
	}

	i = old.find_by( i, "#if defined( __cplusplus )", cmp_ignore_spaces );
	if ( 
		(i != -1) && 
		((i+2) < c) && 
		cmp_ignore_spaces(old[i+1],BRACE_CS) && 
		cmp_ignore_spaces(old[i+2],"#endif") 
	) {
		comment_out( old[i+0] );
		comment_out( old[i+1] );
		comment_out( old[i+2] );
	}
	m_replace( old, anew );
}


// ---
static int fdecl( COutputSect& s, int start, CStr* token ) {
	
	if ( start )
		start++;

	int i = s.find_by( start, "#define", cmp_by_prefix );
	if ( i != -1 ) {
		const char* str = s[i] + 7; 
		advance( str, 0 );
		if ( token && !get_token(str,*token) )
			i = -1;
	}
	return i;
}

// ---
void CCodeGen::m_msg( COutputSect& old, COutputSect& anew ) {
	int  ni, ninext;
	CStr token;

	for( ni=fdecl(anew,0,&token); ni != -1; ni=ninext ) {
		int oi = old.find_by( 0, token, cmp_by_substr );
		ninext = fdecl( anew, ni+1, &token );
		if ( (-1 != oi) && !cmp_ignore_spaces_no_comment(old[oi],anew[ni]) ) {
			int i;
			int oi_next = fdecl( old, oi+1, 0 );
			if ( -1 == oi_next )
				oi_next = old.Count();

			int stop = ninext;
			if ( stop == -1 )
				stop = anew.Count();
			
			for( i=oi; oi<oi_next; oi++ )
				comment_out( old[i] );

			for( i=ni; i<stop; i++ ) {
				char* s = anew[i];
				anew[i] = 0;
				old.Insert( oi_next++, s );
			}
		}
	}
}

*/
