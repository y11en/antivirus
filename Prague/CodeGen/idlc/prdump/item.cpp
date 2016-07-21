#include "scantree.h"
#include "../../AVPPveID.h"


const char* cRootItem::s_imp_excludes[]  = { "os", "seqio", "string", 0 };
const char* cRootItem::s_tdef_excludes[] = { "hSTRING", "hOS", "hObjPtr", "hIO", "hSEQ_IO", "hREGISTRY", "hREPORT", 0 };


// ---
inline int crlf( const char*& s ) {
	if ( *s == '\n' ) {
		++s;
		if ( *s == '\r' ) {
			++s;
			return 2;
		}
		return 1;
	}
	if ( *s == '\r' ) {
		++s;
		if ( *s == '\n' ) {
			++s;
			return 2;
		}
		return 1;
	}
	return 0;
}



static bool printout_int( HANDLE file, const char* str, int len );


// ---
static bool po( HANDLE file, const char* str, int len ) {
	int clen;
	char out[0x100];
	while( len ) {
		if ( len > (sizeof(out)-1) )
			clen = sizeof(out)-1;
		else
			clen = len;
		memcpy( out, str, clen );
		out[clen] = 0;
		if ( !printout_int(file,out,clen) )
			return false;
		len -= clen;
		str += clen;
	}
	return true;
}



// ---
bool printout( HANDLE file, const char* str, int len, int level ) {
	
	if ( !str || !*str )
		return true;
	if ( len <= 0 )
		len = strlen( str );
	if ( len <= 0 )
		return true;

	const char* cr = strchr( str, '\r' );
	const char* lf = strchr( str, '\n' );
	bool ret = true;

	if ( cr > lf )
		cr = lf;
	if ( !cr || ((cr-str) > len) )
		ret = po( file, str, len );
	else {
		crlf( cr );
		int clen = cr - str;
		ret = po( file, str, clen );
		if ( ret ) {
			len -= clen;
			if ( len > 0 ) {
				space( file, level+2 );
				ret = printout( file, str+clen, len, level );
			}
		}
	}
	return ret;
}



// ---
static bool printout_int( HANDLE file, const char* str, int len ) {

	if ( !file )
		return !!printf( str );
	
	int flen = len;
	while( *str && len && ((*(str+len-1)=='\n') || (*(str+len-1)=='\r')) )
		--len;
	
	bool ret = true;
	DWORD written = 0;
	if ( len )
		ret = !!::WriteFile( file, str, len, &written, 0 );

	if ( ret && (len<flen) ) {
		static char lf[] = "\r\n";
		str += len;
		flen -= len;
		int adv = crlf(str);
		do {
			flen -= adv;
			ret = !!::WriteFile( file, lf, 2, &written, 0 );
		} while( ret && (flen>0) && (0 != (adv=crlf(str))) );
	}
	return ret;
}



// ---
bool space( HANDLE file, int c ) { 
	char buff[21]; 
	c *= 2;
	memset( buff,' ',sizeof(buff) ); 
	int cl = __min( sizeof(buff)-1, c ); 
	while( cl ) {
		buff[cl] = 0;
		po( file, buff, cl );
		c -= cl;
		cl = __min(sizeof(buff),c); 
	}
	return true;
}



// ---
bool cOutput::write( const char* str, int len ) {

	if ( !str || !*str || !len )
		return true;

	int l = strlen(str);
	if ( len == -1 )
		len = l;
	if ( l < len )
		len = l;

	int offs = cp - beg;
	int avail = m_size - offs;
	if ( avail < (len+1) ) {
		int add = (((len+1) / 0x1000) + !!((len+1) % 0x1000)) * 0x1000;
		beg = (char*)realloc( beg, m_size+add );
		if ( !beg )
			return false;
		cp = beg + offs;
		m_size += add;
	}

	memcpy( cp, str, len+1 );
	cp += len;

	return true;
}


// ---
bool cOutput::write( uint val, uint base ) {
	int  len;
	char buff[12];
	if ( base == 16 )
		len = sprintf( buff, "%x", val );
	else
		len = sprintf( buff, "%u", val );
	return write( buff, len );
}



// ---
bool cItem::out( HANDLE file, uint level, bool no_comment, bool no_attrib, bool new_line, bool sp ) {
	uint i, c;
	bool no_indent = false;
	if ( m_section ) {
		if ( !no_comment ) {
			printout( file, "\n", 1, level );
			space( file, level );
			printout( file, "// ------------------------------------\n", 0, level );
		}
		else
			no_indent = true;
	}
	if ( !no_comment )
		comment( file, level, true, new_line, ecp_b );

	bool nl = true;
	if ( !no_attrib ) {
		c = m_attr.Count();
		if ( !m_hidden && c ) {
			bool single_line = ((1==c) && (16>strlen(m_attr[0])));
			if ( single_line ) {
				space( file, level );
				printout( file, "[ ", 2, level );
			}
			else {
				printout( file, "\n", 1, level );
				space( file, level );
				printout( file, "[\n", 2, level );
			}
			for( i=0,c=m_attr.Count(); i<c; ++i ) {
				if ( !single_line )
					space( file, level+1 );
				printout( file, m_attr[i], 0, level );
				if ( !single_line )
					printout( file, "\n", 1, level );
			}
			if ( !single_line ) {
				space( file, level );
				printout( file, "]\n", 2, level );
			}
			else if ( (m_nt == VE_NT_PROPERTY) || (m_nt == VE_NT_MESSAGE) || !content() ) {
				printout( file, " ] ", 3, level );
				nl = false;
				sp = false;
			}
			else
				printout( file, " ]\n", 3, level );
		}
	}

	bool hdr = (m_hdr && *m_hdr);
	if ( !m_hidden && hdr ) {
		if ( nl && new_line || sp )
			space( file, level );
		else
			printout( file, " ", 1, 0 );
		printout( file, m_hdr, 0, level );
	}

	if ( 0 != (c=Count()) ) {
		bool l_nl = true;
		if ( (m_nt == VE_NT_METHOD) && (c < 3) && (!comment() || (m_comment_placement!=ecp_h)) ) {
			for( i=0; i<c; ++i ) {
				cItem& n = *(*this)[i];
				if ( n.attr_count() || (!no_comment && n.comment()) )
					break;
			}
			l_nl = (i<c);
		}
		if ( !m_hidden && hdr ) {
			open( file, level );
			bool lf = new_line && l_nl;
			if ( (no_comment || !comment(file,level,!hdr,lf,ecp_h)) && lf )
				printout( file, "\n", 1, level );
		}

		int add = (m_hidden || no_indent) ? 0 : 1;
		for( i=0,c=Count(); i<c; ++i ) {
			cItem& n = *(*this)[i];
			n.out( file, level+add, no_comment, no_attrib, l_nl, l_nl );
			if ( !l_nl && (nt() == VE_NT_METHOD) && (i==(c-1)) )
				printout( file, " ", 1, 0 );
		}

		if ( !m_hidden ) {
			if ( m_hdr && *m_hdr ) {
				bool lf = l_nl;
				if ( l_nl && ((m_nt == VE_NT_INTERFACE) || (m_nt == VE_NT_PLUGIN) || (m_nt == VE_NT_MESSAGECLASS)) ) {
					lf = false;
					space( file, level );
				}
				close( file, level, lf );
				if ( ((m_nt == VE_NT_INTERFACE) || (m_nt == VE_NT_PLUGIN)) )
					printout( file, "\n", 1, 0 );
			}
			if ( m_print_name && m_name ) {
				if ( *m_name ) {
					printout( file, " ", 0, level );
					printout( file, m_name, 0, level );
				}
				printout( file, ";", 0, level );
			}
			else if ( m_nt == VE_NT_DATASTRUCTURE )
				printout( file, ";", 0, level );
			if ( (no_comment || !comment(file,level,false,new_line,ecp_a)) && new_line )
				printout( file, "\n", 1, level );
		}
	}
	else {
		if ( hdr && !m_no_open ) {
			open( file, level );
			close( file, level, false );
		}
		if ( m_print_name && m_name ) {
			printout( file, m_name, 0, level );
			printout( file, ";", 0, level );
		}
		else if ( m_nt == VE_NT_DATASTRUCTURE )
			printout( file, ";", 0, level );
		if ( no_comment && hdr ) {
			if ( new_line )
				printout( file, "\n", 1, level );
			else
				printout( file, " ", 1, 0 );
		}
		else {
			bool lf = !m_no_open && new_line;
			if ( !comment(file,level,!hdr,lf,ecp_h) && !comment(file,level,!hdr,lf,ecp_a) && lf )
				printout( file, "\n", 1, level );
		}
	}
	return true;
}



// ---
bool cRootItem::tdef( const char* iface, const char* tdef ) { 
	if ( !tdef || !*tdef )
		return false;

	for ( const char** ex = s_tdef_excludes; *ex; ++ex ) {
		if ( !strcmp(*ex,tdef) )
			return false;
	}
	uint i = 0, c = m_tdef.Count();
	for( ; i<c; ++i ) {
		tdef_t& td = *m_tdef[i];
		if ( !strcmp(td.tdef(),tdef) )
			return true;
	}
	m_tdef.Add( new tdef_t(iface,tdef) );
	return true;
}



// ---
bool cRootItem::inc( HDATA iface, bool own, bool add_imp, bool check_excld ) {
	CFaceInfo* add = new CFaceInfo( iface, own && !add_imp );
	uint i=0, c=m_incs.Count();
	for( ; i<c; ++i ) {
		CFaceInfo* curr = m_incs[i];
		if ( (iface == curr->Data()) || !strcmp(curr->Name(),add->Name()) ) {
			delete add;
			add = 0;
			break;
		}
	}
	if ( add )
		m_incs.Add( add );

	if ( add_imp ) {
		char path[MAX_PATH];
		HPROP cont_prop = ::DATA_Find_Prop( iface, 0, VE_PID_IF_CONTAINERNAME );
		if ( cont_prop && ::PROP_Get_Val(cont_prop,path,sizeof(path)) ) {
			const char* fname = _extract_fname( path );
			if ( check_excld ) {
				for ( const char** ex = s_imp_excludes; *ex; ++ex ) {
					if ( !strcmp(*ex,fname) ) {
						fname = 0;
						break;
					}
				}
			}
			if ( fname )
				imp( fname );
		}
	}
	return true;
}



// ---
cRootItem::type_t* cRootItem::find_resolved_type( const char* tname ) {
	uint i=0, c=m_knwn.Count();
	for( ; i<c; ++i ) {
		type_t& t = m_knwn[i];
		if ( t.check(tname) )
			return &t;
	}
	return 0;
}



// ---
cRootItem::type_t* cRootItem::add_resolved_type( const char* type, cAutoChar& resolved, uint tid ) {
	type_t* t = find_resolved_type( type );
	if ( t ) {
		if ( t->_tid != tid )
			return 0;
		return t;
	}
	t = m_knwn.Add();
	if ( t ) {
		new (t) type_t();
		t->set( type, resolved, tid );
		return t;
	}
	return 0;
}



// ---
char* cRootItem::dup_type( type_t* type ) {
	if ( !type )
		return 0;
	if ( m_iface ) {
		const char* dot = strchr( type->_resolved, '.' );
		if ( dot ) {
			const char* name = m_iface.Name();
			int len = strlen(name);
			if ( !strncmp(name,type->_resolved,len) && (type->_resolved[len] == '.') )
				return _strdup( type->_resolved+len+1 );
		}
	}
	return _strdup( type->_resolved );
}



// ---
char* cRootItem::check_import_type( const char* tname, uint& tid ) {

	uint i=0, c=m_unk_types.Count();
	for( ; i<c; ++i ) {
		if ( !strcmp(m_unk_types[i],tname) )
			return 0;
	}

	type_t* t = find_resolved_type( tname );
	if ( t )
		return dup_type( t );

	ctdparam p(tname,tid);
	if ( _check_type_defined(m_iface.Data(),&p) )
		return dup_type( add_resolved_type(tname,p.combined,tid) );

	i=0, c=m_incs.Count();
	for( ; i<c; ++i ) {
		CFaceInfo* iface = m_incs[i];
		if ( m_iface.Data() == iface->Data() )
			continue;
		if ( _check_type_defined(iface->Data(),&p) )
			return dup_type( add_resolved_type(tname,p.combined,tid) );
	}
	
	HDATA iface = _find_proto( (bool(*)(HDATA,void*))_check_type_defined, &p );
	if ( iface ) {
		inc( iface, true, true, false );
		return dup_type( add_resolved_type(tname,p.combined,tid) );
	}

	m_unk_types.Add( _strdup(tname) );
	return 0;
}


// ---
bool cRootItem::out( HANDLE file, uint level, bool no_comment, bool no_attrib, bool new_line, bool sp ) {
	uint i = 0, c = m_imp.Count();
	for( ; i<c; ++i ) {
		printout( file, "#import \"", 0, level );
		printout( file, m_imp[i], 0, level );
		printout( file, DEF_PRT_EXT "\"\n", 0, level );
	}
	if ( c )
		printout( file, "\n", 1, level );

	i = 0, c = m_tdef.Count();
	for ( ; i<c; ++i ) {
		tdef_t& td = *m_tdef[i];
		printout( file, "forward interface ", 18, level );
		printout( file, td._iface, 0, level );
		printout( file, ";\n", 2, level );
		printout( file, "typedef ", 8, level );
		printout( file, td._iface, 0, level );
		printout( file, "* ", 2, level );
		printout( file, td._tdef, 0, level );
		printout( file, ";\n\n", 3, level );
	}
	if ( c )
		printout( file, "\n", 1, level );

	i = 0, c = m_unk_types.Count();
	for ( ; i<c; ++i ) {
		const char* type = m_unk_types[i];
		if ( (*type == 'h') || _check_keyword(type) || _check_type(type) )
			continue;
		printout( file, "forward struct ", 15, level );
		printout( file, type, 0, level );
		printout( file, ";\n", 2, level );
	}
	if ( c )
		printout( file, "\n", 1, level );

	return cItem::out( file, level, no_comment, no_attrib, new_line, sp );
}



// ---
bool cItem::comment( HANDLE file, uint level, bool print_space, bool lf, tCommPlace place ) {
	if ( place == m_comment_placement ) {
		bool printed = false;
		if ( m_comment && *m_comment ) {
			const char* comm = m_comment;
			while( *comm && *comm == ' ' )
				++comm;
			if ( (*comm == '/') && (*(comm+1) == '/') ) {
				comm += 2;
				while( *comm && *comm == ' ' )
					++comm;
			}
			if ( *comm ) {
				printed = true;
				if ( print_space )
					space( file, level ); 
				else
					printout( file, " ", 1, level );
				printout( file, "// ", 3, level );
				printout( file, comm, 0, level );
			}
		}
		if ( lf && (printed || !print_space) )
			printout( file, "\n", 1, level );
		return true;
	}
	return false;
}


// ---
bool cItem::close( HANDLE file, uint level, bool lf ) {
	if ( lf ) {
		printout( file, "\n", 1, 0 );
		space( file, level );
	}
	return printout( file, m_close, 0, 0 );
}


// ---
void cItem::attr( const char* attr ) { 
	if ( !attr || !*attr )
		return;
	uint i, c = m_attr.Count();
	for( i=0; i<c; ++i ) {
		const char* str = m_attr[i];
		if ( !strcmp(str,attr) )
			return;
	}
	m_attr.Add( _strdup(attr) ); 
}



// ---
void cItem::attr_insert( const char* attr ) { 
	if ( !attr || !*attr )
		return;
	uint i, c = m_attr.Count();
	for( i=0; i<c; ++i ) {
		const char* str = m_attr[i];
		if ( !strcmp(str,attr) )
			return;
	}
	m_attr.Insert( 0, _strdup(attr) ); 
}



// ---
bool cItem::content() const {
	uint i, c = Count();
	for( i=0; i<c; ++i ) {
		cItem* it = operator[](i);
		if ( it->content() )
			return true;
		const char* hdr = it->hdr();
		if ( hdr && *hdr )
			return true;
	}
	return false;
}



// ---
void cRootItem::imp( const char* imp ) { 
	uint i, c = m_imp.Count();
	for( i=0; i<c; ++i ) {
		const char* str = m_imp[i];
		if ( !strcmp(str,imp) )
			return;
	}
	m_imp.Add( _strdup(imp) ); 
}


