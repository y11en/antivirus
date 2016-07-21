/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	AntispamOUSupportClass.h
*		\brief	Класс поддержки антиспам-плагина в Outlook
*		
*		\author Vitaly DVi Denisov
*		\date	07.09.2004 14:43:56
*		
*		
*/		


#ifndef _ANTISPAMOUSUPPORTCLASS_H_
#define _ANTISPAMOUSUPPORTCLASS_H_

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Mail/structs/s_mcou_antispam.h>
#include <stuff/thread.h>
#include <Prague/iface/i_objptr.h>
#include <Mail/iface/i_as_trainsupport.h>
#include <Prague/iface/i_csect.h>
#include <string>
#include <Prague/pr_wrappers.h>

#include "../mapiedk/mapiedk.h"
extern CMAPIEDK* g_pMAPIEDK;

#include "backgroundscan/backgroundscan.h"

//! Идентификатор оригинальной папки для перемещенного письма
#ifndef PR_MESSAGE_PARENT_FOLDER
	#define	PR_MESSAGE_PARENT_FOLDER	PROP_TAG(PT_BINARY, 0x646A)
#endif

//! Сохраняем в этом свойстве информацию о том, что мы проверили это сообщение
//! Это позволит нам быстро находить непроверенные сообщения
#ifndef PR_MESSAGE_AS_PROCESSED
	#define	PR_MESSAGE_AS_PROCESSED		PROP_TAG(PT_LONG, 0x646B)
#endif


#ifdef ULRELEASE
#undef ULRELEASE
#define ULRELEASE(x) \
	if (x)	\
	{                    \
		g_pMAPIEDK->pUlRelease((x));  \
		(x) = NULL;      \
	}
#endif

#ifdef MAPIFREEBUFFER
#undef MAPIFREEBUFFER
#define MAPIFREEBUFFER(x) \
	if (x)	\
	{                         \
		g_pMAPIEDK->pMAPIFreeBuffer((x));  \
		(x) = NULL;           \
	}
#endif

#ifdef FREEPADRLIST
#undef FREEPADRLIST
#define FREEPADRLIST(x) \
	if (x)	\
	{                       \
		g_pMAPIEDK->pFreePadrlist((x));  \
		(x) = NULL;         \
	}
#endif

#ifdef FREEPROWS
#undef FREEPROWS
#define FREEPROWS(x)    \
	if (x)	\
	{                       \
		g_pMAPIEDK->pFreeProws((x));     \
		(x) = NULL;         \
	}
#endif

namespace AntispamOUSupport
{
	class CAntispamOUSupportClass; //!< forward declaration
	extern CAntispamOUSupportClass* g_pAntispamOUSupportClass;

	//! Настройки для первичного обучения
	typedef TrainFolderInfoList_t AntispamPluginTrainSettings_t;
	

	//! Действия с выделенным письмом (в методе ActionOnMessage)
	enum Action_t
	{ 
		TrainAsSpam,	//! Это - спам
		TrainAsHam,		//! Это - не спам
		RemoveFromSpam, //! Удалить это письмо из базы спама
		RemoveFromHam,	//! Удалить это письмо из базы не_спама
		Process			//! Процессировать письмо
	};

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: TrainStatusCallback_t
	//! \brief			:	Callback, который будет вызываться по окончание 
	//!						тренинга каждого письма.
	//!						Если он вернет E_FAIL, это будет сигналом на остановку
	//!						тренинга.
	//! \return			: HRESULT 
	//! \param          : void* _context - контекст вызывателя
	//! \param          : tULONG ulMessagesCount - Сколько всего писем в очереди на тренировку
	//! \param          : tULONG ulMessagesProcessed - Сколько писем процессировано
	//! \param          : cStringObj szCurrentFolder - Текущая папка
	//! \param          : cStringObj szCurrentMessage - Текущее сообщение
	typedef HRESULT (*TrainStatusCallback_t) (
		void* _context,
		tULONG ulMessagesCount, 
		tULONG ulMessagesProcessed, 
		cStringObj szCurrentFolder, 
		cStringObj szCurrentMessage
		);

	///////////////////////////////////////////////////////////////////////////
	//! \fn				: AntispamTaskStatus
	//! \brief			:	Эта функция будет вызываться при изменении статуса.
	//!						Ее реализацию стоит написать отдельно.
	//! \return			: void 
	//! \param          : tTaskState taskState
	void AntispamTaskStatus( tTaskState taskState );
	
	class CAntispamOUSupportClass
	{
	public:
		CAntispamOUSupportClass();
		virtual ~CAntispamOUSupportClass();

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: IsTaskExist
		//! \brief			:	check existance of the <sProfileName> task
		//! \return			: tBOOL 
		static tBOOL IsTaskExist(LPCSTR sProfileName);
		
		///////////////////////////////////////////////////////////////////////////
		//! \fn				: ShowProductASSettings
		//! \brief			:	Открывает страницу настроек антиспама в продукте
		//! \return			: tERROR 
		static tERROR ShowProductASSettings();

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: IsAVPTaskManagerEnabled
		//! \brief			:	Показывает, доступен ли AVP
		//! \return			: static bool 
		static bool IsAVPTaskManagerEnabled();

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: IsAntispamEngineEnabled
		//! \brief			:	Показывает, доступно ли ядро антиспама
		//! \return			: static bool 
		static bool IsAntispamEngineEnabled();

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: IsAntispamEngineEnabled
		//! \brief			:	Показывает, доступно ли ядро антиспама
		//! \return			: static check_algorithm_t 
		check_algorithm_t GetCheckAlgorithm();

		inline static CAntispamOUSupportClass* GetInstance(IN HINSTANCE hModule)
		{
			static PRLocker cs;
			PRAutoLocker _cs_(cs);
			CAntispamOUSupportClass*& This = g_pAntispamOUSupportClass;
			if ( !This )
			{
				This = new CAntispamOUSupportClass();
				if ( This && FAILED(This->Init(hModule)) )
				{
					delete This;
					This = NULL;
				}
			}
			return This;
		}
		inline static void ReleaseInstance()
		{
			if ( g_pAntispamOUSupportClass )
				delete g_pAntispamOUSupportClass;
			g_pAntispamOUSupportClass = NULL;
		}
		
		///////////////////////////////////////////////////////////////////////////
		//! \fn				: Init
		//! \brief			:	Инициализация
		//! \param          : IN HINSTANCE hModule
		//! \return			: HRESULT 
		HRESULT Init(IN HINSTANCE hModule);

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: GetSettings
		//! \brief			:	Возвращает текущие настройки
		//! \return			: HRESULT 
		//! \param          : OUT AntispamPluginSettings_t* pAntispamPluginSettings
		HRESULT GetSettings( OUT AntispamPluginSettings_t* pAntispamPluginSettings );

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: SetSettings
		//! \brief			:	Установка настроек
		//! \return			: HRESULT 
		//! \param          : IN AntispamPluginSettings_t* pAntispamPluginSettings
		HRESULT SetSettings( IN AntispamPluginSettings_t* pAntispamPluginSettings );

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: SaveSettings
		//! \brief			:	Сохраняет настройки в файл
		//! \return			: HRESULT 
		HRESULT SaveSettings();

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: LoadSettings
		//! \brief			:	Читает настройки из файла
		//! \return			: HRESULT 
		HRESULT LoadSettings();

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: CheckPluginSettings
		//! \brief			:	Проверяет корректность настроек. Может показать диалог настроек
		//! \return			: void
		bool CheckPluginSettings();

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: ShowPluginSettings
		//! \brief			:	Показывает диалог настроек
		//! \return			: int
		int ShowPluginSettings(bool bAskOnCancel = false, bool bAdvansedSettings = true);

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: GetFolderInfoList
		//! \brief			:	Запрос списка подпапок. Если RecoursiveFolderInfo* == 0 - возвращается список топовых папок
		//! \return			: HRESULT 
		//! \param          : IN FolderInfo_t* pFolderInfo
		//! \param          : OUT FolderInfoList_t* pFolderInfoList
		HRESULT GetFolderInfoList( IN FolderInfo_t* pFolderInfo, OUT FolderInfoList_t* pFolderInfoList );

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: CreateSubFolder
		//! \brief			:	Создание новой папки 
		//! \return			: HRESULT 
		//! \param          : IN FolderInfo_t* pFolderInfo
		//! \param          : IN OUT FolderInfo_t* pNewFolderInfo
		HRESULT CreateSubFolder( FolderInfo_t* pFolderInfo, FolderInfo_t* pNewFolderInfo );

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: TrainEx
		//! \brief			:	Первичное обучение антиспама по заданным настройкам (асинхронное)
		//!						Возвращает хэндл потока
		//! \return			: HANDLE 
		//! \param          : IN AntispamPluginTrainSettings_t* pAntispamPluginTrainSettings
		//! \param          : IN TrainStatusCallback_t TrainStatusCallback
		//! \param          : IN void* _context - контекст вызывателя
		HANDLE TrainEx( 
			IN AntispamPluginTrainSettings_t* pAntispamPluginTrainSettings, 
			IN TrainStatusCallback_t TrainStatusCallback,
			IN void* _context
			);

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: Train
		//! \brief			:	Первичное обучение антиспама по заданным настройкам
		//! \return			: HRESULT 
		//! \param          : IN AntispamPluginTrainSettings_t* pAntispamPluginTrainSettings
		//! \param          : IN TrainStatusCallback_t TrainStatusCallback
		//! \param          : IN void* _context - контекст вызывателя
		HRESULT Train( 
			IN AntispamPluginTrainSettings_t* pAntispamPluginTrainSettings, 
			IN TrainStatusCallback_t TrainStatusCallback,
			IN void* _context
			);

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: CompareEntryID
		//! \brief			:	Сравнивает два EntryID
		//! \return			: bool 
		//! \param          : IN const EntryID_t* pEntry1
		//! \param          : IN const EntryID_t* pEntry2
		bool CompareEntryID( IN const EntryID_t* pEntry1, IN const EntryID_t* pEntry2);


		///////////////////////////////////////////////////////////////////////////
		//! \fn				: ActionOnMessage
		//! \brief			:	Производит действие над письмом, согласно текущих настроек
		//!						После перемещения pMessage становится невалидным - его надо только зарелизить
		//! \return			: HRESULT 
		//! \param          : IN LPMESSAGE pMessage
		//! \param          : IN Action_t Action
		HRESULT ActionOnMessage( 
			IN LPMESSAGE pMessage,
			IN Action_t Action
			);
		
		///////////////////////////////////////////////////////////////////////////
		//! \fn				: ProcessOnMessage
		//! \brief			:	Производит тренировку или классификацию на письме
		//! \return			: HRESULT 
		//! \param          : IN LPMESSAGE pMessage
		//! \param          : IN Action_t Action
		HRESULT ProcessOnMessage( 
			IN LPMESSAGE pMessage,
			IN Action_t Action,
			OUT tDWORD* result
			);

		tERROR MassTrainingStart();
		tERROR MassTrainingFinish();

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: ScanNewMessages
		//! \brief			:	Сканирует новые сообщения, которые найдет в приемных папках
		//! \return			: bool 
		bool ScanNewMessages();

		// Эту структуру я передам в поток обучения
		struct TrainAsinch_t
		{
			CAntispamOUSupportClass* _this;
			AntispamPluginTrainSettings_t* pAntispamPluginTrainSettings;
			TrainStatusCallback_t TrainStatusCallback;
			void* _context;
		};

	private:
		AntispamPluginSettings_t* m_pAntispamPluginSettings;
		tBOOL m_bPluginHasSettings;
		cCriticalSection* m_lock; //!< Критическая секция для корректного разведения m_pAntispamPluginSettings
		LPMAPISESSION	m_pMAPISession;
		std::string m_szProfileName; //!< Имя текущего профайла я буду использовать для хранения настроек
		struct MDB_t : public EntryID_t
		{
			LPMDB lpMDB;
			MDB_t() : EntryID_t(), lpMDB(0)
			{;};
			MDB_t(LPMAPISESSION session, EntryID_t& entry) : EntryID_t(entry), lpMDB(0)
			{
				HRESULT hr = S_OK;
				ULONG ulObjType = 0;
				if ( cb && session )
					hr = session->OpenEntry(
						cb,
						reinterpret_cast<LPENTRYID>(get_lpb()),
						&IID_IMsgStore,
						MAPI_BEST_ACCESS,
						&ulObjType,
						reinterpret_cast<LPUNKNOWN *>(&lpMDB)
						);
				if ( FAILED(hr) )
					lpMDB = NULL;
			}
			virtual ~MDB_t();
		};
		MDB_t*	m_pMDB;
		cVector<MDB_t*> m_pMDBs;
		HRESULT Train(
			LPMDB lpMDB, 
			LPMAPIFOLDER lpFolder, 
			bool bIsSpam, 
			TrainStatusCallback_t TrainStatusCallback,
			void* _context,
		    tUINT ulFolderNumber,
		    tUINT ulFolderCount
			);
		bool HasSubfolders(FolderInfo_t* pFolderInfo);

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: MoveMessage
		//! \brief			:	Перемещает спамовое письмо в папку для спама
		//!						После перемещения pMessage становится невалидным - его надо только зарелизить
		//! \return			: HRESULT 
		//! \param          : LPMESSAGE pMessage
		HRESULT MoveMessage(LPMESSAGE pMessage, AntispamPluginRule_t* pRule);

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: RemoveMessage
		//! \brief			:	Перемещает письмо из папки для спама в исходную папку
		//!						либо в приемную папку по умолчанию.
		//!						После перемещения pMessage становится невалидным - его надо только зарелизить
		//! \return			: HRESULT 
		//! \param          : LPMESSAGE pMessage
		HRESULT RemoveMessage(LPMESSAGE pMessage);

		CBackgroundScan* m_hBackgroundScan; //!< Поток фонового сканирования приемных папок
		hAS_TRAINSUPPORT m_pTrainSupport;

		///////////////////////////////////////////////////////////////////////////
		//! \fn				: InitNotifyWnd
		//! \brief			:	исполнения потоконебезопасных функций
		//! \return			: void 
		//! \param          : HINSTANCE hModule
		void InitNotifyWnd(HINSTANCE hModule);
		HWND m_hNotifyWnd;                  //!< Невидимое окно для исполнения потоконебезопасных функций


		//////////////////////////////////////////////////////////////////////////
		//
		//	Схема работы с "серым списком"
		//
			///////////////////////////////////////////////////////////////////////////
			//! \fn				: IsMessageReRequest
			//! \brief			:	Возвращает TRUE, если сообщение является ответом на Request
			//! \return			: bool 
			//! \param          : LPMESSAGE pMessage
			bool IsMessageReRequest( LPMESSAGE pMessage, bool bRemoveFound );

			HRESULT SendMessageRequestEx( LPMESSAGE pMessage, bool bIsThanks = false );
			
			///////////////////////////////////////////////////////////////////////////
			//! \fn				: SendMessageRequest
			//! \brief			:	Посылает Request на pMessage
			//! \return			: HRESULT 
			//! \param          : LPMESSAGE pMessage
			HRESULT SendMessageRequest( LPMESSAGE pMessage );

			///////////////////////////////////////////////////////////////////////////
			//! \fn				: SendMessageThanks
			//! \brief			:	Посылает Thanks на pMessage
			//! \return			: HRESULT 
			//! \param          : LPMESSAGE pMessage
			HRESULT SendMessageThanks( LPMESSAGE pMessage );

			///////////////////////////////////////////////////////////////////////////
			//! \fn				: RemoveMessages
			//! \brief			:	Возвращает все сообщения от отправителя pMessage 
			//						в оригинальные папки
			//! \return			: HRESULT 
			//! \param          : LPMESSAGE pMessage
			HRESULT RemoveMessages( LPMESSAGE pMessage );
		//
		//	Схема работы с "серым списком"
		//
		//////////////////////////////////////////////////////////////////////////
		
		HRESULT ReopenMDBs();
		HRESULT OpenEntryForFolder(IN FolderInfo_t* pFolderInfo, OUT LPMAPIFOLDER* ppFolder);
	};

}	//AntispamOUSupport

#endif//_ANTISPAMOUSUPPORTCLASS_H_