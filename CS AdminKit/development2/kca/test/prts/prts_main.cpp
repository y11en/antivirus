/*!
*		(C) 2002 Kaspersky Lab 
*		
*		\file	main.cpp
*		\brief	
*		
*		\author Андрей Брыксин
*		\date	10.10.2002 11:54:52
*		
*		Example:	
*/		


#include "build/general.h"
#include "std/io/klio.h"
#include "transport/tr/transport.h"
#include <kca/prts/tasksstorage.h>
#include <exception>
#include <list>
#include <vector>

#include <kca/init/init_kca.h>

#include "prts_test.h"
#ifdef _WIN32
#include <windows.h>
#endif

using namespace KLPAR;
using namespace KLERR;
using namespace KLSTD;

#pragma comment(lib, "Ws2_32.lib")

#include <testmodule.h>
#include <helpers.h>

#define KLCS_MODULENAME							L"PRTS_TEST"

#ifdef WIN32
void PRTS_DeleteFolderTree(LPCTSTR pszFolder)
{
	if (pszFolder)
	{
		WIN32_FIND_DATA FindFileData;

#ifdef UNICODE
		std::wstring sFolder(pszFolder);
		HANDLE hFind = ::FindFirstFile(std::wstring(sFolder + L"\\*.*").c_str(), &FindFileData);
		if (hFind == (void *)0xFFFFFFFF) return;
		while(true)
		{
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				DeleteFile(std::wstring(sFolder + L"\\" + std::wstring(FindFileData.cFileName)).c_str());
#else
		std::string sFolder(pszFolder);
		HANDLE hFind = ::FindFirstFile(std::string(sFolder + "\\*.*").c_str(), &FindFileData);
		while(true)
		{
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				DeleteFile(std::string(sFolder + "\\" + std::string(FindFileData.cFileName)).c_str());
#endif
			if (!::FindNextFile(hFind, &FindFileData)) break;
		}

		::FindClose(hFind);
	}
}
#elif defined(__UNIX__)
	// Unix
	void PRTS_DeleteFolderTree(LPCTSTR pszFolder){};
#else // N_PLAT_NLM
	void PRTS_DeleteFolderTree(wchar_t *pszFolder){};
#endif

///////////////////////////////////////////////////////////////////////////////
// Test 2

using namespace KLTST2;

void PRTSTestInit()
{
    KLCSKCA_Initialize();

	// Delete storage file
	PRTS_DeleteFolderTree(TASK_STORAGE_FILE_PATH);
	KLSTD_CreateDirectory(TASK_STORAGE_FILE_PATH_W, false);

	// получаем транспортное имя компоненты
	KLTRAP::ConvertComponentIdToTransportName(g_sPRTSTrServerCompName, g_cidPRTSServerComponent);

	// добавляем location сервера для входящих соединений ( порт location'а может быть любым )
	int serverUsedPort;	// используется для сохранения порта
	KLTR_GetTransport()->AddListenLocation(g_sPRTSTrServerCompName.c_str(), TR_PRTS_SERVER_ADDRESSL, serverUsedPort);

	KLPRTS_CreateTasksStorageServer(KLPRTS_TASK_STORAGE_TEST, KLPRCI::ComponentId(L"KLPRTS:TEST_PRODUCT", L"", L"KLPRTS:TEST_TS", L"1"), TASK_STORAGE_FILE_PATH_W, true);
}

void PRTSTestDeInit()
{
	KLPRTS_DeleteTasksStorageServer(KLPRTS_TASK_STORAGE_TEST);
    KLCSKCA_Deinitialize();
}	
	
void CPRTSGeneralTest2::Initialize(KLTST2::TestData2* pTestData)
{
	;
};
KLSTD_NOTHROW void CPRTSGeneralTest2::Deinitialize() throw()
{
    ;
};

long CPRTSGeneralTest2::Run()
{
	return m_Test.Run()?0:(-1);
};


