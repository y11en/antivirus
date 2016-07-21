// Test.h: interface for the CTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(PRTS_TEST_H)
#define PRTS_TEST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "build/general.h"
#include <kca/prts/tasksstorage.h>
#include <exception>
#include <list>
#include <vector>

#include <testmodule.h>
#include <helpers.h>

using namespace KLPAR;
using namespace KLERR;	
using namespace KLSTD;
using namespace KLPRTS;
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
# define TASK_STORAGE_FILE_PATH		TEXT("c:\\PRTS_STORAGE")
# define TASK_STORAGE_FILE_PATH_W	L"c:\\PRTS_STORAGE"
#endif

#ifdef __unix
# define TASK_STORAGE_FILE_PATH		L"./PRTS_STORAGE"
# define TASK_STORAGE_FILE_PATH_W	L"./PRTS_STORAGE"
#endif

#ifdef N_PLAT_NLM
# define TASK_STORAGE_FILE_PATH		L"sys:/csdata/PRTS_STORAGE"
# define TASK_STORAGE_FILE_PATH_W	L"sys:/csdata/PRTS_STORAGE"
#endif

#define TR_PRTS_SERVER_ADDRESSL			L"http://127.0.0.1:16001"
#define KLPRTS_TASK_STORAGE_TEST	std::wstring(L"KLPRTS_TASK_STORAGE_TEST")

extern std::wstring g_sPRTSTrServerCompName;	// транспортное имя компоненты
extern KLPRCI::ComponentId g_cidPRTSServerComponent;

//////////////////////////////////////////////////////////////////////
// CPRTSTest

class CPRTSTest
{
public:
	CPRTSTest();

	bool Run();

private:
	time_t m_time;
	std::wstring m_sComponentName;

	bool IsItMyOwnTask(std::wstring TaskID, std::vector<std::wstring>& arTaskUniqueIDs);

// Steps
	void AddTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs);
	void VerifyAddTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs);
	void UpdateTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs);
	void VerifyUpdateTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs);
	void DeleteTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs);
	void VerifyDeleteTasks(TasksStorage* pTasks, std::vector<std::wstring>& arTaskUniqueIDs);
};

class CPRTSGeneralTest2 : public KLSTD::KLBaseImpl<KLTST2::Test2>
{
public:
	IMPLEMENT_TEST2_INSTANTIATE(CPRTSGeneralTest2)

	void Initialize(KLTST2::TestData2* pTestData);
	KLSTD_NOTHROW void Deinitialize() throw();
	virtual long Run();

protected:
	CPRTSTest	m_Test;
};

void PRTSTestInit();
void PRTSTestDeInit();


#endif // !defined(PRTS_TEST_H)
