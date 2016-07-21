/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	jrnl/journal.h
 * \author	Mikhail Karmazine
 * \date	22.01.2003 11:58:09
 * \brief	Интерфейс для работы с журналом - persitent-хранилищем записей,
    в который либо добавляються записи в конец, либо удаляются существующие записи.
    Также возможен просмотр всех сущестующих записей.
 */


#ifndef __JOURNAL_H__
#define __JOURNAL_H__

#include "std/base/klbase.h"
#include "std/io/klio.h"
#include "std/par/params.h"
#include "errors.h"

#define KLJRNL_DEF_MAX_REC_NUM_IN_PAGE      100000
#define KLJRNL_DEF_MAX_PAGE_SIZE            1024  // 1 kb
#define KLJRNL_DEF_LIMIT_PAGES              true
#define KLJRNL_DEF_BINARY                   true
#define KLJRNL_DEF_OPEN_TIMEOUT             0
#define KLJRNL_DEF_PAGE_LIMIT_TYPE          KLJRNL::pltRecCount
#define KLJRNL_DEF_USE_REC_NUM_INDEX        false

namespace KLJRNL
{
    /*!
    * \brief Интерфейс записи журнала. Запись должна уметь соохранять себя в поток,
        и доставать себя оттуда.
    */
    class KLSTD_NOVTABLE JournalRecord : public KLSTD::KLBase
    {
    public:
        /*!
          \brief	Serialize
            Метод вызывается при сохранении записи в журнал.

          \param	strData [out] - запись должна сохранить себя в переданную строку.
        */
        virtual void Serialize( std::string & strData ) const = 0;

        /*!
          \brief	Deserialize
            Метод вызывается при считывании записи из журнала.

          \param	strData [in] - строка, из который должна себя прочитать запись.
        */
        virtual void Deserialize( const std::string & strData ) = 0;
    };

    // \brief см. описание структуры CreationInfo
    enum PageLimitType { pltRecCount, pltSize, pltUnlimited };


    /* \brief CreationInfo - используется в тех случаях, когда журнал нужно создать или пересоздать.
        Содержит параметры, необходимые для формирования внутренней структуры журнала.

    \param bLimitPages [in] - Если false, то количество страниц
        не ограничено, они будут добавляться по мере добавления соответсвующего количества
        записей, и удаляться, когда все записи в странице, заполненной до конца
        (см. параметр pageLimitType), будут удалены.
        Если true, то будут присутсвовать не больше двух страниц. Когда более новая страница
        заполняется, старая страница удаляется, и вместо нее создается еще более новая страница.

    \param bBinaryFormat [in] - Если true, то ID, различные флаги и проч. 
        служебная информация журнала будет лежать в файле в бинарном виде. В противном случае - 
        в текстовом (данные будут в любом случае лежать в том виде, в котором положат).

    \param pageLimitType [in] - если pltRecCount, то страница будет ограничена кол-вом положенных
        в нее записей с помощью параметра nMaxRecordsInPage. Если pltSize, то страница будет
        ограничена своим физическим размером на диске с помощью параметра nMaxPageSize.
         Если pltUnlimited, то страница всегда будет одна, и будет увеличиваться без ограничений.

    \param nMaxRecordsInPage [in] - Указывает максимальное число записей
        в странице - параметр используется, когда pageLimitType==pltRecCount, иначе игнорируется.

    \param nMaxRecordsInPage [in] - Указывает максимальный размер страницы в байтах -
        - параметр используется, когда pageLimitType==pltSize, иначе игнорируется.
    */
    struct CreationInfo
    {
        CreationInfo() :
            bLimitPages(KLJRNL_DEF_LIMIT_PAGES),
            pageLimitType(KLJRNL_DEF_PAGE_LIMIT_TYPE),
            bBinaryFormat(KLJRNL_DEF_BINARY),
            nMaxRecordsInPage(KLJRNL_DEF_MAX_REC_NUM_IN_PAGE),
            nMaxPageSize(KLJRNL_DEF_MAX_PAGE_SIZE),
            bUseRecNumIndex(KLJRNL_DEF_USE_REC_NUM_INDEX)
        {};

        bool bLimitPages;
        bool bBinaryFormat;
        PageLimitType pageLimitType;
        long nMaxRecordsInPage;
        long nMaxPageSize;
        bool bUseRecNumIndex;
    };

    /*!
    * \brief Интерфейс журнала. При открытии на запись журнал открывается с эксклюзивным
        правом на запись, т.е. писать в один журнал может одновременно только один 
        экземпляр класса.
    */
    class KLSTD_NOVTABLE Journal : public KLSTD::KLBase
    {
    public:
        virtual ~Journal() { };

        /* \brief Открывает журнал для записи или чтения. При открытии для записи может
            создать его, если журнала не существовало (см. параметры ниже).
            Рекомендуется создавать каждый журнал в отдельной папке, так как каждый 
            журнал состоит из нескольких файлов.

            Если журнал поврежден, то этот метод генерирует исключение ERR_CORRUPTED. В
                таком случае для восстановления журнала требуеться вызвать метод Repair.

  		    \param wstrPath [in] - путь к файлу журнала. Hint для создания журнала: данный
                файл - не единственый в журнале (он является центральным), поэтому каждый
                журнал рекомендуется создавать в отдельной папке.

            \param flagsCreation [in] - см. описание KLSTD::CREATION_FLAGS.
                В случае открытия только на чтение (т.е. параметр flagsAccessFlags == AF_READ):
                    а) данный параметр может принимать только значение CF_OPEN_EXISTING.
                    б) Если журнал не существует, то генерирует исключение.
                В случае открытия на запись, если журнал не существовал, для него используются
                  ограничения, указанные в параметрах nMaxRecordsInPage, bLimitPages и bBinaryFormat.
                  В противном случае эти параметры игнорируются.
                Если указан флаг CF_CLEAR, а журнал уже существовал, то журнал будет очищен,
                    но ограничения все равно останутся прежними (т.е. параметры-ограничения опять-таки
                    игнорируються)

            \param flagsAccessFlags [in] - указывает, открывать журнал на чтение или на запись.

            \param creationInfo [in] - используется только при создании журнала (см. описание 
                параметров flagsCreation и flagsAccessFlags). Содержит параметры, необходимые для
                 формирования внутренней структуры журнала. См.описание структуры CreationInfo.

            \param nTimeout [in] - время в мсек, в теч. которого этот метод будет ждать, когда другой 
                клиент отпустит журнал (если другой клиент держит журнал). Время ожидания истечет, но журнал
                все еще будет занят, то будет выброшено исключение STDE_TIMEOUT. Если параметр равен 0, 
                то метод не будет ждать, а сразу выбросит исключение, если журнал будет занят. Если параметр равен
                KLSTD_INFINITE, то метод будет ждать, пока не дождется.
        */
        virtual void Open(
            const std::wstring &    wstrPath,
            KLSTD::CREATION_FLAGS   flagsCreation,
            KLSTD::ACCESS_FLAGS     flagsAccessFlags,
            const CreationInfo &    creationInfo = CreationInfo(),
            long                    nTimeout = KLJRNL_DEF_OPEN_TIMEOUT ) = 0;

        /*!
          \brief	GetUsedCreationInfo
            Возвращает параметры структуры открытого журнала
          \param creationInfo [out] - параметры структуры открытого журнала.
                См.описание структуры CreationInfo.
        */
        virtual void GetUsedCreationInfo( CreationInfo & creationInfo ) = 0;

        /*!
          \brief	ChangeLimits
            Меняет ограничения журнала - см. описание метода Open. Открывает журнал
                в эксклюзивном режиме, в т.ч. и на чтение.

          \param	wstrPath [in] - путь к файлу журнала.

          \param creationInfo [in] - Содержит параметры, необходимые для
                формирования внутренней структуры журнала. См.описание структуры CreationInfo.
        */
        virtual void ChangeLimits(const std::wstring& wstrPath, const CreationInfo& creationInfo) = 0;

        /*!
          \brief	Close
            Закрывает журнал
        */
        virtual void Close() = 0;

        /*!
          \brief	CloseAndRemoveFiles
            Закрывает предв. открытый журнал, а затем пытается
            физически удалить файлы, которые составляют журнал. Если журнал
            в это время открыт другим клиентом, то выброситься исключение, при этом 
            не удалиться ничего. То есть либо удаляется все, либо ничего.
        */
        virtual void CloseAndRemoveFiles() = 0;

        /*!
          \brief	IsOpened
            Индикатор состояния экземпляра класса - открыт/закрыт.

          \return	bool - true если журнал открыт, false otherwise.
        */
        virtual bool IsOpened() = 0;
        
        /*!
          \brief	GetJournalSize
            Возвращает размер журнала. Журнал должен быть открыт.

          \return	AVP_longlong - размер журнала.
        */
        virtual AVP_longlong GetJournalSize() = 0;

        /*!
          \brief	Repair
            Если при открытии журнала было получено исключение ERR_CORRUPTED (см.описание метода Open),
                то журналу может помочь вызов этого метода (а может и не помочь :) )
                Открывает журнал в эксклюзивном режиме, в т.ч. и на чтение.

            \param	const std::wstring & wstrPath [in] - путь к файлу журнала.
            \param	const CreationInfo & creationInfo - журнал будет восстановлен с такими параметрами
        */
        virtual void Repair(const std::wstring& wstrPath, const CreationInfo& creationInfo) = 0;

        /*!
          \brief	Add
            Добавляет запись в журнал. 

          \param	pParams [in] - параметр сериализуеться и в таком виде записывается 
                в конец журнала.
          \return	long - id новой записи.
        */
        virtual long Add(const KLPAR::Params* pParams) = 0;

        /*!
          \brief	Add
            Добавляет запись в журнал. 

          \param	pRecord [in] - для параметра вызывается его метод Serialize, который 
            должен записать себя в поток.
          \return	long - id новой записи.
        */
        virtual long Add(JournalRecord* pRecord) = 0;
        
        /*!
          \brief	Add
            Добавляет запись в журнал. 

          \param	str [in] - строка, представляющая из себя запись. Может содержать любые
            символы, в том числе нули. Строка рассматривается как область памяти,
            начинающаяся в str.c_str(), размером str.size().
          \return	long - id новой записи.
        */
        virtual long Add( const std::string & str ) = 0;
        
        /*!
          \brief	Update
            Обновляет текущую запись в журнале. Запись должна быть того же размера,
                что и старая запись - т.е. сераилизованный в память параметр должен
                иметь тот же размер.

          \param	pParams [in] - параметр сериализуеться и в таком виде записывается 
                в журнал.
        */
        virtual void Update( const KLPAR::Params* pParams ) = 0;

        /*!
          \brief	Update
            Обновляет текущую запись в журнале. Запись должна быть того же размера,
                что и старая запись - т.е. сераилизованный в строку параметр должен
                иметь тот же размер.

          \param	JournalRecord [in] - параметр сериализуеться и в таком виде
            записывается в журнал.
        */
        virtual void Update( JournalRecord* pRecord ) = 0;

        /*!
          \brief	Update
            Обновляет текущую запись в журнале. Запись должна быть того же размера,
                что и старая запись.

          \param	str [in] - строка, представляющая из себя запись. Может содержать любые
            символы, в том числе нули. Строка рассматривается как область памяти,
            начинающаяся в str.c_str(), размером str.size().
        */
        virtual void Update( const std::string & str ) = 0;

        /*!
          \brief	Flush - делает Flush журналу.
        */
        virtual void Flush() = 0;

        /*!
          \brief	ResetIterator
            Устанавливает итератор записей на начало журнала. Сразу после этого, 
                если IsEOF()==false, можно получать запись по GetCurrent().
        */
        virtual void ResetIterator() = 0;
        
        /*!
          \brief	SetIterator
            Устанавливает итератор на запись с заданным ID. Если такой записи нет, то
            итератор устанавливается на следующую запись. Если следующеая запись
            отсутствует, то указатель устанавливается на конец журнала (IsEOF()==true).
            \param	nId [in] - ID записи.
			\return bool - true если встали точно на запись с заданным ID, false otherwise.
        */
        virtual bool SetIterator( long nId ) = 0;

        /*!
          \brief	GotoNthRecord
            Устанавливает итератор на N-ю запись от начала. Первая запись имеет индекс 0.
            Если nRecIdx больше или равно количеству записей в журнале, то итератор 
            устанавливается на конец журнала (IsEOF()==true).
            \param	nRecIdx [in] - Номер записи от начала. Первая запись имеет номер 0.

        */
        virtual void MoveToRecordN( long nRecIdx ) = 0;
        
        /*!
          \brief	GetRecordCount
            Возвращает количество записей в журнале.
            \param	long - количество записей в журнале.
        */
        virtual long GetRecordCount() = 0;
        
        /*!
          \brief	Next
            Переводит итератор на следующую запись.
        */
        virtual void Next() = 0;

        /*!
          \brief	IsEOF
            Индикатор конца журнала.

          \return	bool - false если запись может быть получена, true если достигнут
            конец журнала.
        */
        virtual bool IsEOF() = 0;

        /*!
          \brief	GetCurrent
            Возвращает текущую запись. Для параметра pRecord вызывается его
             метод Deserialize(), который должен зачесть себя из потока.

          \param	 long & nId [out] - id записи.
          \param	JournalRecord* pRecord [in] - указаель на интерфейс JournalRecord.
        */
        virtual void GetCurrent( long & nId, JournalRecord* pRecord ) = 0;

        /*!
          \brief	GetCurrent
            Возвращает текущую запись. 

          \param	 long & nId [out] - id записи.
          \param	KLPAR::Params** ppParams [out] - Params, в который десериализются данные
        */
        virtual void GetCurrent( long & nId, KLPAR::Params** ppParams ) = 0;

        /*!
          \brief	GetCurrent
            Возвращает текущую запись.

          \param	 long & nId [out] - id записи.
          \param	std::string & strData [in] - строка, представляющая из себя запись.
            Может содержать любые символы, в том числе нули. Строка рассматривается как
            область памяти, начинающаяся в str.c_str(), размером str.size().
        */
        virtual void GetCurrent( long & nId, std::string & strData ) = 0;


        /*!
          \brief	DeleteOldRecords
            Удаляет записи, оставляя nRecordsToLeave самых новых записей.
                Если nRecordsToLeave >= общего количества записей, то удаление
                не производится.

          \param	long nRecordsToLeave - максимальное количество записей, которые
            нужно оставить.
        */
        virtual void DeleteOldRecords( long nRecordsToLeave ) = 0;

        /*!
          \brief	DeleteCurrent
            Удаляет текущую запись.
        */
        virtual void DeleteCurrent() = 0;

        /*!
          \brief	DeleteOlderThanID
            Удаляет записи, добавленные раньше, чем запись с переданным ID,
                и саму запись с таким ID.
          \param	long nID - ID записи, от которой нужно удалять.
        */
        virtual void DeleteOlderThanID( long nID ) = 0;

        /*!
          \brief	DeleteRanges - удаляет записи из журнала, попадающие в переданные диапазоны.
          \param	vectRanges [in] - список диапазонов записей, которые надо удалить.
        */
        virtual void DeleteRanges( const std::vector< std::pair<long, long> > & vectRanges ) = 0;

        /*!
          \brief	DeleteAll
            Удаляет все записи.
        */
        virtual void DeleteAll() = 0;
		
        /*!
          \brief	DeleteAll
            Удаляет неиспользуемые данные из .ctrl файла.
        */
		virtual void ShrinkCentralFile(            
			const std::wstring &    wstrPath,
            const CreationInfo &    creationInfo = CreationInfo(),
            long                    nTimeout = KLJRNL_DEF_OPEN_TIMEOUT ) = 0;

        /*!
          \brief	ResetIterator
            Устанавливает итератор записей на начало журнала. Сразу после этого, 
                если IsEOF()==false, можно получать запись по GetCurrent().

			\param	wstrJournalID [out] - Возвращает идентификатор файла журнала, 
										  который генерируется всякий раз, когда создается 
										  новый Central File
        */
		virtual void ResetIteratorEx(std::string& strJournalID) = 0;

        /*!
          \brief	SetIterator
            Устанавливает итератор на запись с заданным ID. Если такой записи нет, то
            итератор устанавливается на следующую запись. Если следующеая запись
            отсутствует, то указатель устанавливается на конец журнала (IsEOF()==true).
            \param	nId [in] - ID записи.
			\param	wstrJournalID [out] - Возвращает идентификатор файла журнала, 
										  который генерируется всякий раз, когда создается 
										  новый Central File
			\return bool - true если встали точно на запись с заданным ID, false otherwise.
        */
		virtual bool SetIteratorEx(long nId, std::string& strJournalID) = 0;
    };

}


/*!
  \brief	KLJRNL_CreateJournal
                Создает быструю и защищеную от сбоев имплементацию журнала.
  \param	KLJRNL::Journal** ppJournal
  \param	bool bMultiThread
  \return	void 
*/
KLCSKCA_DECL void KLJRNL_CreateJournal(KLJRNL::Journal** ppJournal, bool bMultiThread );

/*!
  \brief	KLJRNL_CreateJournal
                Создает имплементацию журнала, содержимое которго может быть 
                импортировано в различные инструменты, такие как MS SQL и Excel.
                Данные содержаться по принципу "одна запсиь = одна строка", удаление
                записи вызывает заполнение строки пробелами.
  \param	KLJRNL::Journal** ppJournal
  \param	bool bMultiThread
  \return	void 
*/
KLCSKCA_DECL void KLJRNL_CreateSimpleJournal( KLJRNL::Journal** ppJournal, bool bMultiThread );

#endif //__JOURNAL_H__

// Local Variables:
// mode: C++
// End:
