// type_conv.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <serialize/kldtser.h>
#include <prop_util.h>
#include <AVPPveID.h>
#include <_avpio.h>
#include <stdlib.h>
#include <pr_compl.h>
#include <pr_types.h>
#include <objbase.h>
#include <stdio.h>

#define I(a) VE_PID_IF##a

/*
I(P_TYPE)
I(M_RESULTTYPE)
I(MP_TYPE)
I(T_BASETYPE)
I(C_BASETYPE)
*/

typedef struct tag_VID {
	const char* mnemonic;
	AVP_dword   id;
} VID;


VID vids[] = {
	{"VID_ANDY",         65	 },
	{"VID_ANDREW",       64	 },
	{"VID_PETROVITCH",   100 },
	{"VID_GRAF",         66	 },
	{"VID_SOBKO",        67	 },
	{"VID_COSTIN",       68	 },
	{"VID_VICTOR",       69	 },
	{"VID_MIKE",         70	 },
	{"VID_EUGENE",       71	 },
	{"VID_VASILIEV",     72	 },
	{"VID_VALIK",        73	 },
  {"VID_ALEX",         74  },
  {"VID_TAG",          75  },
  {"VID_TIM",          76  },
};

const char title[] = "PVE type checker";
char    buff[10*MAX_PATH];
int     msg_count = 0;
int     msg_plen[1000];
int     msg_dclen[1000];
char*   msg_prefix[1000];
int     msg_clen[1000];
char*   msg_cont[1000];

char    dword_name[100];
DWORD   dword_size;

char    current_file_name[MAX_PATH];
char    current_iface[MAX_PATH];

HDC     dc;
int     ten_sp;

static void   CheckResult( const WIN32_FIND_DATA* fd, const char* name, HDATA tree, BOOL changed );
static void   add_msg( int shift, const char* p, const char* c );
static void   add_msg_fmt( int shift, const char* p, const char* fmt, ... );
static void   add_named_msg( int shift, const char* p, const char* fmt, ... );
static void   show_msg();

static BOOL   ProcessFile( const char* name, HDATA tree, HDATA types );
static void   ProcessIncludes( int shift, HDATA iface, BOOL prototype, HDATA types, BOOL* changed );
static void   ProcessTypes( HDATA iface, BOOL prototype, HDATA types, BOOL* changed );
static void   ProcessConstants( HDATA iface, BOOL prototype, HDATA types, BOOL* changed );
static void   ProcessErrorCodes( HDATA iface, BOOL prototype, HDATA types, BOOL* changed );
static void   ProcessProperties( HDATA iface, BOOL prototype, HDATA types, BOOL* changed );
static void   ProcessMethods( HDATA iface, BOOL prototype, HDATA types, DWORD id, BOOL* changed );

static void   DeleteSpare( HDATA method, BOOL* changed );
static void   CheckVID( int shift, HDATA data, BOOL* changed );
static void   CheckMethodResult( HDATA data, BOOL* changed );
static void   CheckType( HDATA data, HDATA types, DWORD tid, DWORD nid, BOOL* changed );
static void   CheckEx( HDATA data, HDATA types, DWORD tid, DWORD nid, BOOL* changed );
static void   CheckNodeGUID( int shift, HDATA data, BOOL prototype, BOOL* changed );
static void   CheckNodeType( int shift, HDATA tree, BOOL* changed );
static void   CheckTypeOfType( int shift, HDATA data, HDATA types, DWORD type_name_id, BOOL* changed );
static void   ChangeUINTtoDWORD( int shift, HDATA prop, HDATA types, tDWORD tid, tDWORD nid, BOOL* changed );
  
static DWORD  GetIdByName( HDATA types, const char* name );
static DWORD  GetNameById( HDATA types, DWORD type_id, char* name );
static DWORD  GetIdByNameProp( HDATA t, const char* n, DWORD pid, DWORD tid );

static DWORD  DetectType( HDATA tree );
static BOOL   FindUserType( HDATA data, const char* type_name );
static HDATA  GetTypifiedDad( HDATA data, DWORD type_id );


// ---
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {

  char* p;
  SIZE  s;
  HWND  win = CreateWindow( "#32770", "", 0, 1, 1, 1, 1, 0, 0, hInstance, 0 );

  dc = GetDC( win );
  GetTextExtentPoint( dc, "          ", 10, &s );
  ten_sp = s.cx;

  if ( lpCmdLine && *lpCmdLine ) {

    HANDLE          f_handle;
    int             len;
    char            folder[MAX_PATH];
    WIN32_FIND_DATA fd;

    memset( &fd, 0, sizeof(fd) );
    if ( *lpCmdLine == '\"' ) {
      len = lstrlen( lpCmdLine );
      memmove( lpCmdLine, lpCmdLine+1, len-2 );
      lpCmdLine[len-2] = 0;
    }

    p = strrchr( lpCmdLine, '\\' );
    if ( p ) {
      memcpy( folder, lpCmdLine, ++p-lpCmdLine );
      folder[ p-lpCmdLine ] = 0;
    }
    else {
      len = GetCurrentDirectory( sizeof(folder), folder );
      folder[len++] = '\\';
      folder[len] = 0;
    }

    f_handle = FindFirstFile( lpCmdLine, &fd );

    if ( f_handle && f_handle != INVALID_HANDLE_VALUE ) {

      BOOL found = TRUE;
      HDATA types = 0;
      char name[MAX_PATH] = {0};
      HDATA tree = 0;

      CoInitialize( 0 );

	    AvpCloseHandle = CloseHandle;
	    AvpWriteFile   = WriteFile;
	    AvpCreateFile  = CreateFile;
	    AvpGetFileSize = GetFileSize;
	    AvpReadFile    = ReadFile;

      KLDT_Init_Library( malloc, free );

      len = lstrlen( lstrcpy(buff,"Cannot load type library file : \"") );

      GetModuleFileName( 0, buff+len, sizeof(buff)-len );
      p = strrchr( buff+len, '\\' );
      lstrcpy( p+1, "PTypeTbl.ptt" );

      if ( KLDT_DeserializeUsingSWM(buff+len,&types) ) {
        for( ; found; found = FindNextFile(f_handle,&fd) ) {
          HDATA tree = 0;

          if ( !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
            BOOL changed;

            lstrcpy( name, folder );
            lstrcat( name, fd.cFileName );

            if ( !KLDT_DeserializeUsingSWM(name,&tree) ) {
              add_msg( 0, fd.cFileName, "loading error" );
              continue;
            }

            GetNameById( types, tid_DWORD, dword_name );
            dword_size = lstrlen( dword_name ) + 1;

            changed = ProcessFile( name, tree, types );
            CheckResult( &fd, name, tree, changed );
            DATA_Remove( tree, 0 );
          }
          else if ( lstrcmp(fd.cFileName,".") && lstrcmp(fd.cFileName,"..") )
            add_msg( 0, fd.cFileName, "is a folder" );
        }
      }
      else {
        lstrcat( buff, "\"" );
        add_msg( 0, buff, 0 );
      }

      FindClose( f_handle );
      CoUninitialize();
    }
    else 
      add_msg( 0, lpCmdLine, "not found" );

  }
  else {
    GetModuleFileName( 0, buff+MAX_PATH, MAX_PATH );
    p = strrchr( buff+MAX_PATH, '\\' );
    if ( p ) {
      char* t;
      *p++ = 0;
      t = strlwr( p );
      p = strstr( p, ".exe" );
      if ( p )
        *p = 0;
      p = t;
    }
    else
      p = buff + MAX_PATH;
    add_msg_fmt( 0, "usage", "%s  filemask", p );
  }

  show_msg();
  DestroyWindow( win );

	return 0;
}




// ---
void add_msg( int shift, const char* p, const char* c ) {

  int add;
  int len;
  SIZE s;

  len = lstrlen( p );
	if ( shift ) {
		char* m = malloc( len + 2 * shift + 1 );
		memset( m, ' ', 2*shift );
		memcpy( m+2*shift, p, len+1 );
		p = m;
		len += 2 * shift;
	}
	else
		p = (const char*)memcpy( malloc(len+1), p, len+1 );

  msg_plen[ msg_count ] = len;
  GetTextExtentPoint( dc, p, len, &s );
  msg_dclen[ msg_count ] = s.cx;
  msg_prefix[ msg_count ] = (char*)p;

  if ( c ) {
    char* p;
    len = lstrlen( c );
    add = c[len-1] != '\n';
    msg_clen[ msg_count ] = len + add;
    msg_cont[ msg_count ] = p = malloc( len + add );
    memcpy( p, c, len );
    if ( add ) 
      p[len++] = '\n';
  }
  else {
    msg_clen[ msg_count ] = 0;
    msg_cont[ msg_count ] = 0;
  }
  msg_count++;
}


// ---
static void add_msg_fmt( int shift, const char* p, const char* fmt, ... ) {
	char buffer[MAX_PATH];
	vsprintf( buffer, fmt, va_start(const char*,fmt,fmt) );
	add_msg( shift, p, buffer );
}



// ---
static void show_msg() {

  int i = 0;
  int ax = 0;
  int st = 0;
  int in = 0xffff;
  char* a;
  char* p;
  char* t;
  int c = 0;

  for( ; i<msg_count; i++ ) {
    
    c += msg_plen[i];
    c += msg_clen[i];

    if ( msg_clen[i] ) {
      if ( ax < msg_dclen[i] )
        ax = msg_dclen[i];
      if ( in > msg_dclen[i] )
        in = msg_dclen[i];
    }
  }

  ax *= 10;
  ax /= ten_sp;
  ax += 4;

  if ( in == 0xffff )
    in = 0;
  else {
    in *= 10;
    in /= ten_sp;
  }

  for( i=0; i<msg_count; i++ ) {
    if ( msg_clen[i] ) {
      if ( ax > msg_plen[i] ) 
        c += ax - msg_plen[i];
    }
    else
      c++;
  }

  c += msg_count * 4;

  a = malloc( ax - in + 100 );
  memset( a, ' ', ax - in + 100 );

  t = p = malloc( c + 1 );
  for( i=0; i<msg_count; i++ ) {
    
    memcpy( t, msg_prefix[i], msg_plen[i] ); 
    t += msg_plen[i];
    
    if ( msg_clen[i] ) {
      int al = ax - ((msg_dclen[i] * 10) / ten_sp);
      memcpy( t, a, al );
      t += al; // ;

      memcpy( t, "  - ", 4 ); 
      t += 4;

      memcpy( t, msg_cont[i], msg_clen[i] );
      t += msg_clen[i];
    }
    else 
      *(t++) = '\n';
  }

  *t = 0;

  MessageBox( 0, p, title, 0 );

  free( p );
  free( a );
}


// ---
static BOOL ProcessFile( const char* name, HDATA tree, HDATA types ) {

  char*   tmp = 0;
  BOOL    changed = FALSE;
  
  BOOL    prototype = FALSE;
  HPROP   property = DATA_Find_Prop( tree, 0, 0 ); 
  HDATA   iface;
	int     shift = 0;

  lstrcpy( current_file_name, name );

  if ( property && (sizeof(prototype) == PROP_Get_Val(property,&prototype,sizeof(prototype))) ) 
    prototype = !prototype;
  else
    prototype = 0;

	add_msg( 0, name, "start processing" );
	CheckVID( 0, tree, &changed );
  CheckNodeGUID( 0, tree, prototype, &changed );
//  CheckNodeType( tree, &changed );

  iface = DATA_Get_FirstEx( tree, 0, DATA_IF_ROOT_CHILDREN );
  while( iface ) {

    GetStrPropVal( iface, 0, VE_PID_IF_INTERFACENAME, current_iface, sizeof(current_iface), 0 );

		add_msg( 1, current_iface, "start_processing" );
    CheckNodeGUID( 1, iface, prototype, &changed );
    CheckNodeType( 1, iface, &changed );

    ProcessIncludes( 1, iface, prototype, types, &changed );
    ProcessTypes( iface, prototype, types, &changed );
    ProcessConstants( iface, prototype, types, &changed );
    ProcessErrorCodes( iface, prototype, types, &changed );
    ProcessProperties( iface, prototype, types, &changed );
    
    ProcessMethods( iface, prototype, types, I(_METHODS), &changed );
    ProcessMethods( iface, prototype, types, I(_SYSTEMMETHODS), &changed );

    iface = DATA_Get_Next( iface, 0 );
  }

  return changed;
}



// ---
static void CheckResult( const WIN32_FIND_DATA* fd, const char* name, HDATA tree, BOOL changed ) {
  if ( changed ) {

    if ( fd->dwFileAttributes & FILE_ATTRIBUTE_READONLY ) 
      add_msg( 0, fd->cFileName, "has to be changed but it is RO" );

    else {

      char*   tmp = 0;
      int     len;
      HANDLE  file = CreateFile( name, GENERIC_READ, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );

      if ( file && file != INVALID_HANDLE_VALUE ) {
        AVP_KL_Header KLHeader;
        KLDT_DeserializeFullHeaders( file, &KLHeader, &tmp );
        CloseHandle( file );
      }
      else {
        //add_msg( 0, name, "cannot open file" );
      }
      
      len = lstrlen( name );
      memcpy( buff, name, len+1 );

      if ( lstrcmpi(buff+(len-4),".bak") ) {
        DWORD attrs;

        lstrcat( buff, ".bak" );
        attrs = GetFileAttributes( buff );
        if ( attrs == ((DWORD)~0) )
          attrs = 0;
        if ( attrs & FILE_ATTRIBUTE_READONLY )
          SetFileAttributes( buff, (attrs & (~FILE_ATTRIBUTE_READONLY)) );
        MoveFileEx( name, buff, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
      }

      if ( KLDT_SerializeUsingSWM(name,tree,tmp) ) 
        add_msg( 0, fd->cFileName, "sucessully checked and converted" );
      else
        add_msg( 0, fd->cFileName, "has to be changed but cannot be saved" );
    }
  }
  else 
    add_msg( 0, fd->cFileName, "is fine" );
  
}




// ---
static void ProcessIncludes( int shift, HDATA iface, BOOL prototype, HDATA types, BOOL* changed ) {
  static AVP_char val[200];
  AVP_char* p;
  AVP_dword i, c;

  HPROP inc = DATA_Find_Prop( iface, 0, VE_PID_IF_INCLUDENAMES );
  if ( inc && GET_AVP_PID_APP(PROP_Get_Id(inc)) ) {
    c = PROP_Arr_Count( inc );
    for( i=c-1; ((AVP_int)i)>=0; i-- ) {
      if ( PROP_Arr_Get_Items(inc,i,val,sizeof(val)) ) {
        p = val;
        while( *p && (*p == ' ') )
          p++;
        if ( !*p || (*p == '{') ) {
          *changed = TRUE;
          PROP_Arr_Remove( inc, i, 1 );
					add_msg( shift, val, "removed" );
        }
      }
    }
  }
  
  inc = DATA_Find_Prop( iface, 0, VE_PID_IF_INCLUDEUIDS );
  if ( inc && GET_AVP_PID_APP(PROP_Get_Id(inc)) ) {
    c = PROP_Arr_Count( inc );
    for( i=c-1; ((AVP_int)i)>=0; i-- ) {
      if ( PROP_Arr_Get_Items(inc,i,val,sizeof(val)) ) {
        p = val;
        while( *p && (*p == ' ') )
          p++;
        if ( !*p || (*p != '{') ) {
          *changed = TRUE;
          PROP_Arr_Remove( inc, i, 1 );
					add_msg( shift, val, "removed" );
        }
      }
    }
  }
  
}



// ---
static void ProcessTypes( int shift, HDATA iface, BOOL prototype, HDATA types, BOOL* changed ) {
  tDWORD addr[] = { I(_TYPES), 0 };
  HDATA prop = DATA_Find( iface, addr );
  
  if ( prop ) {
    prop = DATA_Get_FirstEx( prop, 0, DATA_IF_ROOT_CHILDREN );
    while( prop ) {
      CheckNodeGUID( shift+1, prop, prototype, changed );
      CheckType( shift+1, prop, types, I(T_BASETYPE), I(T_BASETYPENAME), changed );
      CheckEx( shift+1, prop, types, I(T_BASETYPE), I(T_BASETYPENAME), changed );
      CheckNodeType( shift+1, prop, changed );
      CheckTypeOfType( shift+1, prop, types, I(T_BASETYPENAME), changed );
      ChangeUINTtoDWORD( shift+1, prop, types, I(T_BASETYPE), I(T_BASETYPENAME), changed );
      prop = DATA_Get_Next( prop, 0 );
    }
  }

}



// ---
static void ProcessConstants( int shift, HDATA iface, BOOL prototype, HDATA types, BOOL* changed ) {
  tDWORD addr[] = { I(_CONSTANTS), 0 };
  HDATA prop = DATA_Find( iface, addr );
  
  if ( prop ) {
    prop = DATA_Get_FirstEx( prop, 0, DATA_IF_ROOT_CHILDREN );
    while( prop ) {
      CheckNodeGUID( shift+1, prop, prototype, changed );
      CheckEx( shift+1, prop, types, I(C_BASETYPE), I(C_BASETYPENAME), changed );
      CheckNodeType( shift+1, prop, changed );
      CheckTypeOfType( shift+1, prop, types, I(C_BASETYPENAME), changed );
      ChangeUINTtoDWORD( shift+1, prop, types, I(C_BASETYPE), I(C_BASETYPENAME), changed );
      prop = DATA_Get_Next( prop, 0 );
    }
  }
  
}



// ---
static void ProcessErrorCodes( int shift, HDATA iface, BOOL prototype, HDATA types, BOOL* changed ) {
  tDWORD addr[] = { I(_ERRORCODES), 0 };
  HDATA prop = DATA_Find( iface, addr );
  
  if ( prop ) {
    prop = DATA_Get_FirstEx( prop, 0, DATA_IF_ROOT_CHILDREN );
    while( prop ) {
      CheckNodeGUID( shift+1, prop, prototype, changed );
      CheckNodeType( shift+1, prop, changed );
      prop = DATA_Get_Next( prop, 0 );
    }
  }
}



// ---
static void ProcessProperties( int shift, HDATA iface, BOOL prototype, HDATA types, BOOL* changed ) {
  tDWORD addr[] = { I(_PROPERTIES), 0 };
  HDATA prop = DATA_Find( iface, addr );
  
  if ( prop ) {
    prop = DATA_Get_FirstEx( prop, 0, DATA_IF_ROOT_CHILDREN );
    while( prop ) {
      CheckNodeGUID( shift+1, prop, prototype, changed );
      CheckEx( shift+1, prop, types, I(P_TYPE), I(P_TYPENAME), changed );
      CheckNodeType( shift+1, prop, changed );
      CheckTypeOfType( shift+1, prop, types, I(P_TYPENAME), changed );
      ChangeUINTtoDWORD( shift+1, prop, types, I(P_TYPE), I(P_TYPENAME), changed );
      prop = DATA_Get_Next( prop, 0 );
    }
  }
}



// ---
static void ProcessMethods( int shift, HDATA iface, BOOL prototype, HDATA types, DWORD id, BOOL* changed ) {

  DWORD addr[2] = { id, 0 };
  HDATA method = DATA_Find( iface, addr );

  if ( method ) {
    method = DATA_Get_FirstEx( method, 0, DATA_IF_ROOT_CHILDREN );

    while( method ) {
      HDATA param;

      CheckNodeGUID( shift+1, method, prototype, changed );
      CheckMethodResult( shift+1, method, changed );
      CheckType( shift+1, method, types, I(M_RESULTTYPE), I(M_RESULTTYPENAME), changed );
      CheckEx( shift+1, method, types, I(M_RESULTTYPE), I(M_RESULTTYPENAME), changed );
      CheckNodeType( shift+1, method, changed );
      CheckTypeOfType( shift+1, method, types, I(M_RESULTTYPENAME), changed );
      ChangeUINTtoDWORD( shift+1, method, types, I(M_RESULTTYPE), I(M_RESULTTYPENAME), changed );
      DeleteSpare( shift+1, method, changed );

      param = DATA_Get_FirstEx( method, 0, DATA_IF_ROOT_CHILDREN );
      while( param ) {
        CheckNodeGUID( shift+2, param, prototype, changed );
        CheckType( shift+2, param, types, I(MP_TYPE), I(MP_TYPENAME), changed );
        CheckEx( shift+2, param, types, I(MP_TYPE), I(MP_TYPENAME), changed );
        CheckNodeType( shift+2, param, changed );
        CheckTypeOfType( shift+2, param, types, I(MP_TYPENAME), changed );
        ChangeUINTtoDWORD( shift+2, param, types, I(MP_TYPE), I(MP_TYPENAME), changed );
        param = DATA_Get_Next( param, 0 );
      }

      method = DATA_Get_Next( method, 0 );
    }
  }

}



#define VE_PID_IFM_OLD_SHORT_ID (302)
#define VE_PID_IFM_OLD_SHORT    MAKE_AVP_PID(VE_PID_IFM_OLD_SHORT_ID,VE_APP_ID,avpt_str,0)

#define VE_PID_IFM_OLD_LARGE_ID (303)
#define VE_PID_IFM_OLD_LARGE    MAKE_AVP_PID(VE_PID_IFM_OLD_LARGE_ID,VE_APP_ID,avpt_str,0)

#define VE_PID_IFM_OLD_BEHAV_ID (304)
#define VE_PID_IFM_OLD_BEHAV    MAKE_AVP_PID(VE_PID_IFM_OLD_BEHAV_ID,VE_APP_ID,avpt_str,0)

#define VE_PID_IFM_OLD_RESULT_ID (305)
#define VE_PID_IFM_OLD_RESULT    MAKE_AVP_PID(VE_PID_IFM_OLD_RESULT_ID,VE_APP_ID,avpt_str,0)

#define VE_PID_IFMP_PARAMETER_COMMENT_ID		(404)
#define VE_PID_IFMP_PARAMETER_COMMENT				MAKE_AVP_PID(VE_PID_IFMP_PARAMETER_COMMENT_ID,VE_APP_ID,avpt_str,0)


// ---
static void DeleteSpare( int shift, HDATA method, BOOL* changed ) {

  HDATA param;
  HPROP prop;
  
  prop = DATA_Find_Prop( method, 0, I(M_OLD_SHORT) );
  if ( prop ) {
    *changed = TRUE;
    DATA_Remove_Prop_H( method, 0, prop );
		add_msg( shift, "VE_PID_IFM_OLD_SHORT comment", "removed" );
  }

  prop = DATA_Find_Prop( method, 0, I(M_OLD_LARGE) );
  if ( prop ) {
    *changed = TRUE;
    DATA_Remove_Prop_H( method, 0, prop );
		add_msg( shift, "VE_PID_IFM_OLD_LARGE comment", "removed" );
  }

  prop = DATA_Find_Prop( method, 0, I(M_OLD_BEHAV) );
  if ( prop ) {
    *changed = TRUE;
    DATA_Remove_Prop_H( method, 0, prop );
		add_msg( shift, "VE_PID_IFM_OLD_BEHAV comment", "removed" );
  }

  prop = DATA_Find_Prop( method, 0, I(M_OLD_RESULT) );
  if ( prop ) {
    *changed = TRUE;
    DATA_Remove_Prop_H( method, 0, prop );
		add_msg( shift, "VE_PID_IFM_OLD_RESULT", "removed" );
  }

  param = DATA_Get_FirstEx( method, 0, DATA_IF_ROOT_CHILDREN );
  while( param ) {
    prop = DATA_Find_Prop( param, 0, I(MP_PARAMETER_COMMENT) );
    if ( prop ) {
      *changed = TRUE;
      DATA_Remove_Prop_H( param, 0, prop );
			add_msg( shift, "VE_PID_IFMP_PARAMETER_COMMENT", "removed" );
    }
    param = DATA_Get_Next( param, 0 );
  }

}





// ---
static void CheckVID( int shift, HDATA data, BOOL* changed ) {
	AVP_dword id;
	char buff[MAX_PATH];
	if ( GetDwordPropVal(data,0,VE_PID_PL_VENDORID,&id) ) {
		int i;
		for( i=0; i<sizeof(vids)/sizeof(vids[0]); i++ ) {
			if ( vids[i].id == id ) {
				memset( buff, 0, sizeof(buff) );
				GetStrPropVal( data, 0, VE_PID_PL_VENDORMNEMONICID, buff, sizeof(buff), 0 );
				if ( lstrcmp(buff,vids[i].mnemonic) ) {
					DATA_Set_Val( data, 0, VE_PID_PL_VENDORMNEMONICID, (AVP_dword)vids[i].mnemonic, lstrlen(vids[i].mnemonic) );
					add_msg( shift, buff, "")
					*changed = TRUE;
				}
				return;
			}
		}
		wsprintf( buff, "%d", id );
		add_msg( shift, buff, "VENDOR ID is unknown" );
	}
	else
		add_msg( shift, "cannot find VENDOR ID in the plugin properties", 0 );
}





// ---
static void CheckMethodResult( int shift, HDATA data, BOOL* changed ) {
  AVP_dword id = 0;
  char buff[MAX_PATH];

  if ( GetDwordPropVal(data,0,I(M_RESULTTYPE),&id) ) {
    if ( id == tid_VOID ) {
      id = tid_ERROR;
      if ( sizeof(id) == DATA_Set_Val(data,0,I(M_RESULTTYPE),id,sizeof(id)) ) {
				add_msg( shift, data, "result type has been changed" )
        *changed = TRUE;
			}
    }
  }

  if ( GetStrPropVal(data,0,I(M_RESULTTYPENAME),buff,sizeof(buff),0) ) {
    if ( !lstrcmp(buff,"tVOID") ) {
      if ( 7 == DATA_Set_Val(data,0,I(M_RESULTTYPENAME),(AVP_dword)"tERROR",7) )
        *changed = TRUE;
    }
  }
}



// ---
static void CheckType( HDATA data, HDATA types, DWORD type_id, DWORD type_name_id, BOOL* changed ) {
  char buff[MAX_PATH];
  if ( GetStrPropVal(data,0,type_name_id,buff,sizeof(buff),0) ) {
    HPROP prop = DATA_Find_Prop( data, 0, type_id );
    if ( prop ) {
      DWORD id0, id1;
      PROP_Get_Val( prop, &id0, sizeof(id0) );
      id1 = GetIdByName( types, buff );
      if ( id1 && (id0 != id1) && (sizeof(id1) == DATA_Set_Val(data,0,type_id,id1,sizeof(id1))) ) 
        *changed = TRUE;
    }
  }
}




// ---
static void ChangeUINTtoDWORD( HDATA data, HDATA types, DWORD type_id, DWORD type_name_id, BOOL* changed ) {
  HPROP prop = DATA_Find_Prop( data, 0, type_id );
  if ( prop ) {
    DWORD t_id;
    PROP_Get_Val( prop, &t_id, sizeof(t_id) );
    if ( t_id == tid_UINT ) {
      DWORD t_o_t;
      HPROP t_o_t_prop = DATA_Find_Prop( data, 0, VE_PID_NODETYPETYPE );
      if ( t_o_t_prop && (sizeof(t_o_t) == PROP_Get_Val(t_o_t_prop,&t_o_t,sizeof(t_o_t))) && (t_o_t == VE_NTT_GLOBALTYPE) ) { 
        if ( sizeof(tDWORD) == PROP_Set_Val(prop,tid_DWORD,sizeof(tDWORD)) ) {
          HPROP type_name = DATA_Find_Prop( data, 0, type_name_id );
          if ( type_name )
            PROP_Set_Val( type_name, (DWORD)dword_name, dword_size );
          else
            DATA_Add_Prop( data, 0, type_name_id, (DWORD)dword_name, dword_size );
          *changed = TRUE;
        }
      }
    }
  }
}



// ---
static HDATA GetTypifiedDad( HDATA data, DWORD type_id ) {
  HDATA dad = DATA_Get_Dad( data, 0 );
  while( dad && (type_id != DetectType(dad)) )
    dad = DATA_Get_Dad( dad, 0 );
  return dad;
}



// ---
static BOOL FindUserType( HDATA data, const char* type_name ) {
  HDATA iface = GetTypifiedDad( data, VE_NT_INTERFACE );
  if ( iface ) {
    DWORD addr[] = { I(_TYPES), 0 };
    HDATA type = DATA_Find( iface, addr );
    if ( type ) {
      type = DATA_Get_FirstEx( type, 0, DATA_IF_ROOT_CHILDREN );
      while( type ) {
        if ( 1 < DATA_Get_Val(type,0,VE_PID_IFT_TYPENAME,buff,sizeof(buff)) ) {
          if ( !lstrcmp(type_name,buff) )
            return TRUE;
        }
        type = DATA_Get_Next( type, 0 );
      }
    }
  }
  return FALSE;
}



// ---
static void CheckTypeOfType( HDATA data, HDATA types, DWORD type_name_id, BOOL* changed ) {
  DWORD type;
  DWORD type_id;
  HPROP prop;
  char  buff[MAX_PATH];

  if ( !GetStrPropVal(data,0,type_name_id,buff,sizeof(buff),0) )
    return;

  type_id = GetIdByName( types, buff );
  if ( !type_id )
    type_id = GetIdByNameProp( types, buff, TT_PID_ROOT, TT_PID_TYPEID );

  prop = DATA_Find_Prop( data, 0, VE_PID_NODETYPETYPE );
  if ( prop && (sizeof(type) == PROP_Get_Val(prop,&type,sizeof(type))) ) {
    if ( type_id ) { // type id found - it is global type 
      if ( type == VE_NTT_GLOBALTYPE )
        return;
    }
    else if ( (type == VE_NTT_USERTYPE) || (type == VE_NTT_INTERFACE) )
      return;
  }

  if ( type_id == 0 ) { // type id not found - it is not global type 
    if ( FindUserType(data,buff) )
      type = VE_NTT_USERTYPE;
    else {
      DWORD node_type;
      HDATA dad;
      static BOOL do_not_ask = 0;
      static char msg[1000];
      static char add[200];
      static char fmt[] = 
        "Cannot give a type of type for:\n"
        "File name \t- \"%s\"\n"
        "IFace name\t- \"%s\"\n%s"
        "Type name \t- \"%s\"\n\n"
        "To process click:\n"
        "\t\"YES\"\t- VE_NTT_USERTYPE (interface defined type)\n"
        "\t\"NO\"\t- VE_NTT_INTERFACE (another interface)\n"
        "\t\"CANCEL\"\t- I don't know(and do not process it)";

      if ( do_not_ask )
        return;

      add[0] = 0;
      node_type = DetectType( data );
      switch( node_type ) {

        case VE_NT_PLUGIN    :
        case VE_NT_INTERFACE : 
          break; 

        case VE_NT_PROPERTY : 
          DATA_Get_Val( data, 0, I(P_PROPNAME), msg, sizeof(msg) );
          wsprintf( add, "Property name\t- \"%s\"\n", msg );
          break;

        case VE_NT_METHOD : 
          DATA_Get_Val( data, 0, I(M_METHODNAME), msg, sizeof(msg) );
          wsprintf( add, "PubMethod name\t- \"%s\" (return value)\n", msg );
          break;

        case VE_NT_METHODPARAM :
          dad = GetTypifiedDad( data, VE_NT_METHOD );
          if ( dad ) {
            DATA_Get_Val( dad, 0, I(M_METHODNAME), msg, sizeof(msg)/2 );
            DATA_Get_Val( data, 0, I(MP_PARAMETERNAME), msg+sizeof(msg)/2, sizeof(msg)/2 );
            wsprintf( add, "PubMethod name\t- \"%s\"\nParamName\t- \"%s\"\n", msg, msg+sizeof(msg)/2 );
          }
          break;

        case VE_NT_DATASTRUCTURE : 
        case VE_NT_DATASTRUCTUREMEMBER : 
          break;

        case VE_NT_TYPE : 
          DATA_Get_Val( data, 0, I(T_TYPENAME), msg, sizeof(msg) );
          wsprintf( add, "Typedef name\t- \"%s\"\n", msg );
          break;

        case VE_NT_CONSTANT : 
          DATA_Get_Val( data, 0, I(C_CONSTANTNAME), msg, sizeof(msg) );
          wsprintf( add, "Constant name\t- \"%s\"\n", msg );
          break;

        case VE_NT_SYSTEMMETHOD : 
          DATA_Get_Val( data, 0, I(M_METHODNAME), msg, sizeof(msg) );
          wsprintf( add, "IntMethod name\t- \"%s\" (return value)\n", msg );
          break;

        case VE_NT_SYSTEMMETHODPARAM : 
          dad = GetTypifiedDad( data, VE_NT_SYSTEMMETHOD );
          if ( dad ) {
            DATA_Get_Val( dad, 0, I(M_METHODNAME), msg, sizeof(msg)/2 );
            DATA_Get_Val( data, 0, I(MP_PARAMETERNAME), msg+sizeof(msg)/2, sizeof(msg)/2 );
            wsprintf( add, "IntMethod name\t- \"%s\"\nParamName\t- \"%s\"\n", msg, msg+sizeof(msg)/2 );
          }
          break;

        case VE_NT_ERRORCODE :
          DATA_Get_Val( data, 0, I(E_ERRORCODENAME), msg, sizeof(msg) );
          wsprintf( add, "Error name\t- \"%s\"\n", msg );
          break;

      }

      wsprintf( msg, fmt, current_file_name, current_iface, add, buff );
      type = MessageBox( 0, msg, title, MB_YESNOCANCEL );
      if ( type == IDYES )
        type = VE_NTT_USERTYPE;
      else if ( type == IDNO )
        type = VE_NTT_INTERFACE;
      else {
        do_not_ask = (IDNO == MessageBox( 0, "Would you like to process another types ?", title, MB_YESNO ));
        return;
      }
    }
  }
  else
    type = VE_NTT_GLOBALTYPE;

  if ( !prop ) {
    if ( 0 != DATA_Add_Prop(data,0,VE_PID_NODETYPETYPE,type,sizeof(type)) ) 
      *changed = TRUE;
  }
  else if ( sizeof(type) == PROP_Set_Val(prop,type,sizeof(type)) ) 
    *changed = TRUE;
}



// ---
static void CheckEx( HDATA data, HDATA types, DWORD type_id, DWORD type_name_id, BOOL* changed ) {

  DWORD id;
  char  rn[MAX_PATH];
  char  tn[MAX_PATH];

  if ( GetDwordPropVal(data,0,type_id,&id) && GetNameById(types,id,rn) && GetStrPropVal(data,0,type_name_id,tn,sizeof(tn),0) && lstrcmp(rn,tn) ) {
    HPROP prop = DATA_Find_Prop( data, 0, type_name_id );
    if ( prop ) {
      PROP_Set_Val( prop, (AVP_dword)rn, lstrlen(rn)+1 );
      *changed = TRUE;
    }
  }
}




// ---
static DWORD GetNameById( HDATA t, DWORD type_id, char* name ) {

  DWORD addr[2];

  addr[0] = TT_PID_ROOT;
  addr[1] = 0;

  t = DATA_Find( t, addr );
  if ( t ) {
    HPROP type_ids  = DATA_Find_Prop( t, 0, TT_PID_TYPEID );
    HPROP type_defs = DATA_Find_Prop( t, 0, 0 ); // TT_PID_DEF
    //HPROP type_defs = DATA_Find_Prop( t, 0, tname ); // TT_PID_DEF

    if ( type_ids && type_defs ) {
      AVP_dword i;
      AVP_dword id = 0;
      AVP_dword c = PROP_Arr_Count( type_defs );

      for( i=0; i<c; i++ ) {
        PROP_Arr_Get_Items( type_ids, i, &id, sizeof(id) );

        if ( id == type_id ) 
          return PROP_Arr_Get_Items( type_defs, i, name, MAX_PATH );
      }
    }
  }

  return 0;
}




// ---
static DWORD GetIdByNameProp( HDATA t, const char* n, DWORD pid, DWORD tid ) {

  DWORD addr[2] = { pid, 0 };

  t = DATA_Find( t, addr );
  if ( t ) {
    HPROP type_ids  = DATA_Find_Prop( t, 0, tid );
    HPROP type_defs = DATA_Find_Prop( t, 0, 0 ); // TT_PID_DEF
    //HPROP type_defs = DATA_Find_Prop( t, 0, tname ); // TT_PID_DEF

    if ( type_ids && type_defs ) {
      AVP_dword id = 0;
      char b[MAX_PATH];
  
      AVP_dword i;
      AVP_dword c = PROP_Arr_Count( type_defs );

      for( i=0; i<c; i++ ) {
        PROP_Arr_Get_Items( type_defs, i, b, sizeof(b) );

        if ( !lstrcmp(n,b) ) {
          PROP_Arr_Get_Items( type_ids, i, &id, sizeof(id) );
          return id;
        }
      }
    }
  }

  return 0;
}




// ---
static DWORD GetIdByName( HDATA types, const char* name ) {
  AVP_dword id = GetIdByNameProp( types, name, PT_PID_ROOT, PT_PID_TYPEID );
  if ( id == 0 )
    id = GetIdByNameProp( types, name, HP_PID_ROOT, HP_PID_TYPEID );
  if ( id == 0 )
    id = GetIdByNameProp( types, name, SP_PID_ROOT, SP_PID_TYPEID );
  return id;
}



// ---
static void CheckNodeGUID( HDATA data, BOOL prototype, BOOL* changed ) {
  if ( prototype ) {
    HPROP guid = DATA_Find_Prop( data, 0, VE_PID_UNIQUEID );
    if ( !guid ) {
      GUID g;
      wchar_t ws[40];
      char s[40];
      if ( SUCCEEDED(CoCreateGuid(&g)) && StringFromGUID2(&g,ws,sizeof(ws)/sizeof(ws[0])) ) {
        AVP_dword l = wcstombs( s, ws, sizeof(ws)/sizeof(ws[0]) );
        if ( l && DATA_Add_Prop(data,0,VE_PID_UNIQUEID,(AVP_dword)s,l+1) )
          *changed = TRUE;
      }
    }
  }
}




// ---
static void CheckNodeType( HDATA tree, BOOL* changed ) {
  AVP_dword type;
  HPROP type_prop = DATA_Find_Prop( tree, 0, VE_PID_NODETYPE );
  if ( type_prop && (sizeof(type)==PROP_Get_Val(type_prop,&type,sizeof(type))) ) {
    switch( type ) {
      case VE_NT_PLUGIN               : 
      case VE_NT_INTERFACE            : 
      case VE_NT_PROPERTY             : 
      case VE_NT_METHOD               : 
      case VE_NT_METHODPARAM          : 
      case VE_NT_DATASTRUCTURE        : 
      case VE_NT_DATASTRUCTUREMEMBER  : 
      case VE_NT_TYPE                 : 
      case VE_NT_CONSTANT             : 
      case VE_NT_SYSTEMMETHOD         : 
      case VE_NT_SYSTEMMETHODPARAM    : 
      case VE_NT_ERRORCODE            : return;
      default: 
        add_msg( "Node has unknown type", 0 );
    }
  }

  type = DetectType( tree );

  if ( type && (~0 != type) ) {
    if ( type_prop )
      *changed = ( sizeof(type) == PROP_Set_Val(type_prop,type,sizeof(type)) );
    else
      *changed = ( 0 != DATA_Add_Prop(tree,0,VE_PID_NODETYPE,type,sizeof(type)) );
  }
}



// ---
static DWORD DetectType( HDATA tree ) {
  DWORD type;
  HPROP type_prop;
  if ( !tree )
    return ~0;
  type_prop = DATA_Find_Prop( tree, 0, VE_PID_NODETYPE );
  if ( type_prop && (sizeof(type)==PROP_Get_Val(type_prop,&type,sizeof(type))) ) {
    switch( type ) {
      case VE_NT_PLUGIN               : 
      case VE_NT_INTERFACE            : 
      case VE_NT_PROPERTY             : 
      case VE_NT_METHOD               : 
      case VE_NT_METHODPARAM          : 
      case VE_NT_DATASTRUCTURE        : 
      case VE_NT_DATASTRUCTUREMEMBER  : 
      case VE_NT_TYPE                 : 
      case VE_NT_CONSTANT             : 
      case VE_NT_SYSTEMMETHOD         : 
      case VE_NT_SYSTEMMETHODPARAM    : 
      case VE_NT_ERRORCODE            : return type;
    }
  }
  if ( DATA_Find_Prop(tree,0,VE_PID_PL_PLUGINNAME) )
    return VE_NT_PLUGIN;
  else if ( DATA_Find_Prop(tree,0,I(_INTERFACENAME)) )
    return VE_NT_INTERFACE;
  else if ( DATA_Find_Prop(tree,0,I(T_TYPENAME)) )
    return VE_NT_TYPE;
  else if ( DATA_Find_Prop(tree,0,I(C_CONSTANTNAME)) )
    return VE_NT_CONSTANT;
  else if ( DATA_Find_Prop(tree,0,I(E_ERRORCODENAME)) )
    return VE_NT_ERRORCODE;
  else if ( DATA_Find_Prop(tree,0,I(P_PROPNAME)) )
    return VE_NT_PROPERTY;
  else if ( DATA_Find_Prop(tree,0,I(M_METHODNAME)) ) {
    HDATA dad = DATA_Get_Dad( tree, 0 );
    if ( dad ) {
      DWORD id = DATA_Get_Id( dad, 0 );
      if ( id == I(_SYSTEMMETHODS) )
        return VE_NT_SYSTEMMETHOD;
      else if ( id == I(_METHODS) )
        return VE_NT_METHOD;
    }
    return VE_NT_METHOD;
  }
  else if ( DATA_Find_Prop(tree,0,I(MP_PARAMETERNAME)) ) {
    DWORD dad_type = DetectType( DATA_Get_Dad(tree,0) );
    if ( dad_type == VE_NT_SYSTEMMETHOD ) 
      return VE_NT_SYSTEMMETHODPARAM;
    else if ( dad_type == VE_NT_METHOD )
      return VE_NT_METHODPARAM;
    else
      return VE_NT_METHODPARAM;
  }
  else if ( DATA_Find_Prop(tree,0,I(D_DATASTRUCTURENAME)) )
    return VE_NT_DATASTRUCTURE;
  else if ( DATA_Find_Prop(tree,0,I(DM_MEMBERNAME)) )
    return VE_NT_DATASTRUCTUREMEMBER;
  else 
    return ~0;
}





