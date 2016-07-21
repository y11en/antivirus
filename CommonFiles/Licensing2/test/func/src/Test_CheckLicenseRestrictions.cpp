#include "stdafx.h"
#include "Test_CheckLicenseRestrictions.h"
#include "../../api/include/TestContext.h"
#include "ProgramClientInfo_Helpers.h"

#include <string>
#include <sstream>
#include <algorithm>

namespace KasperskyLicensing  {
namespace Test  {

//////////////////////////////////////////////////////////////////////////
std::string MakeCommaSeparatedString(const std::vector<unsigned int>& vec)
{
	std::stringstream os;
	bool bFirst = true;
	for (std::vector<unsigned int>::const_iterator it = vec.begin();
			it != vec.end();
			++it)
	{
		if (bFirst)
			bFirst = false;
		else
			os << ',';
		os << *it;
	}
	return os.str();
}

//////////////////////////////////////////////////////////////////////////
bool CompareNotifications(
	const std::vector<unsigned int>& vec1,
	const std::vector<unsigned int>& vec2
	)
{
	if (vec1.size() != vec2.size())
		return false;
	std::vector<unsigned int> vec1_sorted(vec1), vec2_sorted(vec2);
	std::sort(vec1_sorted.begin(), vec1_sorted.end());
	std::sort(vec2_sorted.begin(), vec2_sorted.end());
	return std::equal(vec1_sorted.begin(), vec1_sorted.end(), vec2_sorted.begin());
}

////////////////////////////////////////////////////////////////////////////
bool Test_CheckLicenseRestrictions_SimpleLO_true(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1210 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1234, 49, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict 1234: expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "Expected(1) and given from library id-notificators are different";
			return false;
		}

	}

	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}



bool Test_CheckLicenseRestrictions_SimpleLO_false(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
 	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1210 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1234, 51, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (bPositive)
		{			
			sErrorDesc = "Invalid verdict 1234: expected FALSE";
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(2);


		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "Expected(2) and given from library id-notificators are different";
			return false;
		}

	}

	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}


bool Test_CheckLicenseRestrictions_IdNotif_ManyRestr(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
				{1300 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1310 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение (1)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1311, 55, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction(1311): " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict 1311: expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1311);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected(1311) and given from library id-notificators are different";
			return false;
		}

	}

	// проверяем лицензионное ограничение (2)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1312, 65, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction(1312): " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict 1312: expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1312);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected(1312) and given from library id-notificators are different";
			return false;
		}

	}

	// проверяем лицензионное ограничение (3)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1313, 75, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction(1313): " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict 1313: expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1313);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected(1313) and given from library id-notificators are different";
			return false;
		}

	}	

	// проверяем лицензионное ограничение (4) - не выполняется ни одно ЛУ.
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1313, 85, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction(1313_FALSE): " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (bPositive)
		{			
			sErrorDesc = "Invalid verdict 1313: expected FALSE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(13130);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected(13130) and given from library id-notificators are different";
			return false;
		}

	}	
	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}

bool Test_CheckLicenseRestrictions_IdNotif_1Key(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1100 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение (1)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1100, 55, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict 1100(1): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(11);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected(11) and given from library id-notificators are different";
			return false;
		}

	}

	// проверяем лицензионное ограничение (2)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1100, 65, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict 1100(2): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(12);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected(12) and given from library id-notificators are different";
			return false;
		}

	}

	// проверяем лицензионное ограничение (3)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1100, 75, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict 1102(3): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(13);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected(13) and given from library id-notificators are different";
			return false;
		}

	}	

	// проверяем лицензионное ограничение (4) - не выполняется ни одно ЛУ.
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1100, 85, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (bPositive)
		{			
			sErrorDesc = "Invalid verdict 1100(4): expected FALSE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(10);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected(10) and given from library id-notificators are different";
			return false;
		}

	}	
	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}

bool Test_CheckLicenseRestrictions_IdNotif_2KeyDifferentID(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1100 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение (1)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1100, 40, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict 1100(11)_(21): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(11);
		vecNotifEtalon.push_back(21);
				
		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (2)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1100, 49, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict(12)_(23): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(12);
		vecNotifEtalon.push_back(23);
		
		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (3)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1100, 70, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict(13): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(13);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}	

	// проверяем лицензионное ограничение (4) - не выполняется ни одно ЛУ.
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1100, 100, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (bPositive)
		{			
			sErrorDesc = "Invalid verdict(10)_(20): expected FALSE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(10);
		vecNotifEtalon.push_back(20);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}	
	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}
bool Test_CheckLicenseRestrictions_IdNotif_2KeySameID(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1220 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение (1)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1220, 40, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict(1)_(2): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (2)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1220, 49, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict(2)_(3): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(2);
		vecNotifEtalon.push_back(3);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}
	}

	// проверяем лицензионное ограничение (3)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1220, 70, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{			
			sErrorDesc = "Invalid verdict(3)_(0): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(3);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}	

	// проверяем лицензионное ограничение (4) - не выполняется ни одно ЛУ.
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1220, 100, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (bPositive)
		{			
			sErrorDesc = "Invalid verdict(0)_(0): expected FALSE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(10);
		
		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}	
	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}

bool Test_CheckLicenseRestrictions_CheckAdditivity(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1230 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение (true 1, 2)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1230, 75, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(11): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(11);
		vecNotifEtalon.push_back(21);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}


	// проверяем лицензионное ограничение (true 2)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1230, 125, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(11)_(21): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(11);
		vecNotifEtalon.push_back(21);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (true 0)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1230, 175, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (bPositive)
		{
			sErrorDesc = "Invalid verdict(11)_(21): expected FALSE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(10);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}

bool Test_CheckLicenseRestrictions_CheckVariables_NoBlackList(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(NoBlackList)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1600 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение (true)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1610, 1, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1611)_(1612): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1612);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}

bool Test_CheckLicenseRestrictions_CheckVariables_CorruptedBases(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(DailyAvcIsCorrupted)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1600 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение (true)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1620, 1, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1621)_(1622): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1622);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}
	}

	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}

bool Test_CheckLicenseRestrictions_CheckVariables_SerialInBlackList(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{6600 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение (true)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(6600, 1, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1601): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(6601);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}

bool Test_CheckLicenseRestrictions_CheckVariables_Other(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1600 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение (1)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1610, 1, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1611): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1611);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (2)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1620, 1, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1621): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1621);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (3)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1630, 1, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1631): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1631);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (4)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1640, 62208000, bPositive, vecNotif)) //увеличили на 1 год
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1641): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1641);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (5)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1640, 1, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1642): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1642);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (6)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1650, 1, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (bPositive)
		{
			sErrorDesc = "Invalid verdict(1650): expected FALSE";
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1650);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (7)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1650, 62208000, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1651): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1651);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}


	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}

bool Test_CheckLicenseRestrictions_CheckCondition(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1500 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение (true 1, 2)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1500, 75, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1500_151): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(151);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}


	// проверяем лицензионное ограничение (true 2)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1500, 125, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1500_152): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(152);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (true 3)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1500, 100, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1500_153): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(153);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}

bool Test_CheckLicenseRestrictions_CheckFormula(std::string& sErrorDesc)
{
	TestContext test_context;

	if (!test_context.InitializeBasesStorage(_T(".\\Bases(Normal)")))
	{
		sErrorDesc = "Initialize bases: " + test_context.GetLastErrorDesc();
		return false;
	}

	if (!test_context.InitializeKeyStorage(_T(".\\KeysStorage")))
	{
		sErrorDesc = "Initialize key storage: " + test_context.GetLastErrorDesc();
		return false;
	}

	ControlMode ctrlMode = CTRL_BLIST_AND_BASES;  // CTRL_BLIST_ONLY

	ProgramClientInfo clientInfo;
	Fill_ProgramClientInfo_WksRelease(clientInfo);

	if (!test_context.InitializeLicensing(g_szDskmPath, ctrlMode, clientInfo))
	{
		sErrorDesc = "Initialize licensing interface: " + test_context.GetLastErrorDesc();
		return false;
	}

	// создаем контекст
	ILicenseContextPtr pContext = test_context.MakeEmptyLicenseContext();
	if (!pContext.GetPointer())
	{
		sErrorDesc = "Make empty license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	{
		LicenseObjectId lic_objects[] = 
			{
//				{1 /*id*/, LicenseObjectId::OBJ_TYPE_PRODUCT /*type*/,		6 /*major version*/, 0 /*minor version*/ },
//				{1200 /*id*/, LicenseObjectId::OBJ_TYPE_APPLICATION /*type*/,	6 /*major version*/, 0 /*minor version*/ },
				{1700 /*id*/, LicenseObjectId::OBJ_TYPE_COMPONENT /*type*/,	6 /*major version*/, 0 /*minor version*/ }
			};
		for (int i = 0; i < sizeof(lic_objects) / sizeof(lic_objects[0]); ++i)
		{
			pContext->AddItem(lic_objects[i]);
		}
	}

	// устанавливаем текущий контекст
	if (!test_context.SetLicenseContext(pContext))
	{
		sErrorDesc = "Set current license context: " + test_context.GetLastErrorDesc();
		return false;
	}

	// проверяем лицензионное ограничение (true 1)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1700, 1, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1700_171): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(171);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}
	}


	// проверяем лицензионное ограничение (true 1)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1700, 10, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1700_171): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(171);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (true 2)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1700, 11, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1700_172): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(172);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}
	
	// проверяем лицензионное ограничение (true 3)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1700, 12, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1700_173): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(173);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}

	// проверяем лицензионное ограничение (ошибочные)
	{
		bool bPositive;
		std::vector<unsigned int> vecNotif;
		if (!test_context.CheckLicenseRestriction(1720, 11, bPositive, vecNotif))
		{
			sErrorDesc = "Check license resriction: " + test_context.GetLastErrorDesc();
			return false;
		}
		
		// проверяем соответствие возвращенного значения вердикта
		if (!bPositive)
		{
			sErrorDesc = "Invalid verdict(1720_1): expected TRUE";
			return false;
		}
		else
		{
		}

		// проверяем соответствие возвращенных нотификационых кодов
		std::vector<unsigned int> vecNotifEtalon;
		vecNotifEtalon.push_back(1721);

		if (CompareNotifications(vecNotif, vecNotifEtalon))
		{
			// нотификации соответствуют эталонным
		}
		else
		{
			// нотификации НЕ соответствуют эталонным
			sErrorDesc = "expected (";
			sErrorDesc += MakeCommaSeparatedString(vecNotifEtalon);
			sErrorDesc += ") and given from library id-notificators are different (";
			sErrorDesc += MakeCommaSeparatedString(vecNotif);
			sErrorDesc += ")";
			return false;
		}

	}
	// повторяем предыдущую операцию для другого лицензионного ограничения, если нужно

	return true;
}

}  // namespace Test
}  // namespace KasperskyLicensing

