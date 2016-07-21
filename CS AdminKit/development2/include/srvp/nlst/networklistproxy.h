/*!
 * (C) 2005 "Kaspersky Lab"
 *
 * \file networklistproxy.h
 * \author Дамир Шияфетдинов
 * \date 2005
 * \brief Прокси интерфейса для доступа к сетевым спискам
 */


#if !defined(KLSPLP_NETWORKLISTPROXY)
#define KLSPLP_NETWORKLISTPROXY

#include <std/base/klbase.h>
#include <srvp/csp/klcsp.h>

namespace KLNLST
{
	//\brief Список параметров возвращаемый функциями работы с итератором элементов списка

	const wchar_t c_szwListItemId[]		= L"strId";					// идентификатор элемента списка на хосте
	const wchar_t c_szwItemId[]			= L"nId";					// уникальный идентификатор элемента полного списка
	const wchar_t c_szwHosDispalyName[] = L"strHostDisplayName";	// имя хоста
	const wchar_t c_szwHosName[]		= L"strHostName";			// идентификатор хоста
	const wchar_t c_szwListName[]		= L"strListName";			// имя списка
	const wchar_t c_szwCreationTime[]	= L"tmCreation";			// время создания элемента списка
	const wchar_t c_szwItemParams[]		= L"params";				// параметры продукта для элемента списка
	const wchar_t c_szwItemTask[]		= L"taskName";				// имя задачи, которая добавлена на выполнения для элемента списка	

	//!\brief Массив элементов списка
	const wchar_t ITEMS_ARRAY[]				= L"KLNLST_ITEMS_RANGE_ARRAY";
	
	class KLSTD_NOVTABLE NetworkListItemsFileProxy : public KLSTD::KLBaseQI
	{
	public:
		//!\brief Структура описания файла
		struct ItemsFileInfo
		{
			unsigned long		lTotalSize;
			std::wstring		wstrFileName;
		};

		//!\brief Результат операции чтения файла
		enum FileOperationResult
		{
			NLF_Ok,					//!< Порция файла успешно прочитанна	
			NLF_OkDownloaded,		//!< Порция файла успешно прочитанна и это была последняя порция данного файла			
			NLF_FileNotFound,		//!< Файл не найден			
			NLF_ErrorInOperation		//!< Ошибка при закачки очередной порции файла			
		};

		virtual void GetFileInfo( ItemsFileInfo& fileInfo ) = 0;

		virtual FileOperationResult GetFileChunk( unsigned long startPos, 
			void *chunkBuff, int chunkBuffSize, int &receivedChunkSize ) = 0;
	};
	
	
	class KLSTD_NOVTABLE NetworkListProxy : public KLSTD::KLBaseQI 
	{
    public:
		
		virtual void AddListItemTask (
			const std::wstring &listName,
			int					itemId,
			const std::wstring &taskName,
			KLPAR::Params*		pTaskParams
			) = 0;
		
		virtual void AddListItemTask (
			const std::wstring &listName,
			std::vector<int>	itemsIds,
			const std::wstring &taskName,
			KLPAR::Params*		pTaskParams
			) = 0;

		virtual void ResetIterator (
            const std::wstring &listName,
            KLPAR::Params* pParamsIteratorDescr,
			const std::vector<std::wstring>& vect_fields,
			const std::vector<KLCSP::field_order_t>& vect_fields_to_order,
			long  lifetime,
			std::wstring& wstrIteratorId ) = 0;

        virtual long GetRecordCount( const std::wstring& wstrIteratorId )  = 0;

        virtual void GetRecordRange(
            const std::wstring& wstrIteratorId,
            long nStart,
            long nEnd,
            KLPAR::Params** pParamsEvents ) = 0;

		virtual void ReleaseIterator( const std::wstring& wstrIteratorId ) = 0;

		virtual void GetListItemsFile( const std::wstring &listName, int itemId, 
			NetworkListItemsFileProxy **ppItemsFile ) = 0;

	};
}

#endif // !defined(KLSPLP_NETWORKLISTPROXY)