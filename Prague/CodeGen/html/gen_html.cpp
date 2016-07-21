#include "stdafx.h"
#include "html.h"
#include "htmlgen.h"
#include <datatreeutils/dtdropper.h>
#include <datatreeutils/dtutils.h>
#include <_avpio.h>
#include "../AVPPveID.h"
//#include "../../avpgs/avpg.h"
//#include "../../wrappers/dmap/plugin_dmap.h"

char      g_buff[0x4000];
const int g_buffs = sizeof(g_buff);

const char* g_ec  = "comment is empty";
const char* g_esc = "short comment is empty";
const char* g_elc = "large comment is empty";
//const char* g_ebc = "short and large comment are empty";
const char* g_ebc = "behavior comment is empty";


const char* gst_co      = "Commment";
const char* gst_sc      = "ShortCommment";
const char* gst_lc      = "LargeComment";
const char* gst_bc      = "BehaviorComment";
const char* gst_nc      = "NoComment";
const char* gst_item_id = "ItemId";
const char* gst_syntax  = "Syntax";
const char* gst_source  = "Source";
const char* gst_topic   = "TopicId";

const tdescriptor g_descr[] = {
	{ VE_NT_PLUGIN,                   false, false, ifPlugin,      "Модуль" },
	{ VE_NT_INTERFACE,                false, false, ifIFace,       "Интерфейс" },
	{ VE_NT_METHOD,                   true,  true,  ifPubMethod,   "Методы интерфейса" },
	{ VE_NT_PROPERTY,                 true,  true,  ifProperty,    "Свойства" },
	{ VE_NT_TYPE,                     true,  true,  ifType,        "Типы" },
	{ VE_NT_CONSTANT,                 true,  true,  ifConstant,    "Константы" },
	{ VE_NT_ERRORCODE,                true,  true,  ifErrCode,     "Коды ошибок" },
	{ VE_NT_MESSAGECLASS,             true,  true,  ifMsgClass,    "Классы сообщений" },
	{ VE_NT_MESSAGE,                  true,  true,  ifMsg,         "Класс включает сообщения" },
	{ VE_NT_METHODPARAM,              true,  true,  ifMethodParam, "Параметры метода" },
	{ VE_NT_SYSTEMMETHOD,             false, false, ifIntMethod,   "Внутренние методы реализации" },
	{ VE_NT_SYSTEMMETHODPARAM,        false, false, ifMethodParam, "Параметры метода" },
	{ VE_PID_PL_EXPORTS,              false, false, ifAny,         "Экспортируемые процедуры" },
	{ VE_PID_PL_IMPORTS,              false, false, ifAny,         "Импортируемые процедуры" },
	{ VE_PID_IF_DATASTRUCTURES,       false, false, ifAny,         "Структура внутренних данных" },
	{ VE_PID_IF_DATASTRUCTUREMEMBERS, false, false, ifAny,         "Полянки внутренней структуры" },
};
const int g_descr_count = countof(g_descr);


// ---
//struct {
//	tDWORD pid;
//	const char* name;
//  const char* folder;
//} g_plugins[] = {
//  {PID_NATIVE_FIO, "nfio", 0 },
//  {AVPG_ID,        "avpgs", 0 },
//  {DMAP_ID,        0,       "wrappers/" },
//
//};



// ---
const char* get_pl_name( CPluginInfo& pl ) {
  tDWORD      pid = pl.PluginID();
  const char* pn = 0;
  
//  for( int i=0; i<countof(g_plugins); i++ ) {
//    if ( g_plugins[i].pid == pid ) {
//      pn = g_plugins[i].name;
//      break;
//    }
//  }
  
  if ( !pn )
    pn = pl.Name();
  
  return pn;
}



// ---
const char* get_pl_folder( CPluginInfo& pl ) {
  tDWORD      pid = pl.PluginID();
  const char* pn = 0;
  
//  for( int i=0; i<countof(g_plugins); i++ ) {
//    if ( g_plugins[i].pid == pid ) {
//      pn = g_plugins[i].folder;
//      break;
//    }
//  }
  
  return pn;
}




// ---
bool imp_name( CPluginInfo& pl, CBaseInfo& module, char* name ) {
	const char* pname = get_pl_name( pl );
	int len = ::lstrlen( pname );
	::memcpy( name, pname, len );
	*(name+len++) = '_';
	module.StrAttr( I(_OUTPUTFILE), name+len, MAX_PATH );
	::lstrcat( name, ".html" );
	return true;
}



// ---
bool CCodeGen::name_gen( CBaseInfo& module, bool prototype, char* ffull_path, char* fname ) {
	const char* pn;
	tDWORD type = module.EntityType();
	if ( type == ifPlugin ) {
		CPluginInfo plugin( module.Data(), false );
		pn = get_pl_name( plugin );
		if ( fname ) {
			::lstrcpy( fname, pn );
			::lstrcat( fname, ".html" );
		}
		if ( ffull_path ) {
			::lstrcpy( ffull_path, m_output_folder );
			::lstrcat( ffull_path, pn );
			::lstrcat( ffull_path, ".html" );
		}
	}
	else if ( prototype ) {
		if ( fname ) {
			module.StrAttr( I(_INCLUDEFILE), fname, MAX_PATH );
			::lstrcat( fname, ".html" );
		}
		if ( ffull_path ) {
			::lstrcpy( ffull_path, m_output_folder );
			module.StrAttr( I(_INCLUDEFILE), ffull_path+::lstrlen(ffull_path), MAX_PATH );
			::lstrcat( ffull_path, ".html" );
		}
	}
	else { 
		if ( fname )
			imp_name( m_plugin, module, fname );
		if ( ffull_path ) {
			int len = ::lstrlen( m_output_folder );
			::memcpy( ffull_path, m_output_folder, len );
			imp_name( m_plugin, module, ffull_path+len );
		}
	}
	return true;
}



// ---
HRESULT CCodeGen::MakeIfaceHTML( bool prototype, const char* src_name ) {
	HANDLE file;

	/*
	LoadIO();
	DTUT_Init_Library( malloc, free );
	DropDTToFileInitLibrary( malloc, free );
	DropDTToFile( m_iface.Data(), "d:\\prague\\test\\drop.txt" );
	*/

	//name_gen( m_iface, prototype, g_buff, 0 );
	file = ::CreateFile( src_name, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( INVALID_HANDLE_VALUE == file )
		return HRESULT_FROM_WIN32( ::GetLastError() );

	CHtml doc( "html" );

	m_iface.Name( g_buff+12, g_buffs-12 );

	CHtml* title = doc.add("title");
	if ( prototype )
		title->str( "Прототип " );
	else
		title->str( "Реализация " );
	title->str( "интерфейса " );
	title->str( m_iface.Name() );

	CHtml* head = doc.add( "head" );
	head->add( "meta http-equiv=Content-Type content=\"text/html; charset=windows-1251\"", 0, 0, false );

	CHtml* link = head->add( "link", "rel", "stylesheet" );
	link->attr( "type", "text/css" );
	link->attr( "href", "prstyles.css" );
	link->str( "<!-- @ -->" );
	//head->add( "<LINK REL="stylesheet" TYPE="text/css" HREF="prstyles.css"><!-- @ --></LINK>" );
	
	CHtml* iframe = head->add( "iframe" );
	iframe->attr( "ID", "HeaderFrame" );
	iframe->attr( "height", "0" );
	iframe->attr( "width", "0" );
	iframe->attr( "marginwidth", "0" );
	iframe->attr( "src", "header.htm" );
	//<iframe ID="HeaderFrame" height=0 width=0 marginwidth=0 src="header.htm"></iframe><BR><BR>

	IfaceHTML( doc.add("body"), prototype );

	CElem::lf = true;
	CElem::aligned = m_brick ? false : true;
	doc.flush( file, 0 );
	::CloseHandle( file );
	return S_OK;
}




// ---
HRESULT CCodeGen::IfaceHTML( CHtml* body, bool prototype ) {
	CHtml* wrk;
	CHtml* dir;
	int    count = 0;

	if ( !prototype ) {
		wrk = body->add( "h1" );
		wrk->str( "Плагин - " );
		wrk->str( m_plugin.Name() );
	}

	wrk = body->add( "h1" );

	if ( prototype )
		wrk->str( "Прототип " );
	else
		wrk->str( "Реализация " );
	wrk->str( "интерфейса \"" );
	wrk->str( m_iface.Name() );
	wrk->str( "\"" );
	wrk->line( "<hr>" );

	int len = 0;
	wrk = body->para();
	if ( prototype ) {
		wrk->str( "Заголовочный файл - ");
		::lstrcpy( g_buff, "../include/iface/" );
		m_iface.StrAttr( I(_INCLUDEFILE), g_work, g_works );
		::lstrcat( g_work, ".h" );
		::lstrcat( g_buff, g_work );
	}
	else {
    const char* folder = get_pl_folder( m_plugin );

		wrk->str( "Исходный текст - ");
		::lstrcpy( g_buff, "../" );
    if ( folder )
      ::lstrcat( g_buff, folder );
		::lstrcat( g_buff+3, get_pl_name(m_plugin) );
		::lstrcat( g_buff, "/" );
		m_iface.StrAttr( I(_OUTPUTFILE), g_work, g_works );
		::lstrcat( g_work, ".c" );
		::lstrcat( g_buff, g_work );
	}
	wrk->href(g_buff,false)->str( g_work );
	
	if ( m_iface.Static() )
		body->para()->line( "Интерфейс реализован как статический" );

	CComment comm( m_iface );

 	count += comment( body, comm, m_lang, t_short );
	count += comment( body, comm, m_lang, t_large );
	//count += comment( body, comm, m_lang, t_behav );

	if ( !count && m_full )
		body->para()->styled_text( gst_nc, "Interface has no description at all !!!" );

	dir = body->add( "dir" );

	if ( !prototype )
		ImpInterfaceSourceTable( body );

	for( int i=0; i<g_descr_count; i++ ) {
		if ( (g_descr+i)->on ) {
			HDATA el = find_el( m_iface, g_descr+i );
			if ( el )
				ListEntities( body, dir, el, g_descr+i, (g_descr+i)->name );
		}
	}

	return S_OK;
}



// ---
//CHtml* make_table( CHtml* parent, int shift=0 ) {
//	parent = parent->para( shift );
//	CHtml* table = parent->add( "table" );
//	table->attr( "border", "1" );
//	CHtml* head = table->add( "thead" );
//	CHtml* tr = head->add( "tr" );
//	CHtml* th;
//	th = tr->add( "th" );  
//	th->attr( "width", "35%" );
//	th->line( "id" );
//	th = tr->add( "th" );
//	th->line( "comment" );
//	return table->add( "tbody" );
//}


// ---
HDATA CCodeGen::find_el( CBaseInfo& dad, const tdescriptor* descr ) {
	HDATA  branch;
	HDATA  elem = 0;
	for( branch=::DATA_Get_FirstEx(dad.Data(),0,0); branch; elem=0,branch=::DATA_Get_Next(branch,0) ) {
		elem = ::DATA_Get_FirstEx( branch, 0, 0 );
		if ( !elem )
			continue;
		HPROP prop = ::DATA_Find_Prop( elem, 0, VE_PID_NODETYPE );
		if ( !prop )
			continue;
		tDWORD ctype;
		if ( sizeof(tDWORD) != ::PROP_Get_Val(prop,&ctype,sizeof(tDWORD)) )
			continue;
		if ( ctype == descr->type )
			break;
	}
	return elem;
}



// ---
void CCodeGen::msg_info( CBaseInfo& bi, CHtml* dad, tDWORD id, const char* name ) {
	bool done = false;
	CAPointer<char>  r;
	CAPointer<char>  e;
	const char*      p = 0;

	if ( 1 < bi.StrAttr(id,g_buff,g_buffs) ) {
		divide( r, e, g_buff );
		if ( m_lang == l_rus )
			p = r;
		else if ( m_lang == l_eng )
			p = e;
		else if ( *r )
			p = r;
		else
			p = e;
	}
		
	CHtml* wrk;
	if ( p && *p ) {
		wsprintf( g_buff, "Параметр \"%s\": ", name );
		wrk = dad->para(50)->style( gst_lc );
		wrk->str( g_buff );
		wrk->str( p );
	}
	else if ( m_full ) {
		wsprintf( g_buff, "Параметр \"%s\" не определен", name );
		wrk = dad->para(50)->style( gst_nc );
		wrk->str( g_buff );
	}
}




// ---
HRESULT CCodeGen::ListEntities( CHtml* parent, CHtml* dir, HDATA elem, const tdescriptor* descr, const char* ename ) {

	CHtml* wrk;
	CHtml* table = 0;
	CHtml* group;
	const char* name;
	
	if ( !elem )
		return S_FALSE;

	CBaseInfo list( elem, descr->en_type );
	CCommentList clist( list, m_lang );

	if ( !clist.Count() )
		return S_OK;
	
	if ( dir ) {
		parent->line( 0 );
		parent->line( 0 );
		parent->line( 0 );
		parent->line( 0 );
		parent->add("h2")->lable(ename)->styled_text( gst_topic, ename );
		dir->add("li",0,0,false)->href(ename)->line( ename );
	}
	else
		parent->add("h5")->styled_text( gst_topic, ename );
	parent->add( "hr", 0, 0, false );

	bool method  = (descr->en_type == ifPubMethod) || (descr->en_type == ifIntMethod);
	bool prop    = descr->en_type == ifProperty;
	bool msg_cls = descr->en_type == ifMsgClass;
	
	CPArray<char> titles(2,10,false);
	titles.Add( "имя" );
	titles.Add( "описание" );

	int i, c;
	int j, n;

	clist.combine_topics( m_lang );
	for( i=0,c=clist.Count(); i<c; i++ ) {
		name = clist[i]->topic( m_lang );
		if ( name ) {
			wrk = parent->add( "h3" );
			wrk->line( name );
			comment( parent, *clist[i], m_lang, t_short );
			comment( parent, *clist[i], m_lang, t_large );
		}
		table = parent->table( titles, 50 );
		for( j=0,n=clist[i]->Count(); j<n; j++ ) {
			CComment& comm = *(*clist[i])[j];
			name = comm.item().Name();
			if ( !table )
				table = parent->table( titles, 50 );
			CHtml* tr = table->add( "tr" );
			tr->add("td","width","1")->href(name)->style(gst_item_id)->str(name);
			comment( tr->add("td"), comm, m_lang, t_short, false );
		}
	}

	if ( m_full || clist.exist(m_lang,t_all) ) {
		group = parent->style( "FullDescription" );
		if ( dir )
			group->add( "h3" )->line( "Подробное описание" );
		group = table = group->para( 50 );
		
		for( i=0,c=clist.Count(); i<c; i++ ) {
			CCommentGroup& cg = *clist[i];
			if ( !method && !msg_cls && !cg.exist(m_lang,t_all) ) 
				continue;

			const tdescriptor* msgs = 0;
			if ( msg_cls ) {
				for( int i=0; i<countof(g_descr); i++ ) {
					if ( (g_descr+i)->type == VE_NT_MESSAGE ) {
						msgs = g_descr + i;
						break;
					}
				}
			}

			name = cg.topic( m_lang );
			if ( name ) {
				int is_comm = 0;
				wrk = table->add( "h4" );
				wrk->str( "Группа - " );
				wrk->str( name );
				is_comm += comment( table, cg, m_lang, t_short );
				is_comm += comment( table, cg, m_lang, t_large );
				if ( !is_comm && m_full )
					table->para()->styled_text( gst_nc, g_ebc );
			}
			group = table->para( 50 );
			
			for( j=0,n=cg.Count(); j<n; j++ ) {
				CComment& comm = *cg[j];
				CBaseInfo& bi = comm.item();

				name = bi.Name();
				if ( !name )
					continue;

				if ( method ) {
					CMethodInfo mi( bi.Data(), descr->en_type );
					group->lable(name)->add( "h4" )->line( name );
					MethodHTML( group->para(50), mi );
				}
				else {
					group->lable(name)->add( "h4" )->line( name );
					comment( group, comm, m_lang, t_short );
					comment( group, comm, m_lang, t_large );
					comment( group, comm, m_lang, t_behav );
					if ( descr->en_type == ifMsg ) {
            msg_info( bi, group, I(M_MESSAGE_SEND_POINT), "Send point" );
            msg_info( bi, group, I(M_MESSAGECONTEXT), "Ctx" );
						msg_info( bi, group, I(M_MESSAGEDATA),    "Data" );
					}
					else if ( msgs )	{
						wrk = group->para(50);
						elem = ::DATA_Get_FirstEx( bi.Data(), 0, 0 );
						ListEntities( wrk, 0, elem, msgs, msgs->name );
					}
				}
			}
		}
	}

	return S_OK;
}



// ---
bool CCodeGen::CommentExist( CBaseInfo& item, bool large, bool shrt, bool behav ) {
	if ( large && (1 < item.ExtComment(0,0)) )
		return true;
	if ( shrt && (1 < item.ShortComment(0,0)) )
		return true;
	if ( behav && (1 < item.BehaviorComment(0,0)) )
		return true;
	return false;
}


	
// ---
bool CCodeGen::ListCommentExist( CBaseInfo& list, bool property, bool large, bool shrt, bool behav ) {
	list.GoFirst();
	while ( list ) {
		const char* name = list.Name();
		if ( property && (!::lstrcmp(name,"INTERFACE_VERSION") || !::lstrcmp(name,"INTERFACE_COMMENT")) )
			;
		else if ( CommentExist(list,large,shrt,behav) )
			return true;
		list.GoNext();
	}
	return false;
}



// ---
bool CCodeGen::GroupCommentExist( CBaseInfo& list ) {
	CBaseInfo clone( list.Data(), list.EntityType() );
	return false;
}



// ---
bool CCodeGen::comment( CHtml* body, CComment& comm, tLang lang, tType type, bool para ) {
	const char* str;
	const char* style;
	const char* def;

	switch( type ) {
		case t_short : style = gst_sc; def = g_esc; break;
		case t_large : style = gst_lc; def = g_elc; break;
		case t_behav : style = gst_bc; def = g_ebc; break;
		default      : style = gst_co; def = g_ec;  break;
	}

	str = comm.str( lang, type );
	if ( str ) {
		if ( para )
			body->para(50)->styled_text( style, str );
		else
			body->style(style)->str( str );
		return true;
	}
	else if ( m_full ) {
		if ( para ) 
			body->para(50)->styled_text( gst_nc, def );
		else
			body->style(gst_nc)->str( def );
		return false;
	}
	else
		return false;
}


