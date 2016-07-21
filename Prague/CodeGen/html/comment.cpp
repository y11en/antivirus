#include "stdafx.h"
#include "htmlgen.h"


// ---
char* copy( const char* b, const char* e ) {
	if ( !b || !*b )
		return 0;

	int l = e ? (e-b) : ::lstrlen(b);

	char* o = (char*)::memcpy( new char[l+1], b, l );
	*o = toupper( *o );
	*(o+l) = 0;
	return o;
}



// ---
void divide( CAPointer<char>& r, CAPointer<char>& e, char* buff ) {
	char* eng = ::strstr( buff, "//eng:" );
	char* rus = ::strstr( buff, "//rus:" );

	if ( !rus && !eng )
		r = ::copy( buff, 0 );
	else if ( !eng )
		r = ::copy( rus+6, 0 );
	else if ( !rus ) {
		e = ::copy( eng+6, 0 );
		if ( eng > buff )
			r = ::copy( buff, eng );
	}
	else if ( eng > rus ) {
		r = ::copy( rus+6, eng );
		e = ::copy( eng+6, 0 );
	}
	else {
		r = ::copy( rus+6, 0 );
		e = ::copy( eng+6, rus );
	}
}


// ---
CComment::CComment( CBaseInfo& item ) : m_info(item.Data(),item.EntityType()) {
	int len;

	m_title = item.IsSeparator();
	len = item.ShortComment( g_buff, g_buffs );
	if ( 1 < len )
		divide( sr, se, g_buff );

	len = item.ExtComment( g_buff, g_buffs );
	if ( 1 < len )
		divide( lr, le, g_buff );
	
	len = item.BehaviorComment( g_buff, g_buffs );
	if ( 1 < len )
		divide( br, be, g_buff );
}



// ---
bool CComment::exist( tLang lang, tType type ) const {
	switch ( type ) {
		case t_short :
			switch( lang ) {
				case l_rus : return sr ? true : false;
				case l_eng : return se ? true : false;
				case l_any : return (sr || se) ? true : false;
			}
			break;
		case t_large :
			switch( lang ) {
				case l_rus : return lr ? true : false;
				case l_eng : return le ? true : false;
				case l_any : return (lr || le) ? true : false;
			}
			break;
		case t_behav :
			switch( lang ) {
				case l_rus : return br ? true : false;
				case l_eng : return be ? true : false;
				case l_any : return (br || be) ? true : false;
			}
			break;
		case t_sl :
			switch( lang ) {
				case l_rus : return (sr || lr) ? true : false;
				case l_eng : return (se || le) ? true : false;
				case l_any : return (sr || lr || se || le) ? true : false;
			}
			break;
		case t_sb :
			switch( lang ) {
				case l_rus : return (sr || br) ? true : false;
				case l_eng : return (se || be) ? true : false;
				case l_any : return (sr || br || se || be) ? true : false;
			}
			break;
		case t_lb :
			switch( lang ) {
				case l_rus : return (lr || br) ? true : false;
				case l_eng : return (le || be) ? true : false;
				case l_any : return (lr || br || le || be) ? true : false;
			}
			break;
		case t_all :
			switch( lang ) {
				case l_rus : return (sr || lr || br) ? true : false;
				case l_eng : return (se || le || be) ? true : false;
				case l_any : return (sr || lr || br || se || le || be) ? true : false;
			}
			break;
	}
	return false;
}



// ---
const char* CComment::str( tLang lang, tType type ) const {
	switch( type ) {
		case t_short :
			switch( lang ) {
				case l_rus : return sr;
				case l_eng : return se;
				case l_any : if ( sr ) return sr; else return se;
			}
			break;
		case t_large :
			switch( lang ) {
				case l_rus : return lr;
				case l_eng : return le;
				case l_any : return lr ? lr : le;
			}
		case t_behav :
			switch( lang ) {
				case l_rus : return br;
				case l_eng : return be;
				case l_any : if ( br ) return br; else return be;
			}
			break;
		case t_sl :
			switch( lang ) {
				case l_rus : return lr ? lr : sr;
				case l_eng : return le ? le : se;
				case l_any : return lr ? lr : (sr ? sr : (le ? le : se));
			}
			break;
		case t_sb :
			switch( lang ) {
				case l_rus : return br ? br : sr;
				case l_eng : return be ? be : se;
				case l_any : return br ? br : (sr ? sr : (be ? be : se));
			}
			break;
		case t_lb :
			switch( lang ) {
				case l_rus : return br ? br : lr;
				case l_eng : return be ? be : le;
				case l_any : return br ? br : (lr ? lr : (be ? be : le));
			}
			break;
		case t_all :
			switch( lang ) {
				case l_rus : return br ? br : (lr ? lr : sr);
				case l_eng : return be ? be : (le ? le : se);
				case l_any : return br ? br : (lr ? lr : (sr ? sr : (be ? be : (le ? le : se))));
			}
			break;
	}
	return 0;
}



// ---
CCommentGroup::CCommentGroup( CBaseInfo& group, tLang lang ) : CPArray<CComment>(10,10,true), CComment(group) {
	for( int c=0; group; group.GoNext() ) {
		const char* name = group.Name();
		if ( !::lstrcmp(name,"INTERFACE_VERSION") || !::lstrcmp(name,"INTERFACE_COMMENT") )
			continue;

		if ( !group.IsSeparator() ) 
			Add( new CComment(group) );
		else if ( !c )
			divide( rtopic, etopic, (char*)name );
		else
			break;
		c++;
	}
}



// ---
bool CCommentGroup::exist( tLang lang, tType type ) const {
	for( int i=0, c=Count(); i<c; i++ ) {
		CComment* comm = parr[i];
		if ( comm->exist(lang,type) )
			return true;
	}
	return false;
}



// ---
const char* CCommentGroup::str( int index, tLang lang, tType type ) const {
	if ( uint(index+1) > Count() )
		return 0;
	else
		return parr[index]->str( lang, type );
}




// ---
CCommentList::CCommentList( CBaseInfo& list, tLang lang ) : CPArray<CCommentGroup>(10,10,true) {
	list.GoFirst();
	while( list ) {
		CCommentGroup* ng = new CCommentGroup(list,lang);
		if ( ng->Count() )
			Add( ng );
		else
			delete ng;
	}
}


// ---
bool CCommentList::exist( tLang lang, tType type ) const {
	for( int i=0, c=Count(); i<c; i++ ) {
		CCommentGroup* cga = parr[i];
		if ( cga->exist(lang,type) )
			return true;
	}
	return false;
}



// ---
bool CCommentList::combine_topics( tLang lang ) {
	for( uint i=0; i<Count(); i++ ) {
		CCommentGroup& cg = *(*this)[i];
		const char* topic = cg.topic( lang );
		if ( !topic || !*topic )
			continue;
		for( uint j=i+1; j<Count(); j++ ) {
			CCommentGroup& acg = *(*this)[j];
			const char* atopic = acg.topic( lang );
			if ( !::lstrcmpi(topic,atopic) ) {
				CComment& c = *acg[0];
				if ( c.title() )
					acg.RemoveInd( 0, Delete );
				cg.EatAway( acg );
				RemoveInd( j, Delete );
				j--;
			}
		}
	}
	return true;
}



