// RegAutoRuns.cpp: implementation of the CRegAutoRuns class.
//
//////////////////////////////////////////////////////////////////////
#define _CRTIMP 

#include "stdafx.h"
#include "RegAutoRuns.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

#include "stuff/servfuncs.h"
#include "stuff/mm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
typedef struct _tControlledKeyStruct
{
	PWCHAR m_pwchKey;
	PWCHAR m_pwchValue;
	unsigned int m_Len;
}ControlledKeyStruct, *PControlledKeyStruct;

ControlledKeyStruct gAutoRunsKeyNames[] = 
{
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"), NULL, 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN\\*"), NULL, 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNONCE"), NULL, 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNONCEEX"), NULL, 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNSERVICES"), NULL, 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUNSERVICESONCE"), NULL, 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\SHELLSERVICEOBJECTDELAYLOAD"), NULL, 0}, // ???
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\POLICIES\\EXPLORER\\RUN"), NULL, 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINDOWS"), _T("RUN"), 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINDOWS"), _T("LOAD"), 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINLOGON"), _T("USERINIT"), 0},
	{_T("SOFTWARE\\MICROSOFT\\WINDOWS NT\\CURRENTVERSION\\WINLOGON"), _T("SHELL"), 0},
	{_T("SOFTWARE\\MICROSOFT\\ACTIVE SETUP\\INSTALLED COMPONENTS\\*"), _T("STUBPATH"), 0}, // Q238441
	{_T("SOFTWARE\\POLICIES\\MICROSOFT\\WINDOWS\\SYSTEM\\SCRIPTS\\*"), _T("SCRIPT"), 0}, // ???
	{_T("SOFTWARE\\CLASSES\\EXEFILE\\SHELL\\OPEN\\COMMAND"), _T(""), 0},
	{_T("SOFTWARE\\CLASSES\\EXEFILE\\SHELL\\OPEN\\COMMAND"), _T(""), 0},
	{_T("SYSTEM\\CONTROLSET???\\SERVICES\\*"), _T("IMAGEPATH"), 0},
};


void InitKeyArray(ControlledKeyStruct* pArray, int count)
{
	int cou;
	for (cou = 0; cou < count; cou++)
		pArray[cou].m_Len = lstrlen(pArray[cou].m_pwchKey);
}

CRegAutoRuns::CRegAutoRuns()
{
	InitKeyArray(gAutoRunsKeyNames, countof(gAutoRunsKeyNames));
}

CRegAutoRuns::~CRegAutoRuns()
{

}

bool IsKeyInArrayUpper(PWCHAR pwchKeyNameUpper, PWCHAR pwchValueNameUpper, ControlledKeyStruct* pArray, int count)
{

	// cut prefix
	if (0 == wcsncmp(pwchKeyNameUpper, _T("\\REGISTRY\\MACHINE\\"), 18))
		pwchKeyNameUpper += 18;
	else if (0 == wcsncmp(pwchKeyNameUpper, _T("\\REGISTRY\\USER\\"), 15))
	{
		pwchKeyNameUpper = wcschr(pwchKeyNameUpper + 15, '\\');
		if (NULL == pwchKeyNameUpper)
			return false;
		pwchKeyNameUpper++;
	}

	for (int cou = 0; cou < count; cou++)	
	{
		if (MatchWithPattern(pwchKeyNameUpper, pArray[cou].m_pwchKey, TRUE))
		{
			if (pArray[cou].m_pwchValue == NULL)
				return true;
			if (0 == wcscmp(pwchValueNameUpper, pArray[cou].m_pwchValue))
				return true;
		}
	}
	return false;
}

bool IsKeyInArray(PWCHAR pwchKeyName, PWCHAR pwchValueName, ControlledKeyStruct* pArray, int count)
{
	bool bRes = false;
	PWCHAR pwchKeyNameUpper = GetUpperStr( pwchKeyName );
	PWCHAR pwchValueNameUpper = GetUpperStr( pwchValueName );
	if (pwchKeyNameUpper && pwchValueNameUpper)
		bRes = IsKeyInArrayUpper(pwchKeyNameUpper, pwchValueNameUpper, pArray, count);
	FreeStr(pwchKeyNameUpper);
	FreeStr(pwchValueNameUpper);
	return bRes;
}


bool CRegAutoRuns::IsAutoRunsKey(PWCHAR pwchKeyName, PWCHAR pwchValueName)
{
	return IsKeyInArray(pwchKeyName, pwchValueName, gAutoRunsKeyNames, countof(gAutoRunsKeyNames));
}
