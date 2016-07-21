/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file Error.h
 * \author andkaz
 * \date 2002
 * \brief Стандартный класс для описания ошибки, произошедшей в одном из модулей.
 *
 */

#ifndef KL_ERROR_H
#define KL_ERROR_H

#include <cwchar>

#ifdef __unix__
 #include <stdarg.h>
#endif

#include <std/err/errintervals.h>
#include <std/base/klbase.h>



#include <std/err/error_localized.h> 

/*!
	\brief	Ниже определены макросы KLERR_THROWx для выбрасывания исключений
			с x параметрами.
		\params _module [in] имя модуля, от имени которого будет выброшено исключение
		\params _code [in] код исключения
		\params _parami [in] параметр исключения.
*/

#define KLERR_THROW0(_module, _code)	\
	KLERR_throwError(_module, _code, __FILE__, __LINE__, NULL, NULL)

#define KLERR_THROW1(_module, _code, _param1)	\
	KLERR_throwError(_module, _code, __FILE__, __LINE__, NULL, _param1)

#define KLERR_THROW2(_module, _code, _param1, _param2)	\
	KLERR_throwError(_module, _code, __FILE__, __LINE__, NULL, _param1, _param2)

#define KLERR_THROW3(_module, _code, _param1, _param2, _param3)	\
	KLERR_throwError(_module, _code, __FILE__, __LINE__, NULL, _param1, _param2, _param3)

#define KLERR_THROW4(_module, _code, _param1, _param2, _param3, _param4)	\
	KLERR_throwError(_module, _code, __FILE__, __LINE__, NULL, _param1, _param2, _param3, _param4)

/*!
	\brief	Ниже определены макросы KLERR_MYTHROWx для выбрасывания исключений
			даного модуля с x параметрами. Для их использования необходимо,
			чтобы в использующем их cpp-файле был определён макрос KLCS_MODULENAME
			Пример:
				#define KLCS_MODULENAME L"KLSTD"
*/

/*!
	\brief	Макросы KLERR_THROWx для выбрасывания исключений
			с x параметрами.
		\params _code [in] код исключения
		\params _parami [in] параметр исключения.
*/
#define KLERR_MYTHROW0(_code)	\
	KLERR_throwError(KLCS_MODULENAME, _code, __FILE__, __LINE__, NULL, NULL)

#define KLERR_MYTHROW1(_code, _param1)	\
	KLERR_throwError(KLCS_MODULENAME, _code, __FILE__, __LINE__, NULL, _param1)

#define KLERR_MYTHROW2(_code, _param1, _param2)	\
	KLERR_throwError(KLCS_MODULENAME, _code, __FILE__, __LINE__, NULL, _param1, _param2)

#define KLERR_MYTHROW3(_code, _param1, _param2, _param3)	\
	KLERR_throwError(KLCS_MODULENAME, _code, __FILE__, __LINE__, NULL, _param1, _param2, _param3)

#define KLERR_MYTHROW4(_code, _param1, _param2, _param3, _param4)	\
	KLERR_throwError(KLCS_MODULENAME, _code, __FILE__, __LINE__, NULL, _param1, _param2, _param3, _param4)

/*! /brief	Выбрасыает исключение STDE_SYSTEM c кодом _code
*/
#define KLSTD_THROW_SYSTEM(_code)		KLERR_THROW2(L"KLSTD", KLSTD::STDE_SYSTEM, _code, L"?")

/*! /brief	Выбрасыает исключение STDE_SYSTEM c кодом _code и описанием
*/
#define KLSTD_THROW_SYSTEM_MSG(_code, _msg)		KLERR_THROW2(L"KLSTD", KLSTD::STDE_SYSTEM, _code, _msg)
	
/*! /brief	Выбрасыает исключение STDE_BADPARAM
*/
#define KLSTD_THROW_BADPARAM(_param)	KLERR_THROW1(L"KLSTD", KLSTD::STDE_BADPARAM, #_param)

/*! /brief	Если указатель равен NULL, то STDE_NOMEMORY
*/
#define KLSTD_CHKMEM(_ptr)				(!(_ptr))?(KLSTD_THROW(KLSTD::STDE_NOMEMORY),NULL):1

/*! /brief	Выбрасывает исключение STDE_NOFUNC
*/
#define KLSTD_NOTIMP()						KLSTD_THROW(KLSTD::STDE_NOFUNC)

/*! /brief	Выбрасывает исключение STDE_NOINIT
*/
#define KLSTD_NOINIT(_name)						KLERR_THROW1(L"KLSTD", KLSTD::STDE_NOINIT, (const wchar_t*)_name)

/*! /brief	Выбрасывает исключение STDE_SIGNFAIL
*/
#define KLSTD_SIGNFAIL(_file)   KLERR_THROW1(L"KLSTD", KLSTD::STDE_SIGNFAIL, (const wchar_t*)_file)

/*! /brief	Выбрасывается исключение STDE_ODBC
*/
#define KLSTD_THROW_ODBC(_state, _msg)		KLERR_THROW2(L"KLSTD", KLSTD::STDE_ODBC, (const wchar_t*)_state, (const wchar_t*)_msg)


/*! /brief	Выбрасыает иное исключение STDE_*
*/
#define KLSTD_THROW(_code)					KLERR_THROW0(L"KLSTD", _code)

#define KLSTD_THROW1(_code,_param)			KLERR_THROW1(L"KLSTD", _code, _param)
#define KLSTD_THROW2(_code,_param1,_param2)	KLERR_THROW2(L"KLSTD", _code, _param1, _param2 )

#define KLSTD_THROW_ERRNO() KLSTD_ThrowErrnoCode(errno, __FILE__, __LINE__)
#define KLSTD_THROW_ERRNO_CODE(_code) KLSTD_ThrowErrnoCode(_code, __FILE__, __LINE__)

#define KLSTD_CHKOUTPTR(ppPtr) if(!(ppPtr)){KLSTD_THROW_BADPARAM(ppPtr);KLSTD_ASSERT(*(ppPtr) == NULL);}
#define KLSTD_CHKINPTR(pPtr) if(!(pPtr)){KLSTD_ASSERT((pPtr) != NULL); KLSTD_THROW_BADPARAM(pPtr);}
#define KLSTD_CHK(_name, _condition) if(!(_condition)){KLSTD_ASSERT((_condition)); KLSTD_THROW_BADPARAM(_name);};

/*! /brief	Выбрасыает исключение STDE_ASSRT, если условие _condition не выполнено.
*/
#define KLSTD_ASSERT_THROW(_condition)	KLSTD::assertion_check(!(!(_condition)), #_condition, __FILE__, __LINE__)

/*! /brief	Для использования выбрасывания данных ошибок нужно не забыть
			вызвать KLSTD_Initialize()
*/

KLCSC_DECL void KLSTD_ThrowErrnoCode( int nCode,
                                      const char* szaFile,
                                      int nLine);


KLCSC_DECL void KLSTD_ThrowLastErrorCode( const wchar_t *szwMessage,
                                          long lCode,
                                          bool bThrowAlways,
                                          const char *szaFile,
                                          int nLine,
                                          bool bTryConvert );

KLCSC_DECL void KLSTD_ThrowLastError( const wchar_t *szwMessage,
                                      const char *szaFile,
                                      int nLine,
                                      bool bTryConvert );

#define KLSTD_THROW_LASTERROR_CODE(_code) KLSTD_ThrowLastErrorCode(NULL, _code, true, __FILE__, __LINE__, true)
#define KLSTD_THROW_LASTERROR() KLSTD_ThrowLastError(NULL, __FILE__, __LINE__, true)

#ifdef _WIN32
		
	#define KLSTD_CHKHR(hr) if(FAILED(hr)) KLSTD_ThrowLastErrorCode(NULL, hr, true, __FILE__, __LINE__, true)
	

#endif

	#define KLSTD_THROW_LASTERROR_MSG(_msg)	\
				KLSTD_ThrowLastError(_msg, __FILE__, __LINE__, true)

	#define KLSTD_THROW_LASTERROR_CODEMSG(_code, _msg)	\
				KLSTD_ThrowLastErrorCode(_msg, _code, true, __FILE__, __LINE__, true)

	#define KLSTD_THROW_LASTERROR2() KLSTD_ThrowLastError(NULL, __FILE__, __LINE__, false)
	#define KLSTD_THROW_LASTERROR_CODE2(_code) KLSTD_ThrowLastErrorCode(NULL, _code, true, __FILE__, __LINE__, false)
	
	#define KLSTD_THROW_LASTERROR_MSG2(_msg)	\
				KLSTD_ThrowLastError(_msg, __FILE__, __LINE__, false)

	#define KLSTD_THROW_LASTERROR_CODEMSG2(_code, _msg)	\
				KLSTD_ThrowLastErrorCode(_msg, _code, true, __FILE__, __LINE__, false)



/*!
	\brief	Макросы KLERR_SAY_FAILURE выдаёт в трейс описание ошибки с уровнем level.
		\params level [in] уровень трассировки
		\params pError [in] указатель на интерфейс Error. Если NULL, то исключение
					трактуется как сообщение о нехватке памяти.
*/

#define KLERR_SAY_FAILURE(level, pError)		KLERR::SayFailure(level, pError, KLCS_MODULENAME, __FILE__, __LINE__)

/*!
	\brief Перехват исключений.
*/

#define KLERR_BEGIN								\
			KLERR_TRY

#define KLERR_END								\
            KLERR_CATCH(pError)                 \
                ;                               \
            KLERR_ENDTRY

#define KLERR_ENDT( _traceLevel )				            \
            KLERR_CATCH(pError)                             \
                KLERR_SAY_FAILURE(_traceLevel, pError);     \
            KLERR_ENDTRY

/*!
	\brief Эмуляция отсутствующей в C++ конструкции try-catch-finally.
*/

#define KLERR_TRY									\
	{												\
		KLSTD::CAutoPtr<KLERR::Error> pTmpErrorPtr;	\
		try{

#define KLERR_CATCH(_ptr)							\
		}											\
		catch(KLERR::Error* pError){				\
			KLERR_SAY_FAILURE(5, pError);			\
			if(!pError)								\
				KLERR_CreateError(					\
					&pTmpErrorPtr,					\
					L"KLSTD",						\
					KLSTD::STDE_NOMEMORY,			\
					__FILE__,						\
					__LINE__,						\
					NULL);							\
			pTmpErrorPtr.Attach(pError);			\
		};											\
		KLERR::Error* _ptr=pTmpErrorPtr;			\
		if(pTmpErrorPtr){

#define KLERR_FINALLY								\
		};											\
		{

#define KLERR_ENDTRY								\
		};											\
	};

#define KLERR_RETHROW()								\
		KLERR::rethrow_if_possible(pTmpErrorPtr)


#define KLERR_IGNORE(_expr)	KLERR_BEGIN _expr; KLERR_END

namespace KLERR
{
	const wchar_t szwModule[]=L"KLERR";
    /*!
    * \brief Описание ошибки, произошедшей в одном из модулей.
    *
    *  Разработчик модуля должен определить в своем модуле статический массив
    *  с описанием ошибок, которые могут произойти в модуле. Далее
    *  об этом массиве нужно сообщить модулю Error путем вызова метода
    *  InitModuleDescriptions.  После этого модуль Error будет использовать
    *  описания ошибок из этого массива, если клиент не задал их явно.
    *
    *  В случае, если массив с описаниями ошибок выгружается из оперативной 
    *  памяти, об этом надо сообщить модулю Errors путем вызова метода
    *  DeinitModuleDescriptions.
    *
    */ 

    struct ErrorDescription {
        int id;
        const wchar_t * message;
    };

    class Error2;

    /*!
    * \brief Класс для создания и получения результатов 
    *        описания ошибки, произошедшей в одном из модулей.
    *
    */ 

    class KLSTD_NOVTABLE Error: public KLSTD::KLBase{
    public :
        /*!
            \brief Возвращает идентификатор модуля, возбудившего исключение.

            Имена модулей в системе должны быть уникальны.  Имя модуля может
            совпадать с пространством имен для соответствующего проекта.

            \return Имя модуля, возвращающего исключение.

        */
        KLSTD_NOTHROW virtual const wchar_t * GetModuleName() const throw() = 0;

        /*!
            \brief Возвращает номер ошибки модуля, возбудившего исключение.

            Номера ошибок должны быть уникальны в системе и каждый модуль
            имеет некоторый диапазон номеров ошибок, внутри которого он может
            присваивать своим ошибкам любые идентификаторы.

            \return Номер ошибки.

        */
        KLSTD_NOTHROW virtual int GetId() const throw() = 0;

        /*!
            \brief Возвращает подкод ошибки, специфичный для данной ошибки модуля.
            \return Подкод ошибки.

        */
		KLSTD_NOTHROW virtual long GetErrorSubcode() const throw() = 0;



        /*!
            \brief Возвращает имя файла исходных текстов, где произошло
                   возбуждение исключения.

            Имя файла получается при помощи стандартной директивы препроцессора
            __FILE__.

            \return Имя файла исходных текстов.

        */
        KLSTD_NOTHROW virtual const char * GetFileName() const throw() =0;

        /*!
            \brief Возвращает номер строки в файле исходных текстов, где произошло
                    исключение.

            Строка в файле получается при помощи стандартной директивы препроцессора
            __LINE__.

            \return Строка в файле исходных текстов.

        */
        KLSTD_NOTHROW virtual int GetLine() const throw() = 0;

        /*!
            \brief Возвращает текстовое сообщение о ошибке.

            \return Текстовое сообщение об ошибке.

        */
        KLSTD_NOTHROW virtual const wchar_t * GetMsg() const throw() = 0;

        /*!
            \brief Устанавливает список параметров в сообщении об ошибке.

            Данный метод позволяет передать run time информацию в сообщении
            об ошибке.  В качестве форматной строки для сообщения об ошибке
            используется статическая строка в описании ошибки.  Число и
            тип параметров, передаваемых в arg, должден сооттветствовать
            числу и типу параметров, определенных в форматной строке. Для
            вставки параметров в сообщение об ошибке используется функция
            vswprintf, поэтому формат сообщения об ошибке определяется форматной
            строкой этой функции.
				Размер сообщения после форматирования НЕ ДОЛЖЕН превышать 511
			символов.

            \param arg [in]  Список параметров для вставки в сообщение об ошибке.


        */
        KLSTD_NOTHROW virtual void SetMessageParams(va_list arg) throw() = 0;

        /*!
          \brief	Sets error subcode. (Usually os error code). 

          \param	lSubCode IN subcode
        */
		KLSTD_NOTHROW virtual void SetErrorSubcode(long lSubCode) throw() = 0;

        /*!
          \brief	Returns interface KLERR::Error2

          \return	KLERR::Error2 interface. Method never returns NULL.
        */
        KLSTD_NOTHROW virtual KLERR::Error2* GetError2() throw() = 0;
    };

    typedef KLSTD::CAutoPtr<KLERR::Error> ErrorPtr;

    /*!
    * \brief Класс для создания и получения результатов 
    *        описания ошибки, произошедшей в одном из модулей.
    *
    */
    class KLSTD_NOVTABLE Error2
        :   public KLSTD::KLBaseQI
    {
    public:
        /*!
            \brief see description of Error interface
        */
        KLSTD_NOTHROW virtual const wchar_t * GetModuleName() const throw() = 0;

        /*!
            \brief see description of Error interface
        */
        KLSTD_NOTHROW virtual int GetId() const throw() = 0;

        /*!
            \brief see description of Error interface
        */
		KLSTD_NOTHROW virtual long GetErrorSubcode() const throw() = 0;

        /*!
            \brief see description of Error interface
        */
        KLSTD_NOTHROW virtual const char * GetFileName() const throw() =0;

        /*!
            \brief see description of Error interface
        */
        KLSTD_NOTHROW virtual int GetLine() const throw() = 0;

        /*!
            \brief see description of Error interface
        */
        KLSTD_NOTHROW virtual const wchar_t * GetMsg() const throw() = 0;

        /*!
            \brief see description of Error interface
        */
        KLSTD_NOTHROW virtual void SetMessageParams(va_list arg) throw() = 0;

        /*!
            \brief see description of Error interface
        */
		KLSTD_NOTHROW virtual void SetErrorSubcode(long lSubCode) throw() = 0;

        /*!
          \brief	Returns old Error* interface

          \return	old Error* interface. Method never returns NULL.
        */
        KLSTD_NOTHROW virtual KLERR::Error* GetError() throw() = 0;

        /*!
            \brief Sets previous (parent) error in chain. 
        */
        KLSTD_NOTHROW virtual void SetPreviousError(KLERR::Error* pError) throw() = 0;

        /*!
            \brief Sets previous (parent) error in chain. 
        */
        KLSTD_NOTHROW virtual void SetPreviousError2(KLERR::Error2* pError) throw() = 0;

        /*!
            \brief Returns previous (parent) error in chain. 
        */
        KLSTD_NOTHROW virtual void GetPreviousError(KLERR::Error** ppError) throw() = 0;

        /*!
            \brief Returns previous (parent) error in chain. 
        */
        KLSTD_NOTHROW virtual void GetPreviousError2(KLERR::Error2** ppError) throw() = 0;

        /*!
            \brief Copies error object.
            \return false in case of error (possibly out-of-memory)
        */
        KLSTD_NOTHROW virtual bool Clone(KLERR::Error2** ppError) throw() = 0;
    };

    typedef KLSTD::CAutoPtr<KLERR::Error2> Error2Ptr;

    class ErrAdapt
    {
    public:
        ErrAdapt(KLERR::Error* pError)
            :   m_pError(pError)
            ,   m_pError2(pError?m_pError->GetError2():NULL)
        {;};
        ErrAdapt(KLERR::Error2* pError2)
            :   m_pError2(pError2)
            ,   m_pError(pError2?m_pError2->GetError():NULL)
        {;};
        operator KLERR::Error*()
        {
            return m_pError;
        };

        operator KLERR::Error2*()
        {
            return m_pError2;
        };

        operator KLSTD::KLBaseQI*()
        {
            return (KLERR::Error2*)m_pError2;
        };
    protected:
        KLERR::ErrorPtr     m_pError;
        KLERR::Error2Ptr    m_pError2;
    };

    /*!
        \brief Вспомогательные функции
	*/
	KLSTD_NOTHROW KLCSC_DECL void SayFailure(
										int level,
										Error* perror,
										const wchar_t* module,
										const char* file,
										int line) throw();

	inline void rethrow_if_possible(KLERR::Error* pError)
	{
		if(pError){
			pError->AddRef();
			throw pError;
		};
	};

}

	/*!
		\brief Создает объект, содержащий информацию об ошибке.

		Создает объект, содержащий сообщение об ошибке.  Если клиент
		определяет message, это сообщение используется вместо сообщения,
		определенного в InitModuleDescriptions.
    
		\param ppError [out] Указатель на интерфейс Error
		\param szwModule [in] Имя модуля, породившего ошибку  (чувствительно к регистру !!!).
		\param nId  [in] Идентификатор ошибки.
		\param szaFile [in] Имя файла исходных текстов, где произошла ошибка.
		\param nLine [in] Строка в файле исходных текстов, где произошла ошибка.
		\param szwMessage [in]  Сообщение об ошибке.

		\return true в случае успеха иначе false 
	*/

	KLSTD_NOTHROW KLCSC_DECL void KLERR_CreateError(
		KLERR::Error** ppError,
		const wchar_t*	szwModule,
		int				nId,
		const char*		szaFile=NULL, 
		int				nLine=0,
		const wchar_t*	szwMessage=NULL) throw();


	KLSTD_NOTHROW KLCSC_DECL void KLERR_CreateError2(
		KLERR::Error** ppError,
		const wchar_t*	szwModule,
		int				nId,
		const char*		szaFile, 
		int				nLine,
		const wchar_t*	szwMessage, ...) throw();


    /*!
        \brief Инициализирует список сообщений об ошибках модуля для
               использования в модуле Error.

        Этот метод устанавливает соответствие между конкретным модулем
        и массивом с сообщениями об ошибках.  Массив должен быть 
        корректным для всего времени жизни модуля (до момента вызова
        DeinitModuleDescripitons).  Модуль должен вызвать 
        DeinitModuleDescriptions в процессе прекращения работы модуля.
        Ответственность за управлением памятью, выделенной для массива с
        описанием ошибок модуля, лежит на клиенте модуля Error.  После вызова
        этого метода клиент не должен менять массив с описанием ошибок.
        
        \param szwModule [in] Имя модуля для инициализации массива ошибок (чувствительно к регистру !!!).
        \param descr [in] Массив с описанием ошибок.
        \param arraySize [in] Размер массива с описанием ошибок.

    */

	KLSTD_NOTHROW KLCSC_DECL void KLERR_InitModuleDescriptions( 
        const wchar_t*					szwModule, 
        const KLERR::ErrorDescription*	descr,
        int								arraySize) throw();


    /*!
        \brief Деинициализирует список сообщений об ошибках модуля для
               использования в модуле Error.

        Этот метод разрывает соответствие между конкретным модулем
        и массивом с сообщениями об ошибках.  После этого вызова
        модуль Error перестает использовать массив с описаниями
        ошибок для создания сообщений об ошибках.
        
        \param szaModule [in] Имя модуля для деинициализации массива ошибок.

    */

	KLSTD_NOTHROW KLCSC_DECL void KLERR_DeinitModuleDescriptions(const wchar_t* szaModule) throw();

    /*!
        \brief Поиск описания ошибки во всех модулях.

            Code sample:
                    KLSTD::klwstr_t wstrModule, wstrString;
                    KLERR_FindString(
                            1186,
                            wstrModule.outref(),
                            wstrString.outref());

			\param	nCode [in] Код исключения, которое предстоит найти
			\param	wstrModule [out] Найденное имя модуля
			\param	wstrString [out] Найденное описание ошибки

			\return true в случае успеха иначе false 
	*/

	KLSTD_NOTHROW KLCSC_DECL bool KLERR_FindString(
										int             nCode,
										KLSTD::AKWSTR&  wstrModule,
										KLSTD::AKWSTR&  wstrString) throw();

    /*!
        \brief Поиск описания ошибки

            Code sample:
                    KLSTD::klwstr_t wstrString;
                    KLERR_FindModuleString(
                            KLSTD::STDE_BADPARAM,
                            L"KLSTD",
                            wstrString.outref());

			\param	nCode [in] Код исключения, которое предстоит найти
			\param	szwModule [in] имя модуля
			\param	wstrString [out] Найденное описание ошибки

			\return true в случае успеха иначе false 
	*/
    KLSTD_NOTHROW KLCSC_DECL bool KLERR_FindModuleString(
								int				nCode,
								const wchar_t*  szwModule,
								KLSTD::AKWSTR&	wstrString) throw();

    /*!
      \brief	Finds error description in a sorted array of 
                KLERR::ErrorDescription
      \param	nCode IN code 
      \param	pData IN array
      \param	nData IN array length
      \return	description or L"" if not found
  
    */
    KLSTD_NOTHROW KLCSC_DECL const wchar_t* KLERR_FindString2(
							int				                nCode,
                            const KLERR::ErrorDescription*  pData,
                            size_t                          nData) throw();

    /*!
        \brief Содаёт и выбрасывает исключение. Если недостаточно памяти для создание исключение,
					вместо него выбрасывается созданное заранее STDE_NOMEMORY
			\param szwModule [in] Имя модуля
			\param nId [in] Код исключения
			\param szaFile [in] имя файла, из которого выбрасывается исключение (__FILE__)
			\param nLine [in] строка в которой выбрасывается исключение (__LINE__)
			\param szwMessage [in] Необязательное описание исключения
			\param ... [in] Аргументы, требующиеся для данного исключения
	*/

	KLCSC_DECL KLSTD_NORETURN void KLERR_throwError(
		const wchar_t*	szwModule,
		int				nId,
		const char*		szaFile, 
		int				nLine,
		const wchar_t*	szwMessage...);


    /*!
      \brief	

      \param	Error code
      \param	Zero ended array opf codes to ignore
      \return	Returns true if ignore exception
    */
    KLSTD_NOTHROW KLCSC_DECL bool KLERR_IfIgnore(long lCode, ...)throw();

#endif // KL_ERROR_H

// Local Variables:
// mode: C++
// End:
