#ifndef _AVZKRNL_DLL_H_
#define _AVZKRNL_DLL_H_

#include "avzapi.h"
#include <windows.h>

#define RETURN_NOTIMPL(return_code)	\
{	\
	SetLastError(E_NOTIMPL);	\
	return return_code;			\
}; 

#define TYPEDEF_FUNC1( _name, _param )	\
	typedef  AVZERROR ( WINAPI *p_t_##_name ) ( _param );	\
	__inline AVZERROR   WINAPI p_def_##_name  ( _param ) RETURN_NOTIMPL(AVZ_Error);
#define TYPEDEF_FUNC2( _name, _param1, _param2 )	\
	typedef AVZERROR ( WINAPI *p_t_##_name  ) ( _param1, _param2 );	\
	__inline AVZERROR  WINAPI p_def_##_name   ( _param1, _param2 ) RETURN_NOTIMPL(AVZ_Error);
#define TYPEDEF_FUNC3( _name, _param1, _param2, _param3 )	\
	typedef AVZERROR ( WINAPI *p_t_##_name  ) ( _param1, _param2, _param3 );	\
	__inline AVZERROR  WINAPI p_def_##_name   ( _param1, _param2, _param3 ) RETURN_NOTIMPL(AVZ_Error);
#define TYPEDEF_FUNC4( _name, _param1, _param2, _param3, _param4 )	\
	typedef AVZERROR ( WINAPI *p_t_##_name  ) ( _param1, _param2, _param3, _param4 );	\
	__inline AVZERROR  WINAPI p_def_##_name   ( _param1, _param2, _param3, _param4 ) RETURN_NOTIMPL(AVZ_Error);
#define TYPEDEF_FUNC5( _name, _param1, _param2, _param3, _param4, _param5 )	\
	typedef AVZERROR ( WINAPI *p_t_##_name  ) ( _param1, _param2, _param3, _param4, _param5 );	\
	__inline AVZERROR  WINAPI p_def_##_name   ( _param1, _param2, _param3, _param4, _param5 ) RETURN_NOTIMPL(AVZ_Error);
#define TYPEDEF_FUNC6( _name, _param1, _param2, _param3, _param4, _param5, _param6 )	\
	typedef AVZERROR ( WINAPI *p_t_##_name  ) ( _param1, _param2, _param3, _param4, _param5, _param6 );	\
	__inline AVZERROR  WINAPI p_def_##_name   ( _param1, _param2, _param3, _param4, _param5, _param6 ) RETURN_NOTIMPL(AVZ_Error);
#define EXTENDEDFUNC_INIT_DEFAULT(finction_name)	\
	p_t_##finction_name	p##finction_name	=	p_def_##finction_name
#define EXTENDEDFUNC_INIT(handle, finction_name, finction_display_name)	\
	p##finction_name =  ( p_t_##finction_name ) GetProcAddress( handle,	finction_display_name )

HINSTANCE g_hAVZKRNL_DLL = NULL;
TYPEDEF_FUNC3(AVZ_InitializeExA, OUT CAVZScanKernel** ppAVZAScanKernel, IN const char* szPath,    IN const char* szProductType    );	
TYPEDEF_FUNC3(AVZ_InitializeExW, OUT CAVZScanKernel** ppAVZAScanKernel, IN const wchar_t* szPath, IN const wchar_t* szProductType );	
TYPEDEF_FUNC1(AVZ_ReloadBase,    IN CAVZScanKernel* pAVZAScanKernel );		
TYPEDEF_FUNC1(AVZ_Done,          IN CAVZScanKernel* pAVZAScanKernel );
TYPEDEF_FUNC3(AVZ_CheckFile,     IN CAVZScanKernel* pAVZAScanKernel, IN char* szFileName,  IN OUT char* szVerdict );
TYPEDEF_FUNC3(AVZ_GetSomeInfo,   IN CAVZScanKernel* pAVZAScanKernel, IN wchar_t* szScript, IN OUT IStream* pXMLStream );
TYPEDEF_FUNC6(AVZ_DeserializeXML,IN CAVZScanKernel* pAVZAScanKernel, IN IStream* pXMLStream, IN wchar_t* szTagName, IN wchar_t* szBufFormat, OUT void** ppAVZBuffer, OUT tDWORD* pdwAVZBufSize);
TYPEDEF_FUNC2(AVZ_Free,          IN CAVZScanKernel* pAVZAScanKernel, IN void* pAVZBuffer );
TYPEDEF_FUNC1(AVZ_GetDBStatus,   IN CAVZScanKernel* pAVZAScanKernel );		
TYPEDEF_FUNC3(AVZ_SetCallbackProc,     IN CAVZScanKernel* pAVZAScanKernel, IN AVZCallback_t dwCallBackID, IN void* pCallBackProc );
TYPEDEF_FUNC2(AVZ_SetCallbackCtx,      IN CAVZScanKernel* pAVZAScanKernel, IN void* pTaskContext );
TYPEDEF_FUNC3(AVZ_LoadCustomScriptDB,  IN CAVZScanKernel* pAVZAScanKernel, IN wchar_t* szDBFilename, OUT void** ppDBHandle );
TYPEDEF_FUNC2(AVZ_FreeCustomScriptDB,  IN CAVZScanKernel* pAVZAScanKernel, IN void* pDBHandle );
TYPEDEF_FUNC3(AVZ_GetCustomScriptCount,IN CAVZScanKernel* pAVZAScanKernel, IN void* pDBHandle, OUT tDWORD* pdwCount );
TYPEDEF_FUNC5(AVZ_RunCustomScript,     IN CAVZScanKernel* pAVZAScanKernel, IN void* pDBHandle, IN tDWORD dwScriptID, IN tDWORD dwScriptMode, OUT tDWORD* pdwResult );
TYPEDEF_FUNC3(AVZ_SR_Init,              IN CAVZScanKernel* pAVZAScanKernel, IN const wchar_t* szFileName, OUT tDWORD* pdwSessionID );
TYPEDEF_FUNC1(AVZ_SR_Done,              IN tDWORD dwSessionID );
TYPEDEF_FUNC4(AVZ_SR_AddEvent,          IN tDWORD dwSessionID, IN tDWORD dwEventType, IN const void* pEventData, IN tDWORD dwEventDataSize );
TYPEDEF_FUNC5(AVZ_SR_Memory,            IN tDWORD dwSessionID, IN tDWORD dwProcessID, IN tDWORD qwBaseAddr, IN const void* pMemory, IN tDWORD dwDataSize );
TYPEDEF_FUNC3(AVZ_SR_GetSecurityRating, IN tDWORD dwSessionID, IN tDWORD ARatingType, OUT tDWORD* pRes );


#define AVZKRNL_DLL_INIT()	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_InitializeExA 				 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_InitializeExW 				 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_ReloadBase    				 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_Done          				 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_CheckFile    				 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_GetSomeInfo    				 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_DeserializeXML  				 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_Free         				 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_GetDBStatus    				 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_SetCallbackProc    			 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_SetCallbackCtx    			 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_LoadCustomScriptDB    		 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_FreeCustomScriptDB    		 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_GetCustomScriptCount    		 );	\
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_RunCustomScript    			 );	
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_SR_Init                			 );	
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_SR_Done                			 );	
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_SR_AddEvent            			 );	
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_SR_Memory              			 );	
	EXTENDEDFUNC_INIT_DEFAULT( AVZ_SR_GetSecurityRating    			 );	
AVZKRNL_DLL_INIT();

AVZERROR pAVZ_RunScript( IN CAVZScanKernel* pAVZAScanKernel, IN wchar_t* szScript ) { return pAVZ_GetSomeInfo ? pAVZ_GetSomeInfo(pAVZAScanKernel, szScript, NULL) : AVZ_Error; };

__inline bool UnloadAVZKrnlDll()
{
	bool bResult = true;

	AVZKRNL_DLL_INIT();
	if ( g_hAVZKRNL_DLL ) bResult = !!FreeLibrary(g_hAVZKRNL_DLL);
	g_hAVZKRNL_DLL = NULL;

	return bResult;
}

__inline bool LoadAVZKrnlDll(const char * path_to_dll = "avzkrnl.dll")
{
	bool bResult = false;

	if ( !g_hAVZKRNL_DLL ) 
	{
		g_hAVZKRNL_DLL = LoadLibraryA(path_to_dll);
		if ( g_hAVZKRNL_DLL ) 
		{
			bResult  = (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_InitializeExA , "Z1" )) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_InitializeExW , "Z2" )) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_ReloadBase    , "Z5" )) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_Done          , "Z3" )) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_CheckFile     , "Z4" )) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_GetSomeInfo   , "Z6" )) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_Free          , "Z7" )) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_GetDBStatus   , "Z8" )) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_DeserializeXML, "Z9" )) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_SetCallbackProc     , "Z10")) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_SetCallbackCtx      , "Z11")) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_RunCustomScript     , "Z12")) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_LoadCustomScriptDB  , "Z13")) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_GetCustomScriptCount, "Z14")) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_FreeCustomScriptDB  , "Z15")) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_SR_Init             , "Z16")) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_SR_Done             , "Z17")) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_SR_AddEvent         , "Z18")) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_SR_Memory           , "Z19")) );
			bResult &= (	NULL !=	(EXTENDEDFUNC_INIT( g_hAVZKRNL_DLL, AVZ_SR_GetSecurityRating, "Z20")) );
		}
	}
	else
		bResult = true;

	if ( !bResult ) 
	{
		UnloadAVZKrnlDll();
	}

	return bResult;
}

#endif//_AVZKRNL_DLL_H_