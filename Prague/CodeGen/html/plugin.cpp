#include "stdafx.h"
#include "html.h"
#include "htmlgen.h"
#include <datatreeutils/dtdropper.h>
#include <datatreeutils/dtutils.h>
#include <_avpio.h>
#include "../AVPPveID.h"


struct {
  AVP_dword id;
  char*     mnemonic;
  char*     name;
} vendors[] = { 
  { VID_ANDY,       "VID_ANDY",       "Andy Nikishin"           },
  { VID_ANDREW,     "VID_ANDREW",     "Andrew Krukov"           },
  { VID_PETROVITCH, "VID_PETROVITCH", "Andrew Doukhvalow"       },
  { VID_GRAF,       "VID_GRAF",       "Alexey De Mont de Rique" },
  { VID_SOBKO,      "VID_SOBKO",      "Andrew Sobko"            },
  { VID_COSTIN,     "VID_COSTIN",     "Costin Raiu"             },
  { VID_VICTOR,     "VID_VICTOR",     "Victor Matioushenkov"    },
  { VID_MIKE,       "VID_MIKE",       "Mike Pavlushchik"        },
  { VID_EUGENE,     "VID_EUGENE",     "Eugene Kaspersky"        },
  { VID_VASILIEV,   "VID_VASILIEV",   "Vladimir Vasiliev"       },
  { VID_VALIK,      "VID_VALIK",      "Valentin Kolesnikov"     },
  { VID_ALEX,       "VID_ALEX",       "Alex Antropov"           },
  { VID_TAG,        "VID_TAG",        "Andrew Tikhonov"         },
  { VID_TIM,        "VID_TIM",        "Tim Yunaev"              },
};



struct {
  AVP_dword id;
  char*     mnemonic;
} cps[] = { 
  { 0,           "cCP_ANSI"    },
  { cCP_ANSI,    "cCP_ANSI"    },
  { cCP_OEM,     "cCP_OEM"     },
  { cCP_UNICODE, "cCP_UNICODE" },
};



// ---
static void printout_vendor_id( AVP_dword id ) {
  int i;
  for( i=0; i<countof(vendors); i++ ) {
    if ( id == vendors[i].id ) {
      strcpy( g_buff, vendors[i].mnemonic );
      return;
    }
  }
  strcpy( g_buff, "unknown vendor" );
}



// ---
static void printout_codepage( AVP_dword id ) {
  int i;
  for( i=0; i<countof(cps); i++ ) {
    if ( id == cps[i].id ) {
      strcpy( g_buff, cps[i].mnemonic );
      return;
    }
  }
  strcpy( g_buff, "unknown codepage" );
}



// ---
static void add_row( CHtml* table, CBaseInfo& item, const char* id, DWORD val_id, bool full ) {
	AVP_dword val;
	CHtml* row;
	const char* sstyle;
	
	*g_buff = 0;
	switch( GET_AVP_PID_TYPE(val_id) ) {
		case avpt_bool :
			if ( item.BoolAttr(val_id) )
				::lstrcpy( g_buff, "да" );
			else
				::lstrcpy( g_buff, "нет" );
			sstyle = "ShortComment";
			break;
		case avpt_dword :
			val = item.DWAttr( val_id );
      if ( val_id == VE_PID_PL_VENDORID )
        printout_vendor_id( val );
      else if ( val_id == VE_PID_PL_CODEPAGEID )
        printout_codepage( val );
			else
        wsprintf( g_buff, "%u", val );
			sstyle = "ShortComment";
			break;
		case avpt_str :
			item.StrAttr( val_id, g_buff, g_buffs );
			sstyle = "ShortComment";
			break;
		default:
			if ( full ) {
				::lstrcpy( g_buff, "unknown type" );
				sstyle = "NoComment";
			}
			break;
	}

	if ( *g_buff ) {
		row = table->add( "tr" );
		row->add("td","width","1")->style("ItemId")->line( id );
		row->add("td")->style( sstyle )->line( g_buff );
	}
}



// ---
HRESULT CCodeGen::MakePluginHTML( bool prototype, const char* src_name ) {
	HANDLE file;

	/*
	LoadIO();
	DTUT_Init_Library( malloc, free );
	DropDTToFileInitLibrary( malloc, free );
	DropDTToFile( m_iface.Data(), "d:\\prague\\test\\drop.txt" );
	*/

	if ( !m_plugin || !m_plugin.Selected() )
		return S_OK;

  if ( m_plugin_done ) 
    return S_OK;

	file = ::CreateFile( src_name, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( INVALID_HANDLE_VALUE == file )
		return HRESULT_FROM_WIN32( ::GetLastError() );

  m_plugin_done = true;

  CHtml doc( "html" );

	m_plugin.Name( g_buff+12, g_buffs-12 );

	CHtml* title = doc.add("title");
	title->str( "Плагин - " );
	title->str( m_plugin.Name() );

	CHtml* head = doc.add( "head" );
	head->add( "meta http-equiv=Content-Type content=\"text/html; charset=windows-1251\"", 0, 0, false );
	CHtml* link = head->add( "link", "rel", "stylesheet" );
	link->attr( "type", "text/css" );
	link->attr( "href", "prstyles.css" );
	link->line( "<!-- @ -->" );
	//head->add( "<LINK REL="stylesheet" TYPE="text/css" HREF="prstyles.css"><!-- @ --></LINK>" );

	CHtml* iframe = head->add( "iframe" );
	iframe->attr( "ID", "HeaderFrame" );
	iframe->attr( "height", "0" );
	iframe->attr( "width", "0" );
	iframe->attr( "marginwidth", "0" );
	iframe->attr( "src", "header.htm" );
	//<iframe ID="HeaderFrame" height=0 width=0 marginwidth=0 src="header.htm"></iframe><BR><BR>

	CHtml* body = doc.add("body");
	CHtml* wrk;
	CHtml* table;
	int ccomment = 0;
	
	wrk = body->add( "h1" );
	wrk->str( "Плагин - " );
	wrk->str( m_plugin.Name() );
	body->add( "hr", 0, 0, false );
	
	CPArray<char> items( 2, 10, false );
	items.Add( "свойство" );
	items.Add( "значение" );
	
  CComment comm( m_plugin );
  ccomment += comment( body, comm, m_lang, t_short );
  
  table = body->table( items, 50 );
  add_row( table, m_plugin, "Автор",                 VE_PID_PL_AUTHORNAME,       m_full );
  add_row( table, m_plugin, "Идентификатор плагина", VE_PID_PL_MNEMONICID,       m_full );
  add_row( table, m_plugin, "Идентификатор автора",  VE_PID_PL_VENDORID,         m_full );
  add_row( table, m_plugin, "Кодовая страница",      VE_PID_PL_CODEPAGEID,       m_full );
  add_row( table, m_plugin, "Версия",                VE_PID_PL_VERSIONID,        m_full );
  add_row( table, m_plugin, "Проект",                VE_PID_PL_PROJECTNAME,      m_full );
	add_row( table, m_plugin, "Раздел проекта",        VE_PID_PL_SUBPROJECTNAME,   m_full );
	add_row( table, m_plugin, "Автостарт",             VE_PID_PL_AUTOSTART,        m_full );
		
	ccomment += comment( body, comm, m_lang, t_large );
	ccomment += comment( body, comm, m_lang, t_behav );
	
	if ( !ccomment && m_full )
		body->para()->styled_text( gst_nc, "Plugin has no description at all !!!" );
	
	ImpInterfaceSourceTable( body );

	CElem::lf = true;
	CElem::aligned = m_brick ? false : true;
	doc.flush( file, 0 );
	::CloseHandle( file );

	return S_OK;
}




// ---
HRESULT CCodeGen::ImpInterfaceSourceTable( CHtml* body ) {
	CHtml* row;
	HDATA  curr = m_iface.Data();

	m_iface.GoFirst();
	if ( m_iface ) {
		CPArray<char> titles( 2, 10, false );
		titles.Add( "имя интерфейса" );
		titles.Add( "описание" );
		
		body->add("h3")->line( "Интерфейсы реализованные в данном плагине" );
		CHtml* table = body->table( titles, 50 );
		while( m_iface ) {
			HPROP linked = ::DATA_Find_Prop( m_iface.Data(), 0, I(_LINKEDBYINTERFACE) );
			if ( !linked ) {
				name_gen( m_iface, false, 0, g_buff );
				row = table->add( "tr" );
				row->add("td","width","1")->href(g_buff,false)->style(gst_item_id)->str( m_iface.Name() );
				CComment comm( m_iface );
				comment( row->add("td"), comm, m_lang, t_short, false );
			}
			m_iface.GoNext();
		}
	}

	m_iface.GoFirst();
	while( curr != m_iface.Data() )
		m_iface.GoNext();

	return S_OK;
}