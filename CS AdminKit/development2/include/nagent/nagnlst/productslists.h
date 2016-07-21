/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file nagent/nlst/productslists.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Константы для работы продуктовых сетевых списков
 */

#if !defined(KLNLST_PRODUCTSLISTS)
#define KLNLST_PRODUCTSLISTS

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif // WIN32

#include <nagent/conn/networklist.h>


namespace KLNLST_PRODUCT
{
	/*
		!\brief Имя списка карнтина
	*/
	const wchar_t KLNLST_PRODUCT_QUARANTINE_LIST_NAME[]			= L"Quarantine";

	/*
		!\brief Имя списка backup'а
	*/
	const wchar_t KLNLST_PRODUCT_BACKUP_LIST_NAME[]				= L"Backup";

	/*
		!\brief Имя списка backup'а
	*/
	const wchar_t KLNLST_PRODUCT_TIF_LIST_NAME[]				= L"TIF";

} // end namespace KLNLST_PRODUCT

namespace KLNAGNLST {

const wchar_t sz_ItemsIdsList[] = L"ItemsIdsList";			

} // end namespace KLNAGNLST

namespace KLNLST_QB
{

	
	/*
		!\brief Идентификатор подписки на события об изменении списка Q&B
	*/
	const wchar_t KLNLST_Q_CHANGE_SUBS_ID[]			=	L"Q_CHANGE_SUBS";
	const wchar_t KLNLST_B_CHANGE_SUBS_ID[]			=	L"B_CHANGE_SUBS";

	// Имя компонента Q&B
	const wchar_t KLNLST_QBACKUP_COMPONENT_NAME[]		=	L"Quarantine & Backup";


	// Скопированно из KAVQBackup.h
	///////////////////////////////////////////////////////////////////////////////

	// Имена полей в параметрах элемента списка 'Quarantine'
	const wchar_t sz_ObjName[]	= L"ObjName";			// Имя обьекта в карантине
	const wchar_t sz_RestorePath[] = L"szRestorePath";	// Путь восстановления обьекта (м.б. пустым)
	const wchar_t sz_Description[] = L"szDescription";	// Информационное поле (напр. имя файла-контейнера)
	const wchar_t sz_FileTitle[] = L"szFileTitle";		// Имя файла (без пути)
	const wchar_t sz_Domain[] = L"szDomain";			// домен
	const wchar_t sz_Computer[] = L"szComputer";		// имя компьютера
	const wchar_t sz_User[] = L"szUser";				// имя пользователя
	const wchar_t sz_VirusName[] = L"VirusName";		// Имя подозреваемого вируса 
	const wchar_t sz_iStatus[]  = L"iObjStatus";		// Статус (WARNING,SUSP. и т.д.)
	const wchar_t sz_StoreTime[] = L"Q&B: StoreTime";	// Время помещения в карантин
	const wchar_t sz_ObjSize[] = L"Q&B: ObjSize";		// Размер обьекта в карантине
	const wchar_t sz_OriginalSize[] = L"Q&B: OriginalSize"; // Исодный размер объекта
	const wchar_t sz_SentDate[] = L"SentToKLDate";		// Дата отсылки обьекта в KL
	const wchar_t sz_LastCheckBaseDate[] = L"Q&B: sz_LastCheckBaseDate";	// Дата вирусной базы, по которой обьект проверялся последний раз
	const wchar_t sz_FilesArray[] = L"FilesArray";		// массив с именами файлов (или обьектов карантина)
	const wchar_t sz_iAskUser[] = L"Q&B: Ask User";		// Спрашивать ли пользователя?
	const wchar_t sz_iReplace[] = L"Q&B: Replace";

	const wchar_t sz_StorageType[]= L"StorageType";		// Карантин или бэкап?
	const int	  c_StorageTypeQuarantine = 1;
	const int	  c_StorageTypeBackup = 2;

	typedef enum _OBJ_STATUS {
		OBJ_STATUS_INFECTED = 1,
		OBJ_STATUS_WARNING = 2,
		OBJ_STATUS_SUSPICIOUS = 3,
		OBJ_STATUS_BY_USER = 4,
		OBJ_STATUS_FALSEALARM = 5,
		OBJ_STATUS_CURED = 6,
		OBJ_STATUS_CLEAN = 7,
		OBJ_STATUS_PASSWORD_PROTECTED = 8
	} OBJ_STATUS;

	// Пользовательские задачи над элементом списка 
	///////////////////////////////////////////////////////////////////////////////
	// 1. Восстановить обьект из хранилища
	//	- sz_ObjName - (!)имя обьекта карантина
	//	- sz_StorageType - тип хранилища (карантин или бэкап)
	//	- sz_RestorePath - путь для восстановления
	//	- sz_iAskUser - спрашивать ли путь у пользователя 
	//	- sz_iReplace - если такой файл уже существует, заменить.
	const wchar_t szQBT_Restore[] = L"RestoreObj";
	///////////////////////////////////////////////////////////////////////////////
	// 2. Удалить обьект из хранилища
	//	- sz_StorageType - тип хранилища (карантин или бэкап)
	//	- sz_FilesArray  - массив имен обьектов для удаления
	const wchar_t szQBT_Delete[] = L"DeleteObj";
	///////////////////////////////////////////////////////////////////////////////
	// 3. Проверить обьект из хранилища
	//	- sz_FilesArray  - массив имен обьектов для проверки. При отсутствии этого параметра проверяется весь карантин.
	const wchar_t szQBT_ScanQuarantine[] = L"ScanQuarantine";
	///////////////////////////////////////////////////////////////////////////////
	// 4. Добавление в доверительную зону
	//	- sz_FilesArray  - массив имен обьектов для добавления.
	const wchar_t sz_AddInTrusted[] = L"AddInTrust";
	
	
	
	
	///////////////////////////////////////////////////////////////////////////////
	// Данные событий:
	// 
	// При сканировании изменился статус обьекта
	// Параметры события
	//	Эти поля заполняет ODS
	//		KLODSCN::c_szwTask_ObjectType
	//		KLODSCN::c_szwTask_ObjectName
	//		KLODSCN::c_szwTask_InfectionType
	//		KLODSCN::c_szwTask_DetectionType
	//		KLODSCN::c_szwTask_Disinfectability
	//		KLODSCN::c_szwTask_VirusName
	//		KLODSCN::c_szwTask_DefaultResult
	//		KLODSCN::c_szwTask_ResultArray
	//		KLODSCN::c_szwDelay_ProcessInfo
	//	Эти поля заполняет Q&B
	//		sz_ObjName			- имя обьекта карантина
	//		sz_EmbeddedObj		- имя вложенного обьекта (если есть)
	//		KLQB::sz_iStatus	- Статус обекта после проверки
	const wchar_t c_szwEvent_ObjStatusChanged[] = L"Q&B: Object Detected";
	///////////////////////////////////////////////////////////////////////////////
	// Обьект помещен в карантин
	//	KLQB::sz_FullObjName	- полный путь к имени файла карантина
	//	KLQB::sz_ObjName		- Имя файла карантина
	//	KLQB::sz_FileName		- Полное имя исходного файла
	const wchar_t c_szwEvent_ObjQuarantined[] = L"Q&B: Object Quarantined";
	///////////////////////////////////////////////////////////////////////////////
	// Завершена операция удаления обьекта (обьектов)
	// sz_FilesArray - массив с именами обьектов
	const wchar_t c_szwEvent_ObjQuarDeleted[] = L"Q&B: QuarObject Deleted";
	///////////////////////////////////////////////////////////////////////////////
	// Завершена операция восстановления обьекта (обьектов)
	const wchar_t c_szwEvent_ObjQuarRestored[] = L"Q&B: QuarObject Restored";


	

	const wchar_t sz_ErrorCode[] = L"iErrorCode";	// Код ошибки (возвращаемый параметр)
	const wchar_t sz_ObjectIds[] = L"ObjectIds";		// Вектор идентификаторов объеков	

	// Получены сведения об обьекте
	const wchar_t c_szwEvent_ObjectInfo[] = L"Q&B: Object Info Obtained";

	

	// 11. ( Task ) Получить сведения об обьекте	
	const wchar_t szQBT_GetObject[] = L"GetObject";

	// 7. ( Task ) Получить список обьектов в хранилище
	const wchar_t szQBT_GetObjectsList[] = L"GetObjectsList";

	const wchar_t sz_StartPos[] = L"StartPos";			// Для получения списка файлов
	const wchar_t sz_Count[] = L"Count";

	const wchar_t c_szwTaskODSLogical[] = L"KLODS_TA_SCAN_TASK";
	const wchar_t BL_SCANQUARANTINE_ASYNC_ID[]	= L"ods-scanquar-async-id";

	inline bool CheckTaskName( const std::wstring &listName, 			
			const std::wstring				&taskName,
			KLPAR::Params					*pTaskParams,
			bool							&forAllItemsFromHost )
	{
		forAllItemsFromHost = false;
		if ( taskName==szQBT_ScanQuarantine ) forAllItemsFromHost = true;
		return true;
	}


#ifdef _WIN32
	////////////////////////////////////////////////////////
	// скопировано из файла ..\ak_plugin\shared\StorageObj.h

	#define OBJ_HDR_SIGN_SIZE			4

	// Заголовок карантинного файла
	typedef struct _typeQFileHeaderStr
	{
		char cSignature[OBJ_HDR_SIGN_SIZE];		// Сигнатура KLQF
		int iVersion;			// Версия карантинного файла
		int iObjType;			// Тип обьекта карантина
		int iObjStatus;			// Статус обьекта
		time_t tSentToKL;		// Дата отсылки обьекта в KL
		DWORD dwFlags;
		time_t tBaseDate;		// Дата вирусной базы, по которой обьект проверялся последний раз

	// Сохраняемые атрибуты исходного файла
		FILETIME tCreateTime;	// Время создания файла
		FILETIME tLastAccess;	// Время последнего доступа к файлу
		FILETIME tLastWrite;	// 
		DWORD dwAttributes;		// Атрибуты

		DWORD dwReserved[4];	// На будущее

		DWORD dwStringSize;			// Размер переменной части заголовка (в байтах)

		_typeQFileHeaderStr()
		{
			memset(this,0,sizeof(_typeQFileHeaderStr));
		}

	} typeQFileHeaderStr;

	// Данные об обьекте в упакованном виде
	typedef struct _typeObjInfoSerializedHeader
	{
		typeQFileHeaderStr Hdr;
		FILETIME ftPlaceTime;
		int iNewStatus;
		wchar_t szObjName[MAX_PATH];

		_typeObjInfoSerializedHeader() {memset(this,0,sizeof(typeObjInfoSerializedHeader));}
	
	} typeObjInfoSerializedHeader;	

	// скопировано из файла ..\ak_plugin\shared\StorageObj.h
	////////////////////////////////////////////////////////
#endif

} // end namespace KLNLST_QB

#endif // !defined(KLNLST_PRODUCTSLISTS)
