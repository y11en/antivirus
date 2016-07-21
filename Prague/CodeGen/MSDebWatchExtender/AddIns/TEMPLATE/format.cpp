#include <windows.h>
#include "common\custview.h"

// this function formats value (passed in dwValue and pEvalInfo parameters) into readable string
HRESULT WINAPI format_value( 
					  DWORD dwValue,        // low 32-bits of address or value
					  DEBUGHELPER *pHelper, // callback pointer to access helper functions
					  int nBase,            // decimal or hex
					  BOOL bIgnore,         // not used
					  char *pResult,        // where the result needs to go
					  size_t max,           // how large the above buffer is
					  EVAL_INFO* pEvalInfo  // internal debugger data
					  ) 
{
	// todo: write yor formatting code here

	return S_OK;
}

