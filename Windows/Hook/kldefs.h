#ifndef __KLDEFS_H
#define __KLDEFS_H

#define KLMAX_PATH		260
#define KLMAX_PROCNAME	64

//+ ----------------------------------------------------------------------------------------
//params
#define FILTER_PARAM_COMMONINIT(_pParam, _HookID, _FMj, _FMi, _ProType, _ParamCount)  \
	_pParam->HookID = _HookID; \
	_pParam->FunctionMj = _FMj; \
	_pParam->FunctionMi = _FMi; \
	_pParam->ProcessingType = _ProType; \
	_pParam->ParamsCount = _ParamCount; \
	_pParam->ProcName[0] = 0; \
	_pParam->ProcessID = 0; \
	_pParam->ThreadID = 0; \
	_pParam->UnsafeValue = 0;

#define SINGLE_PARAM_SHIFT(_pSingleParam) { BYTE* pPointer; \
	pPointer = (BYTE*)_pSingleParam + sizeof(SINGLE_PARAM) + _pSingleParam->ParamSize; \
	_pSingleParam = (PSINGLE_PARAM)pPointer; }

#define SINGLE_PARAM_INIT_NONE(_pSingleParam, _ParamID) { \
	_pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE; \
	_pSingleParam->ParamID = _ParamID; \
	_pSingleParam->ParamSize = 0; }

#define SINGLE_PARAM_INIT_ULONG(_pSingleParam, _ParamID, _Value) { \
	_pSingleParam->ParamSize = sizeof(ULONG); \
	_pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE; \
	_pSingleParam->ParamID = _ParamID; \
	*(ULONG*)_pSingleParam->ParamValue = _Value;}

#define SINGLE_PARAM_INIT_LARGEINTEGER(_pSingleParam, _ParamID, _Value) { \
	_pSingleParam->ParamSize = sizeof(LARGE_INTEGER); \
	_pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE; \
	_pSingleParam->ParamID = _ParamID; \
	*(LARGE_INTEGER*)_pSingleParam->ParamValue = _Value;}


#define SINGLE_PARAM_INIT_SID(_pSingleParam) { \
	_pSingleParam->ParamSize = __SID_LENGTH; \
	_pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE; \
	_pSingleParam->ParamID = _PARAM_OBJECT_SID; \
	_pSingleParam->ParamValue[0] = 0;}
	
				
//+ ----------------------------------------------------------------------------------------
#endif //__KLDEFS_H