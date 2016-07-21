// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Monday,  25 December 2006,  10:59 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- token.c
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end



#if defined (_WIN32)

// AVP Prague stamp begin( Interface version,  )
#define Token_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_win32loader.h>
// AVP Prague stamp end



#include <windows.h>
#include <userenv.h>
#include <lmcons.h>

extern BOOL ImpersonateToUser(HANDLE* pToken);

// AVP Prague stamp begin( Data structure,  )
// Interface Token. Inner data structure

typedef struct tag_Token 
{
  	HANDLE handle; // --
	tDWORD process; // --
 	HANDLE profile;
	tBOOL  try_impersonate_to_user; // --
} Token;

//!
// AVP Prague stamp begin( Object declaration,  )
//!
//! typedef struct tag_hi_Token {
//! 	const iTokenVtbl*  vtbl; // pointer to the "Token" virtual table
//! 	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
//! 	Token*             data; // pointer to the "Token" data structure
//! } *hi_Token;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Token 
{
	const iTokenVtbl*  vtbl; // pointer to the "Token" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	Token*             data; // pointer to the "Token" data structure
} *hi_Token;

// AVP Prague stamp end



BOOL (WINAPI *g_OpenThreadToken)(HANDLE ThreadHandle, DWORD DesiredAccess, BOOL OpenAsSelf, PHANDLE TokenHandle);
BOOL (WINAPI *g_SetThreadToken)(PHANDLE Thread, HANDLE Token);
BOOL (WINAPI *g_LogonUserA)(LPSTR lpszUsername, LPSTR lpszDomain, LPSTR lpszPassword, DWORD dwLogonType, DWORD dwLogonProvider, PHANDLE phToken);
BOOL (WINAPI *g_LogonUserW)(LPWSTR lpszUsername, LPWSTR lpszDomain, LPWSTR lpszPassword, DWORD dwLogonType, DWORD dwLogonProvider, PHANDLE phToken);

BOOL (WINAPI *g_LoadUserProfileW)(HANDLE hToken, LPPROFILEINFOW lpProfileInfo);
BOOL (WINAPI *g_UnloadUserProfile)(HANDLE hToken, HANDLE hProfile);

// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "Token". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Token_ObjectInit( hi_Token _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Token::ObjectInit method" );

	// Place your code here
	_this->data->process = -1;

	PR_TRACE_A1( _this, "Leave Token::ObjectInit method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call Token_ObjectInitDone( hi_Token _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Token::ObjectInitDone method" );

	// Place your code here
	if( _this->data->process == -1 && g_OpenThreadToken )
		g_OpenThreadToken(GetCurrentThread(), TOKEN_QUERY | TOKEN_IMPERSONATE | TOKEN_DUPLICATE, TRUE, &_this->data->handle);

	if( !_this->data->handle )
	{
		HANDLE hProcess = _this->data->process != -1
            ? OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, _this->data->process)
            : GetCurrentProcess();
		if( hProcess )
		{
			if(!OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_IMPERSONATE | TOKEN_DUPLICATE, &_this->data->handle))
            {
                const int lastError = GetLastError();
    		    PR_TRACE((_this, prtERROR, "token\tFailed open token for process with identifier %d, last error %d",
                    _this->data->process, lastError));
            }
			CloseHandle(hProcess);
		}
        else
        {
            const int lastError = GetLastError();
    		PR_TRACE((_this, prtERROR, "token\tFailed open process with identifier %d, last error %d",
                _this->data->process, lastError));
        }

	}
	
	PR_TRACE_A1( _this, "Leave Token::ObjectInitDone method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call Token_ObjectPreClose( hi_Token _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Token::ObjectPreClose method" );

	// Place your code here
	if( _this->data->handle )
	{
		if( _this->data->profile )
		{
			g_UnloadUserProfile(_this->data->handle, _this->data->profile);
			_this->data->profile = NULL;
		}

		CloseHandle(_this->data->handle);
		_this->data->handle = NULL;
	}

	PR_TRACE_A1( _this, "Leave Token::ObjectPreClose method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetName )
// Interface "Token". Method "Get" for property(s):
//  -- OBJECT_NAME
tERROR pr_call Token_PROP_GetName( hi_Token _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter *GET* method GetName for property pgOBJECT_NAME" );

	if( !_this->data->handle )
	{
		// 9x is running

		if( !buffer )
			size = 0;
		
		if( !GetUserName(buffer, &size) )
		{
			if( GetLastError() == ERROR_INSUFFICIENT_BUFFER )
				error = buffer ? errBUFFER_TOO_SMALL : errOK;
			else
				error = errUNEXPECTED;
		}
		if( out_size )
			*out_size = size;

		if( PR_SUCC(error) && prop == pgOBJECT_FULL_NAME && buffer && size )
		{
			tDWORD i;
			tCHAR c;
			
			for(i = 0; i < size - 1; i++)
			{
				c = buffer[i];
				if( !((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '-') )
					buffer[i] = '_';
			}
		}
	}
	else
	{
		HANDLE hToken = _this->data->handle;
		PTOKEN_USER  pInfo = NULL;
		DWORD nSize = 0;

		GetTokenInformation(hToken, TokenUser, NULL, 0, &nSize);
		error = nSize ? CALL_SYS_ObjHeapAlloc(_this, &pInfo, nSize) : errUNEXPECTED;

		if( PR_SUCC(error) && !GetTokenInformation(hToken, TokenUser, pInfo, nSize, &nSize) )
			error = errUNEXPECTED;

		if( PR_SUCC(error) )
		{
			if( prop == pgOBJECT_NAME )
			{
				DWORD nDomainSize = 0;
				SID_NAME_USE nSidUse;

				nSize = 0;
				LookupAccountSid(NULL, pInfo->User.Sid, NULL, &nSize, NULL, &nDomainSize, &nSidUse);
				if( !nSize )
					error = errUNEXPECTED;
				else
				{
					if( out_size )
						*out_size = nSize + nDomainSize;
					
					if( size < nSize + nDomainSize )
						error = buffer ? errBUFFER_TOO_SMALL : errOK;
					else
					{
						if( !LookupAccountSid(NULL, pInfo->User.Sid, buffer + nDomainSize,
								&nSize, buffer, &nDomainSize, &nSidUse) )
							error = errUNEXPECTED;
						else if( nDomainSize )
							buffer[nDomainSize] = '\\';
					}
				}
			}
			else
			{
				SID *  psid = (SID *)pInfo->User.Sid;
				tUINT  i;
				tCHAR  buf[0x100];
				tDWORD len;
				
				len = wsprintf(buf, "S");
				len += wsprintf(buf + len, "-%u", psid->Revision);
				len += wsprintf(buf + len, "-%u", psid->SubAuthorityCount);
				for(i = 0; i < psid->SubAuthorityCount; ++i)
				{
					if( !psid->SubAuthority[i] )
						break;
					len += wsprintf(buf + len, "-%u", psid->SubAuthority[i]);
				}

				if( out_size )
					*out_size = len + 1;
				
				if( size < len + 1 )
					error = buffer ? errBUFFER_TOO_SMALL : errOK;
				else
					memcpy(buffer, buf, min(sizeof(buf), size));
			}
		}

		if( pInfo )
			CALL_SYS_ObjHeapFree(_this, pInfo);
	}

	PR_TRACE_A2( _this, "Leave *GET* method GetName for property pgOBJECT_NAME, ret tDWORD = %u(size), %terr", out_size ? *out_size : 0, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetType )
// Interface "Token". Method "Get" for property(s):
//  -- TOKEN_TYPE
tERROR pr_call Token_PROP_GetType( hi_Token _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	HANDLE hToken = _this->data->handle;
	PTOKEN_USER pInfo = NULL;
	DWORD nSize = 0;
	tDWORD nType = TOKEN_TYPE_UNKNOWN;

	PR_TRACE_A0( _this, "Enter *GET* method GetType for property plTOKEN_TYPE" );

	if( !buffer )
		size = 0;
	if( size < sizeof(tDWORD) )
		error = errBUFFER_TOO_SMALL;

	if( PR_SUCC(error) )
	{
		GetTokenInformation(hToken, TokenUser, NULL, 0, &nSize);
		error = nSize ? CALL_SYS_ObjHeapAlloc(_this, &pInfo, nSize) : errUNEXPECTED;
	}

	if( PR_SUCC(error) && !GetTokenInformation(hToken, TokenUser, pInfo, nSize, &nSize) )
		error = errUNEXPECTED;

	if( PR_SUCC(error) )
	{
		SID * psid;
		
		nType = TOKEN_TYPE_USER;

		psid = (SID *)pInfo->User.Sid;
		
		if( psid->SubAuthorityCount >= 1 &&
			psid->SubAuthority[0] != SECURITY_NT_NON_UNIQUE )
		{
			tDWORD i;
			for(i = 1; i < psid->SubAuthorityCount; i++)
			{
				if( psid->SubAuthority[i] )
					break;
			}
			
			if( i == psid->SubAuthorityCount )
			{
				nType = TOKEN_TYPE_UNKNOWN;
/*
				switch( psid->SubAuthority[0] )
				{
				case SECURITY_LOCAL_SYSTEM_RID:       nType = TOKEN_TYPE_UNKNOWN; break;
				case SECURITY_LOCAL_SERVICE_RID:      nType = TOKEN_TYPE_UNKNOWN; break;
				case SECURITY_NETWORK_SERVICE_RID:    nType = TOKEN_TYPE_UNKNOWN; break;
				}
*/
			}
		}

		*(tDWORD *)buffer = nType;
	}

	if( pInfo )
		CALL_SYS_ObjHeapFree(_this, pInfo);

	if( out_size )
		*out_size = sizeof(tDWORD);

	PR_TRACE_A2( _this, "Leave *GET* method GetType for property plTOKEN_TYPE, ret tDWORD = %u(size), %terr", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Impersonate )
// Parameters are:
//! tERROR pr_call Token_Impersonate( hi_Token _this )
tERROR pr_call Token_Impersonate( hi_Token _this, tDWORD p_type )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Token::Impersonate method" );

	// Place your code here

	if( p_type != eitDefault)
	{
		HANDLE hLinkedToken = NULL, hTokenNew = NULL, hToken = NULL;
		tDWORD dwOutSize = 0, dwElevationType = 0;

		if( !_this->data->handle )
			return errUNEXPECTED;

		PR_TRACE((_this, prtERROR, "token\tTrying to elevate %s", p_type == eitAsAdmin ? "ToAdmin" : "ToRestricted"));

		g_OpenThreadToken(GetCurrentThread(), TOKEN_QUERY | TOKEN_IMPERSONATE | TOKEN_DUPLICATE, TRUE, &hToken);
		if( hToken )
			RevertToSelf();

		error = errNOT_OK;
		if(!GetTokenInformation(_this->data->handle, 18, &dwElevationType, sizeof(dwElevationType), &dwOutSize))
		{
			PR_TRACE((_this, prtERROR, "token\tGetTokenInformation(ElevationType) error(%x)", GetLastError()));
		}
		else if( (dwElevationType == 3 && p_type == eitAsAdmin) || (dwElevationType == 2 && p_type == eitAsUser) )
		{
			if(!GetTokenInformation(_this->data->handle, 19, &hLinkedToken, sizeof(hLinkedToken), &dwOutSize))
				PR_TRACE((_this, prtERROR, "token\tGetTokenInformation(LinkedToken) error(%x)", GetLastError()));
			else
			{
				PR_TRACE((_this, prtIMPORTANT, "token\tGetTokenInformation(LinkedToken) succeded(%x)", hLinkedToken));
				if(DuplicateToken(hLinkedToken, SecurityImpersonation, &hTokenNew))
				{
					PR_TRACE((_this, prtIMPORTANT, "token\tToken elevated"));
					CloseHandle(_this->data->handle);
					_this->data->handle = hTokenNew;
					g_SetThreadToken(NULL, hTokenNew);
					error = errOK;
				}
				else
					PR_TRACE((_this, prtERROR, "token\tDuplicateToken(LinkedToken) failed(%x)", GetLastError()));

				CloseHandle(hLinkedToken);
			}
		}
		if( hToken )
		{
			if( PR_FAIL(error) )
				g_SetThreadToken(NULL, hToken);
			CloseHandle(hToken);
		}
	}
	else
	{
		if( g_SetThreadToken )
		{
			tDWORD winerr = 0;
			if( !g_SetThreadToken(NULL, _this->data->handle) )
			{
				winerr = GetLastError();
				if( winerr == ERROR_BAD_TOKEN_TYPE )
				{
					HANDLE hToken = NULL;
					if( DuplicateToken(_this->data->handle, SecurityImpersonation, &hToken) )
						winerr = g_SetThreadToken(NULL, hToken) ? 0 : GetLastError();

					if( hToken )
						CloseHandle(hToken);
				}
			}

			if( winerr )
				PR_TRACE((0, prtERROR, "token\tSetThreadToken error(%x)", winerr));
		}

		if( _this->data->try_impersonate_to_user )
			ImpersonateToUser(&_this->data->handle);

		{
			DWORD dwUserNameBufSize = UNLEN + 1;
			TCHAR szUserName[UNLEN + 1] = __TEXT("unknown");
			GetUserName(szUserName, &dwUserNameBufSize);
			PR_TRACE((0, prtIMPORTANT, "token\tImpersonated to user '%s'", szUserName));
		}
	}

	PR_TRACE_A1( _this, "Leave Token::Impersonate method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Revert )
// Parameters are:
tERROR pr_call Token_Revert( hi_Token _this )
{
	tERROR error = errOK;
	PR_TRACE_A0( _this, "Enter Token::Revert method" );

	// Place your code here
	RevertToSelf();

	PR_TRACE_A1( _this, "Leave Token::Revert method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Logon )
// Parameters are:
tERROR pr_call Token_Logon( hi_Token _this, hSTRING p_account, hSTRING p_password )
{
	tERROR error = errNOT_IMPLEMENTED;
	PR_TRACE_A0( _this, "Enter Token::LogonUser method" );

	// Place your code here
	if( !g_LogonUserA && !g_LogonUserW )
		error = errNOT_SUPPORTED;
	else if( !p_account || !p_password )
		error = errPARAMETER_INVALID;
	else
	{
		tCODEPAGE cp = g_LogonUserW ? cCP_UNICODE : cCP_ANSI;
		tSTR_RANGE result = cSTRING_EMPTY, range;
		tPTR account = NULL, password = NULL, domain = NULL;
		tDWORD size;

		RevertToSelf();
		Token_ObjectPreClose(_this);

		CALL_String_FindOneOfBuff(p_account, &result, cSTRING_WHOLE, "\\", 0, cCP_ANSI, fSTRING_FIND_BACKWARD);

		if( result != cSTRING_EMPTY )
		{
			range = STR_RANGE(0, STR_RANGE_POS(result));

			error = CALL_String_ExportToBuff(p_account, &size, range, NULL, 0, cp, cSTRING_Z);
			if( PR_SUCC(error) )
				error = CALL_SYS_ObjHeapAlloc(_this, &domain, size);
			if( PR_SUCC(error) )
				error = CALL_String_ExportToBuff(p_account, &size, range, domain, size, cp, cSTRING_Z);

			range = STR_RANGE(STR_RANGE_POS(result)+1, cSTRING_WHOLE_LENGTH);
		}
		else
			range = cSTRING_WHOLE;

		error = CALL_String_ExportToBuff(p_account, &size, range, NULL, 0, cp, cSTRING_Z);
		if( PR_SUCC(error) )
			error = CALL_SYS_ObjHeapAlloc(_this, &account, size);
		if( PR_SUCC(error) )
			error = CALL_String_ExportToBuff(p_account, &size, range, account, size, cp, cSTRING_Z);

		error = CALL_String_ExportToBuff(p_password, &size, cSTRING_WHOLE, NULL, 0, cp, cSTRING_Z);
		if( PR_SUCC(error) )
			error = CALL_SYS_ObjHeapAlloc(_this, &password, size);
		if( PR_SUCC(error) )
			error = CALL_String_ExportToBuff(p_password, &size, cSTRING_WHOLE, password, size, cp, cSTRING_Z);

		if( PR_SUCC(error) )
		{
			BOOL bRet;
			if( g_LogonUserW )
				bRet = g_LogonUserW(account, domain ? domain : L".", password, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &_this->data->handle);
			else
				bRet = g_LogonUserA(account, domain ? domain : ".", password, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &_this->data->handle);
			if( !bRet )
			{
				if( g_LogonUserW )
					PR_TRACE((_this, prtERROR, "ldr\t!!!! Cannot logon user:%S domain:%S error(%d)",
						account, domain, GetLastError()));
				else
					PR_TRACE((_this, prtERROR, "ldr\t!!!! Cannot logon user:%s domain:%s error(%d)",
						account, domain, GetLastError()));
				error = errUNEXPECTED;
			}
			else
			{
				if( g_LoadUserProfileW && g_UnloadUserProfile )
				{
					PROFILEINFOW profile;
					memset(&profile, 0, sizeof(PROFILEINFOW));
					profile.dwSize = sizeof(PROFILEINFOW);
					profile.dwFlags = 0;
					profile.lpUserName = (LPWSTR)account;

					if( g_LoadUserProfileW(_this->data->handle, &profile) )
						_this->data->profile = profile.hProfile;
					else
						PR_TRACE((_this, prtERROR, "ldr\t!!!! LoadUserProfileW error(%d)", GetLastError()));
				}
				if( !ImpersonateLoggedOnUser(_this->data->handle) )
				{
					PR_TRACE((_this, prtERROR, "ldr\t!!!! ImpersonateLoggedOnUser error(%d)", GetLastError()));
					error = errUNEXPECTED;
				}
			}
		}

		if( account )
			CALL_SYS_ObjHeapFree(_this, account);
		if( password )
			CALL_SYS_ObjHeapFree(_this, password);
	}

	PR_TRACE_A1( _this, "Leave Token::LogonUser method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(Token_PropTable)
	mLOCAL_PROPERTY_FN( pgOBJECT_NAME, Token_PROP_GetName, NULL )
	mLOCAL_PROPERTY_BUF( plPROCESS_ID, Token, process, cPROP_BUFFER_WRITE | cPROP_WRITABLE_ON_INIT )
	mLOCAL_PROPERTY_FN( pgOBJECT_FULL_NAME, Token_PROP_GetName, NULL )
	mLOCAL_PROPERTY_FN( plTOKEN_TYPE, Token_PROP_GetType, NULL )
	mLOCAL_PROPERTY_BUF( plTRY_IMPERSONATE_TO_USER, Token, try_impersonate_to_user, cPROP_BUFFER_READ_WRITE )
mPROPERTY_TABLE_END(Token_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_Token_vtbl = 
{
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        Token_ObjectInit,
	(tIntFnObjectInitDone)    Token_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    Token_ObjectPreClose,
	(tIntFnObjectClose)       NULL,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        NULL,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iTokenVtbl e_Token_vtbl = 
{
	(fnpToken_Impersonate)    Token_Impersonate,
	(fnpToken_Revert)         Token_Revert,
	(fnpToken_Logon)          Token_Logon
};
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )
// Interface "Token". Register function
tERROR pr_call Token_Register( hROOT root ) 
{
	tERROR error;

	PR_TRACE_A0( root, "Enter Token::Register method" );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_TOKEN,                              // interface identifier
		PID_WIN32LOADER,                        // plugin identifier
		0x00000000,                             // subtype identifier
		Token_VERSION,                          // interface version
		VID_PETROVITCH,                         // interface developer
		&i_Token_vtbl,                          // internal(kernel called) function table
		(sizeof(i_Token_vtbl)/sizeof(tPTR)),    // internal function table size
		&e_Token_vtbl,                          // external function table
		(sizeof(e_Token_vtbl)/sizeof(tPTR)),    // external function table size
		Token_PropTable,                        // property table
		mPROPERTY_TABLE_SIZE(Token_PropTable),  // property table size
		sizeof(Token),                          // memory size
		IFACE_SYSTEM                            // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ldr\tToken(IID_TOKEN) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_

	if( PR_SUCC(error) )
	{
		HMODULE hModAdvAPI, hModEnvAPI;
		hModAdvAPI = GetModuleHandle("AdvAPI32.dll");
		if( hModAdvAPI )
		{
			*(void**)&g_SetThreadToken = GetProcAddress(hModAdvAPI, "SetThreadToken");
			*(void**)&g_OpenThreadToken = GetProcAddress(hModAdvAPI, "OpenThreadToken");
			*(void**)&g_LogonUserA = GetProcAddress(hModAdvAPI, "LogonUserA");
			*(void**)&g_LogonUserW = GetProcAddress(hModAdvAPI, "LogonUserW");
		}
		hModEnvAPI = LoadLibrary("userenv.dll");
		if( hModEnvAPI )
		{
			*(void**)&g_LoadUserProfileW = GetProcAddress(hModEnvAPI, "LoadUserProfileW");
			*(void**)&g_UnloadUserProfile = GetProcAddress(hModEnvAPI, "UnloadUserProfile");
		}
	}

	PR_TRACE_A1( root, "Leave Token::Register method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Caution !!!,  )
// You have to implement propetry: pgINTERFACE_VERSION
// You have to implement propetry: pgINTERFACE_COMMENT
// AVP Prague stamp end



#endif //_WIN32
