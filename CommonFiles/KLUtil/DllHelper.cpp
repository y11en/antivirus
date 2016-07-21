//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#include "stdafx.h"
#include "DllHelper.h"

using namespace KLUTIL;

//******************************************************************************
// class CCKernel32Helper
//******************************************************************************
CCKernel32Helper::fnSetProcessWorkingSetSize CCKernel32Helper::m_SetProcessWorkingSetSize;

CCDllHelper CCKernel32Helper::m_DLL;

CCKernel32Helper g_Kernel32Helper;

//******************************************************************************
// class CCShell32Helper
//******************************************************************************
CCShell32Helper::fnSHGetSpecialFolderPath CCShell32Helper::m_SHGetSpecialFolderPath;

CCDllHelper CCShell32Helper::m_DLL;

CCShell32Helper g_Shell32Helper;

//==============================================================================
