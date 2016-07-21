/*!
 * (C) 2004 "Kaspersky Lab"
 *
 * \file conn/networklist.h
 * \author Дамир
 * \date 2006
 * \brief Интерфейс, отвечающий за работу приложения с сетевыми списками
 *
 */

#ifndef KLCONN_NETWORKLIST_H
#define KLCONN_NETWORKLIST_H

#include "./conn_types.h"

namespace KLCONN_NLST
{
	/*!
	\brief Константы описания сетевых списков
	*/

	//!\brief Имя списка карнтина
	const wchar_t QUARANTINE_LIST_NAME[]	= L"Quarantine";

	//!\brief Имя списка backup'а
	const wchar_t BACKUP_LIST_NAME[]		= L"Backup";

	//!\brief Имя списка TIF'а
	const wchar_t TIF_LIST_NAME[]			= L"TIF";

	/*!
	\brief Поля парамеров элемента списка, возвращаемые функцией GetItemInfo.
		Для списков Quarantine и Backup формат одинаковый.
	*/

	const wchar_t c_szwItemId[]		= L"ListItemId";	// идентификатор элемента списка ( KLPAR::Value::STRING_T )
	const wchar_t c_szwFileTitle[]	= L"szFileTitle";	// Имя файла (без пути) ( KLPAR::Value::STRING_T )
	const wchar_t c_szwiStatus[]	= L"iObjStatus";	// Статус (WARNING,SUSP. и т.д.) ( KLPAR::Value::INT_T enum _OBJ_STATUS )

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

	const wchar_t c_szwStoreTime[]	= L"Q&B: StoreTime";	// Время помещения в карантин ( KLPAR::Value::DATE_TIME_T )

	const wchar_t c_szwVirusName[]	= L"VirusName";			// Имя подозреваемого вируса ( KLPAR::Value::STRING_T )
	const wchar_t c_szwObjSize[]	= L"Q&B: ObjSize";		// Размер обьекта ( KLPAR::Value::INT_T )
	const wchar_t c_szwRestorePath[]= L"szRestorePath";		// Путь восстановления обьекта (м.б. пустым) ( KLPAR::Value::STRING_T )
	const wchar_t c_szwUser[]		= L"szUser";			// имя пользователя ( KLPAR::Value::STRING_T )
	const wchar_t c_szwDescription[]= L"szDescription";			// Информационное поле (напр. имя файла-контейнера) ( KLPAR::Value::STRING_T )
	const wchar_t c_szwLastCheckBaseDate[] = L"Q&B: sz_LastCheckBaseDate";	// Дата вирусной базы, по которой обьект проверялся последний раз ( KLPAR::Value::DATE_TIME_T )
	const wchar_t c_szwSentDate[]	= L"SentToKLDate";		// Дата отсылки обьекта в KL ( KLPAR::Value::DATE_TIME_T )

	/*!
	\brief file info, see NetworkListItemsFile 
	*/

    //! file name (without path) as it will be shown to the security administrator 
    const wchar_t c_szwNliFile_Name[]	= L"KLCONN_NLST_FNAME"; //STRING_T

    //! file size, 64-bit int
    const wchar_t c_szwNliFile_Size[]	= L"KLCONN_NLST_FIZE"; //LONG_T

	/*!
	\brief Типы действий над элементами списков Quarantine и Backup	
	*/

	const wchar_t c_szwActionScan[]			= L"NLAction: Scan";

	const wchar_t c_szwActionDelete[]		= L"NLAction: Delete";
	
	const wchar_t c_szwActionRestore[]		= L"NLAction: Restore";

	const wchar_t c_szwActionScanExclude[]	= L"NLAction: ScanExclude";
	
	
}

namespace KLCONN
{	

	const wchar_t c_szwListName[]	= L"ListName";		// имя сетевого списка
	const wchar_t c_szwItemId[]		= L"ListItemId";	// идентификатор элемента списка
	const wchar_t c_szwItemsIds[]	= L"ListItemsIds";	// список идентификаторов элементов списка


	/*!
	\brief Список событий, публикуюмый приложением в случае изменения сетевого списка
		Данные события публикуются стандартным механизмом используя функцию Notifier::Event

		Формат тело события для всех событий общий:

			c_szwListName - имя сетевого списка ( KLPAR::Value::STRING_T )
			с_szwItemsId - идентификатор измененного элемента списка ( KLPAR::Value::STRING_T )
		или 
			c_szwListName - имя сетевого списка ( KLPAR::Value::STRING_T )
			c_szwItemsIds - список идентификаторов элементов списка ( KLPAR::Value::ARRAY_T ) 
	*/

	//!\brief Добавление элемента в сетевой список
	const wchar_t c_ev_ListItemAdded[]		= L"NLST: ItemAdded";

	//!\brief Удаление элемента из сетевого списока
	const wchar_t c_ev_ListItemDeleted[]	= L"NLST: ItemDeleted";

	//!\brief Изменение элемента сетевого списока
	const wchar_t c_ev_ListItemModified[]	= L"NLST: ItemModified";
	


	/*!
     \brief Интерфейс, отвечающий за работу приложения с сетевыми списками

    */

	class KLSTD_NOVTABLE NetworkList : public KLSTD::KLBaseQI
    {
    public:

		/*!
        \brief Возвращает свойства указаннного элемента списка
			В случае ошибки выбрасывается исключение типа KLERR::Error*

        \param szwListName		[in]	имя сетевого списка.
		\param szwListItemId	[in]	идентификатор элемента списка ( данный идентификатор должен быть 
				уникален в рамках приложения ( хоста ) )

		\param ppItemParams		[out]	параметры элемента. Если указанный элемент не найден в списке 
				данный параметр приравнивается NULL.
				Формат парамеров варируется в зависимости от 
				использованного списка. ( см. константы в начале файла ).

		
		*/
        virtual void GetItemInfo( 
				const wchar_t*  szwListName,
				const wchar_t*  szwListItemId,
			    KLPAR::Params **ppItemParams ) = 0;

		/*!
        \brief Возвращает список идентификаторов элементов списка
			В случае ошибки выбрасывается исключение типа KLERR::Error*

        \param szwListName		[in]	имя сетевого списка.
		\param nItemsStartPos	[in]	начальная позиция списка
		\param nItemsCount		[in]	необходимое количество идентификаторов ( если <=0 возвращается весь набор с указанной позиции )
		\param vecItemsIds		[out]	вектор идентификаторов списка
		*/

		virtual void GetItemsList( 
				const wchar_t*		szwListName,
				int					nItemsStartPos, 
				int					nItemsCount,
				KLSTD::AKWSTRARR&	vecItemsIds ) = 0;

		/*!
        \brief Выполняет указанное действие над элементом ( элементами ) списка.
			После выполнение действия над элементом списка, для данного элемента, даже если он не изменился,
			должно быть опубликаванно событие 
			В случае ошибки выбрасывается исключение типа KLERR::Error*

        \param szwListName		[in]	имя сетевого списка.
		\param szwActionName	[in]	текстовое имя дейтсвия ( набор действий индивидуален для конкретного списка ) 
				( см. константы в начале файла ).		
		\param vecItemsIds		[in]	вектор идентификаторов списка, над которыми надо произвести указанное действие 
				( если список пустой, то над всеми эелементами списка )
		*/

		virtual void DoItemsAction( 
				const wchar_t*			szwListName,
				const wchar_t*			szwActionName,
				const KLSTD::AKWSTRARR&	vecItemsIds ) = 0;

	};

	/*!
     \brief Интерфейс, отвечающий за работу с файлом данных, ассоциированного с элементом списка

    */
	class KLSTD_NOVTABLE NetworkListItemsFile : public KLSTD::KLBaseQI
	{
    public:
		/*!
		\brief Результат операции чтения файла
		*/
		enum FileOperationResult
		{
			NLF_Ok,					//!< Порция файла успешно прочитанна	
			NLF_OkDownloaded,		//!< Порция файла успешно прочитанна и это была последняя порция данного файла			
			NLF_FileNotFound,		//!< Файл не найден			
			NLF_ErrorInOperation		//!< Ошибка при закачки очередной порции файла			
		};

		/*!
		\brief Функция позволяет получить информацию о файле , 
                    +-c_szwNliFile_Name, 
                    +-c_szwNliFile_Size
		*/
        virtual void GetFileInfo( KLPAR::Params** ppFileInfo ) = 0;


		/*!
		\brief Функция позволяет получить порцию файла данных ассоциированного с элементом списка		
		
		\param startPos [in] Позиция файла с которой необходимо прочитать порцию			
		\param chunkBuff [in,out] Буфер для порции файла 
		\param chunkBuffSize [in] Pазмер выделенной памяти
		\param receivedChunkSize [out] Размер закаченной порции
		*/
		virtual FileOperationResult GetFileChunk( 
                                        AVP_qword startPos, 
			                            void *chunkBuff, 
                                        size_t chunkBuffSize, 
                                        size_t &receivedChunkSize ) = 0;
	};


	class KLSTD_NOVTABLE NetworkList2 : public NetworkList
    {
    public:

		/*!
		\brief Функция позволяет получить интерфес для работы с файлом данных,
				ассоциированного с элементом списка

		\param szwListName		[in]	имя сетевого списка.
		\param szwListItemId	[in]	идентификатор элемента списка
		\param ppListFile		[out]	интерфейс для работы с файлом
		*/
		virtual void GetItemFile( 
				const wchar_t*  szwListName,
				const wchar_t*  szwListItemId,
			    NetworkListItemsFile **ppListFile ) = 0;
		
	};
	
} // end namespace KLCONN

#endif // KLCONN_NETWORKLIST_H
