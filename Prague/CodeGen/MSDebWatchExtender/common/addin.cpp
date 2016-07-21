// AddIn.cpp : Defines the entry point for the MSDev Evaluator AddIn DLL.

// ***********************************************************************
// This file contain base methods for AddIn. You need to implement only 
// format_value function in format.cpp file.
// ***********************************************************************

#include <windows.h>
#include <stdio.h>
#include "custview.h"
#include "../../kernel/kernel.h"
#include <iface/i_string.h>



// ---
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved ) {
	switch (ul_reason_for_call)	{
	  case DLL_PROCESS_ATTACH:
		  // do some initialization work here
	  default:
		  break;
	}
  return TRUE;
}



// ---
HRESULT WINAPI common_format( 
  fnCustomViewer pCustomViewer, // formatting routine
  DWORD dwValue,        // low 32-bits of address or value
  DEBUGHELPER *pHelper, // callback pointer to access helper functions
  int nBase,            // decimal or hex
  BOOL bIgnore,         // not used
  char *pResult,        // where the result needs to go
  size_t max,           // how large the above buffer is
  EVAL_INFO* pEvalInfo  // internal debugger data
  ) 
{
	CHAR szFormatResult[0x200];
	CHAR szValue[0x40];
	CHAR szPointedValue[0x40];
	DWORD dwAddress = dwValue;
	DWORD dwErr;
	
	szValue[0] = 0;
	szPointedValue[0] = 0;
	
	switch (nBase)
	{
	case 16:
		wsprintf(szValue, "0x%08x", dwValue);
		break;
	case 10:
		wsprintf(szValue, "%d", dwValue);
		break;
	default:
		break;
	}
	
	__try {
		
		if (!pEvalInfo) // EECXX.DLL must be patched
		{
			_snprintf(pResult, max, "%s, (EECXX.DLL is incompatible)", szValue);
			return S_OK;
		}
		
		dwAddress = pEvalInfo->dwAddress;

		if (dwAddress == 0)
		{
			if (pEvalInfo->dwRegisterIdx!=0)
				dwValue = pEvalInfo->dwValue[0];
			else
			if (pEvalInfo->dwUnknown0 == 3) // const?
				dwValue = pEvalInfo->dwValue[0];
		}
		else
		{
			if ( FAILED( pHelper->ReadDebuggeeMemory( pHelper, dwAddress, sizeof(dwValue), &dwValue, &dwErr ) ) )
			{
				_snprintf(pResult, max, "%s, cannot retrive value", szValue);
				return S_OK;
			}
			
			if ( dwValue == 0xcccccccc )
			{
				_snprintf(pResult, max, "%s, not initialized", szValue);
				return S_OK;
			}
			
			if (pEvalInfo->SymType.ptr == ePTR_NearPtr32)
			{
				dwAddress = dwValue;
				
				if ( FAILED( pHelper->ReadDebuggeeMemory( pHelper, dwAddress, sizeof(dwValue), &dwValue, &dwErr ) ) )
				{
					_snprintf(pResult, max, "%s, cannot retrive value", szValue);
					return S_OK;
				}
				
				switch (nBase)
				{
				case 16:
					wsprintf(szPointedValue, "0x%08x", dwValue);
					break;
				case 10:
					wsprintf(szPointedValue, "%d", dwValue);
					break;
				default:
					break;
				}
				
			}
		}
		
		if ( dwValue == 0xcccccccc )
		{
			_snprintf(pResult, max, "*%s = %s, not initialized", szValue, szPointedValue);
			return S_OK;
		}
		
		// format value into szFormatResult
		if ( FAILED( pCustomViewer(dwValue, pHelper, nBase, bIgnore, szFormatResult, sizeof(szFormatResult), pEvalInfo) ) )
			_snprintf(szFormatResult, sizeof(szFormatResult), "cannot format data");
		
		if (!*szValue)
			_snprintf(pResult, max, "%s", szFormatResult);
		else
		{
			if (!*szPointedValue)
				_snprintf(pResult, max, "%s, %s", szValue, szFormatResult);
			else
				_snprintf(pResult, max, "*%s = %s, %s", szValue, szPointedValue, szFormatResult);
		}
		return S_OK;
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) 
	{
		lstrcpyn( pResult, "cannot format data, exception occured", max );
		return S_OK;
	}
	return S_OK;
}

HRESULT WINAPI format_value( 
					  DWORD dwValue,        // low 32-bits of address or value
					  DEBUGHELPER *pHelper, // callback pointer to access helper functions
					  int nBase,            // decimal or hex
					  BOOL bIgnore,         // not used
					  char *pResult,        // where the result needs to go
					  size_t max,           // how large the above buffer is
					  EVAL_INFO* pEvalInfo  // internal debugger data
            );

// this function formats value (passed in dwValue and pEvalInfo parameters) into readable string
extern "C" __declspec(dllexport) 		
HRESULT WINAPI format( 
                      DWORD dwValue,        // low 32-bits of address or value
                      DEBUGHELPER *pHelper, // callback pointer to access helper functions
                      int nBase,            // decimal or hex
                      BOOL bIgnore,         // not used
                      char *pResult,        // where the result needs to go
                      size_t max,           // how large the above buffer is
                      EVAL_INFO* pEvalInfo  // internal debugger data
                      ) 
{
  return common_format(format_value, dwValue, pHelper, nBase, bIgnore, pResult, max, pEvalInfo);
}




char src_buff[0x1000];
char local_buff[0x1000];


// ---
HRESULT read_debuggee_mem( DEBUGHELPER* pHelper, DWORD addr, DWORD size, tVOID* buff, tSTRING pref, char* out, size_t dwMaxSize ) {
  
  tDWORD  len;
  DWORD   dwErr;
  HRESULT hr = pHelper->ReadDebuggeeMemory( pHelper, addr, size, buff, &dwErr );
  
  if ( FAILED(hr) ) {
    len = wsprintf( local_buff, "%s: ??? debuggee memory(p=0x%08x,s=%u) - 0x%08x ", pref, addr, size, hr );
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, hr, 0, local_buff+len, sizeof(local_buff)-len, 0 );
    lstrcpyn( out, local_buff, dwMaxSize );
  }
  
  else if ( FAILED(dwErr) ) {
    len = wsprintf( local_buff, "%s: ??? debuggee memory ???(p=0x%08x,s=%u) - 0x%08x ", pref, addr, size, dwErr );
    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, dwErr, 0, local_buff+len, sizeof(local_buff)-len, 0 );
    lstrcpyn( out, local_buff, dwMaxSize );
  }
  
  return hr;
}



