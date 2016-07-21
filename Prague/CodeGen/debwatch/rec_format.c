#include "custview.h"


// ---
typedef struct tag_Format {
	char*          id;
	fnCustomViewer fn;
} tFormat;


// ---
tFormat formats[] = {
	{ "hr",   hr_format_val },
	{ "err",  err_format_val },
	{ "iid",  iid_format_val },
	{ "pid",  pid_format_val },
	{ "cp",   cp_format_val },
	{ "prop", prop_format_val },
	{ "obj",  phOBJECT },
	{ "str",  phString },
	{ "hdl",  phandle }, 
	{ "rng",  str_range_format_val },
	{ "ser",  serializableId },
	{ "heap", phHeap },
	{ "dt",   dt_format },
	{ "time", local_time_format_val },
	{ "ltime", local_time_format_val },
	{ "gtime", gmt_time_format_val },
};



// ---
fnCustomViewer __stdcall  RecognizeFormat(char* szFormat) {
	UINT i;
	
	for( i=0; i<countof(formats); i++ ) {
		if ( 0 == lstrcmp(szFormat,formats[i].id) )
			return formats[i].fn;
	}

	return NULL;
}



