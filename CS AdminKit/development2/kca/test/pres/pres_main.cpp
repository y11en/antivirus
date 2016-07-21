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
#include "transport/tr/transport.h"
#include <kca/pres/eventsstorage.h>
#include <std/conv/klconv.h>
#include <exception>
#include <list>
#include <vector>

#include <kca/init/init_kca.h>

#include "pres_test.h"
#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <direct.h>
#endif
#ifdef N_PLAT_NLM
#include <stat.h>
#endif

using namespace KLPAR;
using namespace KLERR;
using namespace KLSTD;

#pragma comment(lib, "Ws2_32.lib")

///////////////////////////////////////////////////////////////////////////
// DeleteFolderTree - Recursively removes folder and subfolders

#ifdef WIN32
void DeleteFolderTree(LPCTSTR pszFolder)
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
#elif defined(__unix)
	// Unix
	void DeleteFolderTree(wchar_t *pszFolder)

    {

        KLSTD_DeletePath(pszFolder);

    };
#endif

///////////////////////////////////////////////////////////////////////////////
// Test 2
	
#include <testmodule.h>
#include <helpers.h>
	
using namespace KLTST2;


void PRESTestInit()
{
        KLCSKCA_Initialize();

		KLSTD_USES_CONVERSION;

		// Delete old tests files
		DeleteFolderTree( KLSTD_W2T( SUBSCRIPTION_STORAGE_ROOT_W ) );

		// получаем транспортное имя компоненты
		KLTRAP::ConvertComponentIdToTransportName(g_sTrServerCompName, g_cidServerComponent);
		KLTR_GetTransport()->SetTimeouts(300000, 300000, 1000);
		// добавляем location сервера для входящих соединений ( порт location'а может быть любым )
		// используется для сохранения порта
		int serverUsedPort;
		KLTR_GetTransport()->AddListenLocation(g_sTrServerCompName.c_str(), TR_SERVER_ADDRESSL, serverUsedPort);
		
		KLSTD_CreateDirectory( SUBSCRIPTION_STORAGE_ROOT_W, false );
		KLSTD_CreateDirectory( EVENTS_STORAGE_FOLDER_W, false );

		KLPRES_CreateEventsStorageServer(EVENT_STORAGE_NAME_LOCAL, L"product", L"version", 
			SUBSCRIPTION_STORAGE_FILE_PATH_W, EVENTS_STORAGE_FOLDER_W);
}

void PRESTestDeInit()
{
	KLTR_GetTransport()->DeleteListenLocation(g_sTrServerCompName.c_str(), TR_SERVER_ADDRESSL, 0);
    KLCSKCA_Deinitialize();
}	

	
void CPRESGeneralTest2::Initialize(KLTST2::TestData2* pTestData)
{
	;
};

KLSTD_NOTHROW void CPRESGeneralTest2::Deinitialize() throw()
{
    ;
};

long CPRESGeneralTest2::Run()
{
	return m_Test.Run()?0:(-1);
};


void CPRESGeneralTest3::Initialize(KLTST2::TestData2* pTestData)
{
	;
};

KLSTD_NOTHROW void CPRESGeneralTest3::Deinitialize() throw()
{
};

long CPRESGeneralTest3::Run()
{
	return m_Test.Run()?0:(-1);
};

void CPRESGeneralTest4::Initialize(KLTST2::TestData2* pTestData)
{
	;
};

KLSTD_NOTHROW void CPRESGeneralTest4::Deinitialize() throw()
{
};

long CPRESGeneralTest4::Run()
{
	return m_Test.Run()?0:(-1);
};
	
void CPRESGeneralTest5::Initialize(KLTST2::TestData2* pTestData)
{
	;
};

KLSTD_NOTHROW void CPRESGeneralTest5::Deinitialize() throw()
{
    ;
};

long CPRESGeneralTest5::Run()
{
	return m_Test.Run()?0:(-1);
};


