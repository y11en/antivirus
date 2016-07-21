// RegAutoRuns.cpp: implementation of the CRegAutoRuns class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RegAutoRuns.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "servfuncs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
typedef struct _tControlledKeyStruct
{
	PWCHAR m_pwchString;
	unsigned long m_Len;
}ControlledKeyStruct, *PControlledKeyStruct;

ControlledKeyStruct gAutoRunsKeyNames[] = 
{
	{_T("\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINLOGON\\USERINIT"), 0},
	{_T("\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINLOGON\\SHEL_T("), 0},
	{_T("\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNONCE"), 0},
	{_T("\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNONCEEX"), 0},
	{_T("\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"), 0},
	{_T("\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNSERVICES"), 0},
	{_T("\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNSERVICESONCE"), 0},
	{_T("\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\SHELLSERVICEOBJECTDELAYLOAD"), 0},
	{_T("\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\POLICIES\\EXPLORER\\RUN"), 0},
	{_T("\\REGISTRY\\MACHINE\\SOFTWARE\\POLICIES\\MICROSOFT\\WINDOWS\\SYSTEM\\SCRIPTS"), 0},

	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"), 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNSERVICES"), 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNSERVICESONCE"), 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINDOWS\\RUN"), 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINDOWS\\LOAD"), 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\POLICIES\\EXPLORER\\RUN"), 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNONCE"), 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNONCEEX"), 0},

	{_T("SOFTWARE\\POLICIES\\MICROSOFT\\WINDOWS\\SYSTEM\\SCRIPTS"), 0},
};

ControlledKeyStruct gClassesKeyNames[] = 
{
	{_T("\\REGISTRY\\MACHINE\\SOFTWARE\\CLASSES"), 0},
};

CRegAutoRuns::CRegAutoRuns()
{
	int cou;
	int arrlen;

	arrlen = sizeof(gAutoRunsKeyNames) / sizeof(gAutoRunsKeyNames[0]);
	for (cou = 0; cou < arrlen; cou++)
		gAutoRunsKeyNames[cou].m_Len = lstrlen(gAutoRunsKeyNames[cou].m_pwchString) * sizeof(WCHAR);

	arrlen = sizeof(gClassesKeyNames) / sizeof(gClassesKeyNames[0]);
	for (cou = 0; cou < arrlen; cou++)
		gClassesKeyNames[cou].m_Len = lstrlen(gClassesKeyNames[cou].m_pwchString) * sizeof(WCHAR);
}

CRegAutoRuns::~CRegAutoRuns()
{

}

bool CRegAutoRuns::IsAutoRunsKey(PWCHAR pwchStrKeyName, unsigned long Len)
{
	int arrlen = sizeof(gAutoRunsKeyNames) / sizeof(gAutoRunsKeyNames[0]);
	for (int cou = 0; cou < arrlen; cou++)	
	{
		if (GetSubstringPosBM((__int8*) pwchStrKeyName, Len, (__int8*) gAutoRunsKeyNames[cou].m_pwchString, gAutoRunsKeyNames[cou].m_Len) != -1)
			return true;
	}

	return false;
}

bool CRegAutoRuns::IsShellExKey(PWCHAR pwchStrKeyName, unsigned long Len)
{
	if (GetSubstringPosBM((__int8*) pwchStrKeyName, Len, (__int8*) gClassesKeyNames[0].m_pwchString, gClassesKeyNames[0].m_Len) != -1)
			return true;

	return false;
}
