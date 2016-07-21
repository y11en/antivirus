#ifndef __PARAMS
#define __PARAMS

#define _PARAM_SIZEMASK_			0xFF
#define _PARAM_TYPE_(ID, Size)	(ULONG)(ID << 8) | (Size)


#define _PARAM_BINARY_		_PARAM_TYPE_(0, 0)
#define _PARAM_STR_			_PARAM_TYPE_(1, 0)
#define _PARAM_WORD_			_PARAM_TYPE_(2, 2)
#define _PARAM_DWORD_		_PARAM_TYPE_(3, 4)
#define _PARAM_IP4_ADDRESS	_PARAM_TYPE_(4, 4)
#define _PARAM_QDWORD_		_PARAM_TYPE_(5, 8)
#define _PARAM_BYTE_			_PARAM_TYPE_(6, 1)
#define _PARAM_STR_W			_PARAM_TYPE_(7, 0)

//#define 

#ifndef __ONLY_PARAM_DESCR

#ifndef _WINDOWS_
	#include "stdafx.h"
#endif

#include "..\..\hook\avpgcom.h"
#include "..\..\hook\IDriver.h"
#include "..\..\hook\funcs.h"
#include "..\..\hook\HookSpec.h"
#include "..\..\hook\NETID.h"
#include "..\..\hook\klpf_api.h"
#include "..\..\plugins\include\fw_ev_api.h"

#include <list>
using namespace std;
#include "..\..\hook\PFFilterID.h"

void GetFilterDesciptions(PFILTER_TRANSMIT pFilter, CString *pstrDescr);
void GetParameterDescription(PFILTER_PARAM pSingleParam, CString *pstrDescr);

//BOOL ChangeFilterParamValue(PFILTER_TRANSMIT pFilter, PFILTER_PARAM	pNewParam);
BOOL AddFilterParamValue(PFILTER_TRANSMIT pFilter, PFILTER_PARAM	pNewParam);
BOOL DelFilterParamValue(PFILTER_TRANSMIT pFilter, DWORD dwIdx);

// -----------------------------------------------------------------------------------------

CHAR* FillFunctionsList(CStringList *pStringList, CHAR  **FuncNameArr,DWORD FuncNameArrSize, DWORD CurrentPos, CHAR* pstrFun, DWORD* Idx);
CHAR* FillFuncList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun, DWORD* Idx);
CHAR* FillMajList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun, DWORD* Idx);
CHAR* FillMinList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun, DWORD* Idx);

#endif	//__ONLY_PARAM_DESCR

void GetSingleParameterInfo(PSINGLE_PARAM pParam, CHAR* pName, CHAR* pSingleDescr, DWORD* dwType);
void GetSingleParameterInfo(PFILTER_PARAM pParam, CHAR* pName, CHAR* pSingleDescr, DWORD* dwType);
void GetSingleParameterDescription(PSINGLE_PARAM pSingleParam, CHAR *pstrDescr);


#endif //__PARAMS