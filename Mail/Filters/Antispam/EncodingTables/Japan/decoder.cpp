#include <string>
#include <iostream>
#include <stdio.h>
extern "C"{
#include "libnkfm.h"
}
#include "decoder.h"
#include <windows.h>

#include <Prague/pr_wrappers.h>

bool decode_japanese(
					 const char* input, 
					 std::string& output
					 )
{
	static PRLocker g_cs;
	PRAutoLocker _cs_(g_cs);

	static bool nkf_inited = false;
	if ( !nkf_inited )
	{
		nkf_init("-w");
		nkf_inited = true;
	}
	char *out = NULL;
	if ( out = nkf_convert_new(input) )
	{
		output = out;
		free(out);
		return true;
	}
	return false;
}
