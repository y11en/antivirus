/*!
 * (C) 2002 "Kaspersky Lab"
 *
 * \file KlErrors.h
 * \author Андрей Казачков
 * \date 2002
 * \brief Стандартные коды ошибок
 *
 */


#ifndef KLERRORS_H_7A4490CD_96DC_4d0b_826E_5DE35C1EFB3A
#define KLERRORS_H_7A4490CD_96DC_4d0b_826E_5DE35C1EFB3A

#include "std/err/errintervals.h"
#include "std/err/error.h"
#include <errno.h>


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

//! Throws STDE_UNAVAIL with localization
#define KLSTD_THROW_APP_PENDING()   \
                KLSTD_ThrowAppPending(__FILE__, __LINE__)

/*!
  \brief	Throws STDE_UNAVAIL with localization
*/
KLCSC_DECL void KLSTD_ThrowAppPending(const char* szaFile, int nLine);



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


#endif


namespace KLSTD{
	enum{
		// general errors
		STDE_NOERROR=KLSTDERRSTART,		//No error
		STDE_SYSTEM,		//OS specific error
		STDE_GENERAL,		//Unspecified error,
		STDE_NOMEMORY,		//Not enough memory
		STDE_NOACCESS,		//Access denied
		STDE_NOINIT,		//Object not initialized
		STDE_NOTFOUND,		//Object not found
		STDE_BADPARAM,		//Bad parameter (Invalid function argument)
		STDE_BOUND,			//Index out of range
		STDE_BADHANDLE,		//Invalid handle
		STDE_MOREDATA,		//More data available (Buffer too small)
		STDE_TOOBIG,		//Object size too large
		STDE_BADFORMAT,		//Data is corrupted or has an unknown format
		STDE_NOTPERM,		//Operation not permitted
		STDE_NOFUNC,		//Function not implemented
		STDE_UNAVAIL,		//Resource unavailable
		STDE_FAULT,			//Invalid memory address (Access violation)
		STDE_TIMEOUT,		//Operation failed because the timeout period expired
		STDE_ASSRT,			//Assertion failed
		STDE_CANCELED,		//Operation canceled
        STDE_SIGNFAIL,      //Signature doesn't match
		STDE_ODBC,			//ODBC 
		STDE_ADO,			//ADO
		STDE_BAD_ADO_FIELD_TYPE,
        STDE_UNSUPPORTED_TYPE_OF_VALUE_IN_ADO_OUT_PARAM,
		
		// filesystem errors 
		STDE_NOENT,			//No such file or directory
		STDE_IO,			//Input/output error
		STDE_NXIO,			//No such device or address
		STDE_EXIST,			//File exists
		STDE_NODEV,			//No such device
		STDE_NOTDIR,		//Not a directory
		STDE_ISDIR,			//Is a directory
		STDE_NFILE,			//Too many open files in system
		STDE_MFILE,			//Too many open files
		STDE_NOTTY,			//Inappropriate I/O control operation
		STDE_NOSPC,			//No space left on device
		STDE_SPIPE,			//Invalid seek
		STDE_ROFS,			//Read-only file system
		STDE_NAMETOOLONG,	//Filename too long
		STDE_NOTEMPTY,		//Directory not empty
		// Process & thread errors
		STDE_CREATE_THREAD,	//Couldn't create thread
        STDE_TEST_ERROR,  	//Test error
        STDE_THREAD_PARAM_EXISTS,
        STDE_THREAD_PARAM_NOT_EXISTS,
		// general errors 2
		STDE_OBJ_EXISTS			//Object already exists
	};

	//! Перечисление кодов ошибок модуля Std
	//  Любой из методов любого модуля может вернуть исключение с этим кодом ошибки.
	const KLERR::ErrorDescription c_errorDescriptions[]={
		// general errors
		{STDE_NOERROR,		L"No error"},
		{STDE_SYSTEM,		L"System error 0x%X (%ls)"},
		{STDE_GENERAL,		L"Unspecified error"},
		{STDE_NOMEMORY,		L"Not enough memory"},
		{STDE_NOACCESS,		L"Access denied"},
		{STDE_NOINIT,		L"Object %ls is not initialized"},
		{STDE_NOTFOUND,		L"Object not found"},
		{STDE_BADPARAM,		L"Bad parameter \"%hs\""},
		{STDE_BOUND,		L"Index value is out of range"},
		{STDE_BADHANDLE,	L"Handle is invalid"},
		{STDE_MOREDATA,		L"Buffer too small"},
		{STDE_TOOBIG,		L"Object size is too large"},
		{STDE_BADFORMAT,	L"Data is corrupted or has an unknown format"},
		{STDE_NOTPERM,		L"Operation is not permitted"},
		{STDE_NOFUNC,		L"Function is not implemented"},
		{STDE_UNAVAIL,		L"Resource is unavailable"},
		{STDE_FAULT,		L"Access violation fault"},
		{STDE_TIMEOUT,		L"Operation failed because the timeout period expired"},
		{STDE_ASSRT,		L"Assertion \"%hs\" failed"},
		{STDE_CANCELED,		L"Operation canceled"},
        {STDE_SIGNFAIL,     L"Signature mismatch for file '%ls'"},
		{STDE_ODBC,			L"%ls-ODBC error \"%ls\""},
		{STDE_ADO,			L"ADO error. Code: 0x%x, message: \"%ls\", description: \"%ls\", source: \"%ls\", GUID: \"%ls\"" },
		{STDE_BAD_ADO_FIELD_TYPE, L"Failed to get %s value from field %s"},
        {STDE_UNSUPPORTED_TYPE_OF_VALUE_IN_ADO_OUT_PARAM, L"Unsupported type of value (%d) in ADO output parameter \"%s\" in %s stored procedure."},

		{STDE_NOENT,		L"No such file or directory"},
		{STDE_IO,			L"Input/output error"},
		{STDE_NXIO,			L"No such device or address"},
		{STDE_EXIST,		L"File exists"},
		{STDE_NODEV,		L"No such device"},
		{STDE_NOTDIR,		L"Not a directory"},
		{STDE_ISDIR,		L"Is a directory"},
		{STDE_NFILE,		L"Too many open files in system"},
		{STDE_MFILE,		L"Too many open files"},
		{STDE_NOTTY,		L"Inappropriate I/O control operation"},
		{STDE_NOSPC,		L"No space left on device"},
		{STDE_SPIPE,		L"Invalid seek"},
		{STDE_ROFS,			L"Read-only file system"},
		{STDE_NAMETOOLONG,	L"Filename too long"},
		{STDE_NOTEMPTY,		L"Directory not empty"},
		// Process & thread errors
		{STDE_CREATE_THREAD,L"Couldn't create thread. Thread name - '%ls'. System error - %d."},
        {STDE_TEST_ERROR,  L"Test error: %s"},
        {STDE_THREAD_PARAM_EXISTS, L"Thread param \"%s\" already exists in thread store for this thread (thread id 0x%010x)."},
        {STDE_THREAD_PARAM_NOT_EXISTS, L"Thread param \"%s\" does not exists in thread store for this thread (thread id 0x%010x)."},
		// general errors 2
		{STDE_OBJ_EXISTS,	L"Object exists."}
	};

	KLCSC_DECL void assertion_check(bool bOK, const char* szaString, const char* szaFile, int nLine);
};

#endif //KLERRORS_H_7A4490CD_96DC_4d0b_826E_5DE35C1EFB3A
