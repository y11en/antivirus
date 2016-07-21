#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stuff\get_opt.h>
#include <serialize\kldtser.h>
#include <datatreeutils\DTUtils.h>
#include <_AVPIO.h>
#include "scantree.h"
#include "../../AVPPveID.h"


// ---
char       cmd[0x1000];
char       exe[MAX_PATH];
char       out[MAX_PATH];
char       inb[MAX_PATH];
cRootItem* g_root_item;



// ---
int printout_winerr( int level, const char* msg, const char* fn ) {
	DWORD err = GetLastError();
	if ( err ) {
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, err, 0, cmd, sizeof(cmd), 0 );
		if ( fn && *fn )
			printf( "\n%s(winerr = 0x%08x,%d) - \"%s\"\n    ", msg, err, err, fn );
		else
			printf( "\n%s(winerr = 0x%08x,%d)\n    ", msg, err, err );
		printf( cmd );
	}
	else if ( fn && *fn )
		printf( "\n%s - \"%s\"\n    ", msg, fn );
	else
		printf( msg );
	return level;
}


// ---
int usage( int level ) {
	char* dot = strrchr( exe, '.' );
	char* slh = strrchr( exe, '\\' );
	char* bslh = strrchr( exe, '/' );

	if ( slh < bslh )
		slh = bslh;
	if ( slh )
		++slh;
	else
		slh = exe;
	if ( dot > slh )
		*dot = 0;

	printf( "Usage:\n" );
	printf( "  %s [/h|?] [/i]\"input file name\" [/q] [/v] [/d] [/o[\"output file name\"]]\n", slh );
	printf( "    h,? - usage screen\n" );
	printf( "    p   - pause after processing\n" );
	printf( "    i   - intput file name\n" );
	printf( "    q   - quiet mode\n" );
	printf( "    v   - verbose mode\n" );
	printf( "    c   - no comments\n" );
	printf( "    a   - no attributes\n" );
	printf( "    l   - do not convert include file names to lower case\n" );
	printf( "    o   - output to file. defaut value:\n" );
	printf( "            \"input file name" DEF_PRT_EXT "\" for *.prt files and\n" );
	printf( "            \"input file name" DEF_IMP_EXT "\" for *.imp files\n" );
	printf( "    d   - direct default output file to default output folder.\n" );
	printf( "            value is \"" DEF_OUT_DIR "\"\n" );
	printf( "            this key is used when /o is indicated without value\n\n" ); 
	printf( "Examples:\n" );
	printf( "   %s kernel.prt         \n       - process kernel.prt, console output\n", slh );
	printf( "   %s kernel.prt /q /o   \n       - process kernel.prt, output to \"kernel" DEF_PRT_EXT "\"\n", slh );
	printf( "   %s nfio.imp   /q /o /d\n       - process nfio.imp,   output to \"" DEF_OUT_DIR "nfio" DEF_IMP_EXT "\"\n", slh );
	return level;
}


// ---
int pausef( bool pause, int level ) {
	if ( pause ) {
		_cprintf( "strike any key..." );
		while( !_kbhit() )
			;
		char ob[3] = {0};
		ob[0] = _getch();
		ob[1] = '\n';
		_cprintf( ob );
	}
	return level;
}


// ---
void print_job( const char* inp, const char* out, bool console ) {
	char obuf[0x200] = {0,};
	if ( console )
		strcpy( obuf, "console" );
	if ( out ) {
		if ( console )
			strcat( obuf, " and " );
		strcat( obuf, "\"" );
		strcat( obuf, out );
		strcat( obuf, "\"" );
	}
	if ( !console && !out )
		strcat( obuf, "nothing" );
	printf( "\nconverting \"%s\" to %s\n", inb, obuf );
}


bool g_no_lower_fn = false;
bool g_no_comment  = false;


// ---
int main( int argc, char* argv[] ) {

	int a = -0x7fffffff;

	prepare_cmd_line( GetCommandLine(), exe, cmd );

	bool pause = !!get_option( cmd, 'p', 0,   true );
	if ( get_option(cmd,'?',0,true) || get_option(cmd,'h',0,true) )\
		return pausef( pause, usage(1) );

	bool quiet       = !!get_option( cmd, 'q', 0,   true );
	bool output      = !!get_option( cmd, 'o', out, true );
	bool verbose     = !!get_option( cmd, 'v', 0,   true );
	bool no_attrib   = !!get_option( cmd, 'a', 0,   true );
	bool def_out_dir = !!get_option( cmd, 'd', 0,   true );

	g_no_lower_fn = !!get_option( cmd, 'l', 0,   true );
	g_no_comment  = !!get_option( cmd, 'c', 0,   true );

	int ret = 0;
	if ( (!get_option(cmd,'i',inb,true) && !get_option(cmd,0,inb,true)) || !inb[0] ) {
		printf( "input file not specified\n\n" );
		return pausef( pause, usage(2) );
	}

	HDATA tree = 0;
	if ( !LoadIO() )
		return pausef( pause, printout_winerr(3,"can't load IO interface",0) );

	KLDT_Init_Library( malloc, free );
	AVP_bool res = KLDT_DeserializeUsingSWM( inb, &tree );
	if ( !res || !tree )
		return pausef( pause, printout_winerr(4,"can't deserialize input file",inb) );

	bool dsc = false;

	const char* dot = strrchr( inb, '.' );
	const char* slh = strrchr( inb, '\\' );
	const char* bslh = strrchr( inb, '/' );
	if ( slh < bslh )
		slh = bslh;
	if ( slh )
		++slh;
	else
		slh = inb;
	if ( dot < slh )
		dot = slh + strlen(slh);
	
	if ( dot && (*dot == '.') )
		dsc = !_stricmp( dot, ".dsc" );

	bool hidden = false;
	const char* def_ext = DEF_DSC_EXT;
	tNodeDsc* root = 0;
	tMode mode = em_prt;
	if ( DATA_Find_Prop(tree,0,VE_PID_PL_PLUGINNAME) ) {
		mode = em_imp;
		root = &imp_root;
		def_ext = DEF_IMP_EXT;
	}
	else if ( dsc ) {
		mode = em_dsc;
		root = &dsc_root;
		def_ext = DEF_DSC_EXT;
	}
	else {
		hidden = true;
		mode = em_prt;
		root = &prt_root;
		def_ext = DEF_PRT_EXT;
	}

	if ( output ) {
		int name_len = dot - slh;
		if ( !out[0] ) {
			if ( def_out_dir )
				strcpy( out, DEF_OUT_DIR );
			else
				strcpy( out, "." ); 
			//memcpy( out, slh, name_len );
			//memcpy( out+name_len, def_ext, strlen(def_ext)+1 );
		}
		WIN32_FIND_DATA fd = {0};
		HANDLE find = FindFirstFile( out, &fd );
		if ( find != INVALID_HANDLE_VALUE ) {
			FindClose( find );
			if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
				int out_len = strlen( out );
				if ( (out[out_len-1] != '\\') && (out[out_len-1] != '/') )
					out[out_len++] = '\\';
				memcpy( out+out_len, slh, name_len );
				memcpy( out+out_len+name_len, def_ext, strlen(def_ext)+1 );
			}
		}
	}

	print_job( inb, output ? out : 0, !quiet );

	HANDLE file = 0;
	if ( output ) {
		file = CreateFile( out, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
		if ( INVALID_HANDLE_VALUE == file )
			return pausef( pause, printout_winerr(6,"can't create output file",out) );
	}

	::DTUT_Init_Library( malloc, free );

	cRootItem node;
	g_root_item = &node;
	node.hidden( hidden );
	if ( mode == em_prt ) {
		HDATA iface = ::DATA_Get_First( tree, 0 );
		while( iface ) {
			if ( ifIFace == CBaseInfo::DetectEnType(iface) )
				node.inc( iface, false, false, false );
			iface = ::DATA_Get_Next( iface, 0 );
		}
	}

	if ( !tree ) {
		printf( "empty tree" );
		ret = 1;
	}
	else if ( !::ScanTree(node,0,mode,tree,root,true,verbose) )
		ret = printout_winerr( 5, "tree scan error", inb );

	if ( !quiet )
		node.out( 0, 0, g_no_comment, no_attrib, true, true );
	if ( file )
		node.out( file, 0, g_no_comment, no_attrib, true, true );

	return pausef( pause, ret );
}


