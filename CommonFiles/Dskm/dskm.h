/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	dskm.h
*		\brief	
*		
*		\author Victor Matioshenkov
*		\date	6/15/2005 11:26:24 AM
*		
*		Example:	
*		
*		
*		
*/		


#ifndef __dskm_h__
#define __dskm_h__

#include "../AVP_data.h"

#ifndef DSKMAPI
#if defined (__unix__)
#define DSKMAPI
#else
#define DSKMAPI __cdecl
#endif /* __unix__ */
#endif /* DSKMAPI */


#if defined( __cplusplus )
extern "C" {
#endif

#define DSKM_OTYPE_KEY_KL_MASTER_PRIVATE	(0x7fffff)
#define DSKM_OTYPE_KEY_KL_MASTER_PUBLIC		(0x7ffffe)
#define DSKM_OTYPE_KEY_KL_PRIVATE					(0x7ffffd)
#define DSKM_OTYPE_KEY_KL_PUBLIC					(0x7ffffc)
#define DSKM_OTYPE_KEY_OPERABLE_PRIVATE		(0x7ffffb)
#define DSKM_OTYPE_KEY_OPERABLE_PUBLIC		(0x7ffffa)

#define DSKM_OTYPE_LICENCE_KEY            (0x7ffff9)  
#define DSKM_OTYPE_LICENCE_KEY_ORDER      (0x7ffff8)  

#define DSKM_OTYPE_ASSOC_FILE             (0x7ffff7)

#define DSKM_OTYPE_USER_MIN               (0x000001)
#define DSKM_OTYPE_USER_MAX               (0x7ffff6)

#define DSKM_ERR_OK												0xe9ba5770 /*is a crc32 for "DSKM_ERR_OK"*/                       // (0x0)
#define DSKM_ERR_NOT_INITIALIZED					0xd10cc37a /*is a crc32 for "DSKM_ERR_NOT_INITIALIZED"*/					// (0x1)
#define DSKM_ERR_IO_NOT_LOADED						0x48097982 /*is a crc32 for "DSKM_ERR_IO_NOT_LOADED"*/						// (0x2)
#define DSKM_ERR_CANNOT_CREATE_REG_FILE		0x68b0acf0 /*is a crc32 for "DSKM_ERR_CANNOT_CREATE_REG_FILE"*/		// (0x3)
#define DSKM_ERR_CANNOT_WRITE_REG_FILE		0x308015e7 /*is a crc32 for "DSKM_ERR_CANNOT_WRITE_REG_FILE"*/		// (0x4)
#define DSKM_ERR_CANNOT_WRITE_REG_TOMEM		0xdab8d467 /*is a crc32 for "DSKM_ERR_CANNOT_WRITE_REG_TOMEM"*/		// (0x5)
#define DSKM_ERR_CANNOT_OPEN_REG_FILE			0x3d0507bd /*is a crc32 for "DSKM_ERR_CANNOT_OPEN_REG_FILE"*/			// (0x6)
#define DSKM_ERR_CANNOT_READ_REG_FILE			0xfd6eb8be /*is a crc32 for "DSKM_ERR_CANNOT_READ_REG_FILE"*/			// (0x7)
#define DSKM_ERR_INVALID_REG_FILE					0xc2fef413 /*is a crc32 for "DSKM_ERR_INVALID_REG_FILE"*/					// (0x8)
#define DSKM_ERR_INVALID_REG_FILE_VERSION 0x2a592774 /*is a crc32 for "DSKM_ERR_INVALID_REG_FILE_VERSION"*/ // (0x9)
#define DSKM_ERR_INVALID_REG_FILE_OBJTYPE 0x4ef35e4d /*is a crc32 for "DSKM_ERR_INVALID_REG_FILE_OBJTYPE"*/	// (0xa)
#define DSKM_ERR_KEY_NOT_FOUND						0x408b1c02 /*is a crc32 for "DSKM_ERR_KEY_NOT_FOUND"*/						// (0xb)
#define DSKM_ERR_REG_NOT_FOUND						0x8f995ffd /*is a crc32 for "DSKM_ERR_REG_NOT_FOUND"*/						// (0xc)
#define DSKM_ERR_CANNOT_CALC_OBJ_HASH			0x7d11f7e2 /*is a crc32 for "DSKM_ERR_CANNOT_CALC_OBJ_HASH"*/			// (0xd)
#define DSKM_ERR_NOT_ALL_OBJECTS_FOUND		0xc58506ed /*is a crc32 for "DSKM_ERR_NOT_ALL_OBJECTS_FOUND"*/		// (0xe)
#define DSKM_ERR_INVALID_SIGN							0x38f3542d /*is a crc32 for "DSKM_ERR_INVALID_SIGN"*/							// (0xf)
#define DSKM_ERR_INVALID_BUFFER						0xdeabf3f6 /*is a crc32 for "DSKM_ERR_INVALID_BUFFER"*/						// (0x10)
#define DSKM_ERR_INVALID_PARAMETER				0x5aaeeae0 /*is a crc32 for "DSKM_ERR_INVALID_PARAMETER"*/				// (0x11)
#define DSKM_ERR_INVALID_OBJTYPE					0xe75abf28 /*is a crc32 for "DSKM_ERR_INVALID_OBJTYPE"*/  				// (0x12)
#define DSKM_ERR_DATA_PROCESSING					0x6eb9a4a4 /*is a crc32 for "DSKM_ERR_DATA_PROCESSING"*/					// (0x13)
#define DSKM_ERR_HASH_NOT_FOUND						0x9cef5380 /*is a crc32 for "DSKM_ERR_HASH_NOT_FOUND"*/						// (0x14)
#define DSKM_ERR_ANOTHER_BASE_REG_FOUND		0xdec30ebe /*is a crc32 for "DSKM_ERR_ANOTHER_BASE_REG_FOUND"*/		// (0x15)
#define DSKM_ERR_LIB_CODE_HACKED					0x7051b0ac /*is a crc32 for "DSKM_ERR_LIB_CODE_HACKED"*/				  // (0x16)
#define DSKM_ERR_CANNOT_CREATE_SIGN				0x5b118908 /*is a crc32 for "DSKM_ERR_CANNOT_CREATE_SIGN"*/				// (0x17)
#define DSKM_ERR_CRIPTO_LIB								0xa0f61271 /*is a crc32 for "DSKM_ERR_CRIPTO_LIB"*/								// (0x18)
#define DSKM_ERR_OBJECT_NOT_FOUND         0xa200485b /*is a crc32 for "DSKM_ERR_OBJECT_NOT_FOUND"*/         // (0x19)
#define DSKM_ERR_CALLBACK_READ_ERROR			0x96bdea9f /*is a crc32 for "DSKM_ERR_CALLBACK_READ_ERROR"*/			// (0x1a)
#define DSKM_ERR_CANNOT_OPEN_FILE					0x14064f2a /*is a crc32 for "DSKM_ERR_CANNOT_OPEN_FILE"*/					// (0x1b)
#define DSKM_ERR_IO_NOT_INITIALIZED       0x04529da3 /*is a crc32 for "DSKM_ERR_IO_NOT_INITIALIZED"*/				// (0x1c)
#define DSKM_ERR_REGS_NOT_INITIALIZED			0xeb529c3d /*is a crc32 for "DSKM_ERR_REGS_NOT_INITIALIZED"*/			// (0x1d)
#define DSKM_ERR_MORE_THAN_ONE_KEY        0xc9098782 /*is a crc32 for "DSKM_ERR_MORE_THAN_ONE_KEY"*/				// (0x1e)
#define DSKM_ERR_SIGN_NOT_FOUND						0x08cd9bac /*is a crc32 for "DSKM_ERR_SIGN_NOT_FOUND"*/						// (0x1f)
#define DSKM_ERR_ANOTHER_SIGN_FOUND				0xde3346ac /*is a crc32 for "DSKM_ERR_ANOTHER_SIGN_FOUND"*/				// (0x20)
#define DSKM_ERR_INVALID_SIZE							0x504de1b9 /*is a crc32 for "DSKM_ERR_INVALID_SIZE"*/							// (0x21)
#define DSKM_ERR_PROP_NOT_FOUND						0xddee3e3d /*is a crc32 for "DSKM_ERR_PROP_NOT_FOUND"*/						// (0x22)
#define DSKM_ERR_OBJECTS_NOT_EQUAL				0x2ba71ba6 /*is a crc32 for "DSKM_ERR_OBJECTS_NOT_EQUAL"*/				// (0x23)
#define DSKM_UNKNOWN_ERROR								0x9f1e269c /*is a crc32 for "DSKM_UNKNOWN_ERROR"*/                // (0xffff) 

#define DSKM_OK(a)												(!((a)^DSKM_ERR_OK))
#define DSKM_NOT_OK(a)										(((a)^DSKM_ERR_OK))

#define DSKM_OBJ_SIGN_INREG								(0x0001)
#define DSKM_OBJ_SIGN_INOBJ								(0x0002)

#define DSKM_OBJ_EXTERNAL_BUFFER_PTR      (0x0001)	// the size is "sizeof(void *)"
#define DSKM_OBJ_EXTERNAL_PARAM_PTR       (0x0002)	// the size is "sizeof(void *)"
#define DSKM_OBJ_PROCESSING_ERROR					(0x0003)	// the size is "sizeof(AVP_dword)"
#define DSKM_OBJ_SIGNED_BY								(0x0004)	// the size is "sizeof(AVP_dword)"; value is one of DSKM_OBJ_SIGN_*
#define DSKM_OBJ_USERDATA_MIN             (0x0010)
#define DSKM_OBJ_USERDATA_MAX             (0xffff)



typedef struct tagDSKM {
	AVP_byte _;
} *HDSKM;
	

typedef struct tagDSKMLIST {
	AVP_byte _;
	} *HDSKMLIST;


typedef struct tagDSKMLISTOBJ {
	AVP_byte _;
} *HDSKMLISTOBJ;


	
#define DSKM_CB_OPEN  1	
#define DSKM_CB_READ  2
#define DSKM_CB_WRITE 3	
#define DSKM_CB_CLOSE 4

//! \fn				: 	DSKMAPI* pfnDSKM_GetBuffer_CallBack
//! \brief			:	Callback подкачки буфера объекта
//! \return			: typedef int - количество записанных байт
//! \param          : void *pParams - параметры функции
//! \param          : void *pvBuffer	- указатель на буфер чтения
//! \param          : AVP_dword dwBufferSize - размер буфера чтения
//! \param          : AVP_dword dwCommand - код команды (DSKM_CB_*)
typedef int (DSKMAPI* pfnDSKM_GetBuffer_CallBack)( void *pParams, void *pvBuffer, AVP_dword dwBufferSize, AVP_dword dwCommand );



// ---
typedef struct DSKM_Io {									
	struct DSKM_Io_Vtbl *lpVtbl;
	unsigned int	       m_cRef;				
} DSKM_Io;											

typedef struct DSKM_Io_Vtbl DSKM_Io_Vtbl; 

// ---
struct DSKM_Io_Vtbl {
	unsigned int (*AddRef)( DSKM_Io *This );
	unsigned int (*Release) ( DSKM_Io *This );
	void * (*CreateFile)(
		DSKM_Io *This,
    const char   *lpFileName,									// pointer to name of the file
    unsigned long dwDesiredAccess,						// access (read-write) mode
    unsigned long dwShareMode,								// share mode
    void*  lpSecurityAttributes,							// pointer to security descriptor SECURITY_ATTRIBUTES FAR
    unsigned long  dwCreationDistribution,		// how to create
    unsigned long  dwFlagsAndAttributes,			// file attributes
    void *hTemplateFile												// HANDLE handle to file with attributes to copy
	);
	int (*CloseHandle)(
		DSKM_Io *This,
    void *  hObject														// HANDLE handle to object to close
	);
	int (*ReadFile )(
		DSKM_Io *This,
    void*  hFile,															// HANDLE handle of file to read
    void*  lpBuffer,													// address of buffer that receives data
    unsigned long  nNumberOfBytesToRead,			// number of bytes to read
    unsigned long*  lpNumberOfBytesRead,			// address of number of bytes read
    void*  lpOverlapped												// OVERLAPPED FAR * address of structure for data BYTE Disk, WORD Sector ,BYTE Head ,LPBYTE Buffer)
	);
	int (*WriteFile )(
		DSKM_Io *This,
    void*  hFile,															// HANDLE handle to file to write to
    const void*  lpBuffer,										// pointer to data to write to file
    unsigned long  nNumberOfBytesToWrite,     // number of bytes to write
    unsigned long*  lpNumberOfBytesWritten,   // pointer to number of bytes written
    void*  lpOverlapped												// OVERLAPPED FAR * addr. of structure needed for overlapped I/O
	);
	unsigned long (*SetFilePointer)(
		DSKM_Io *This,
    void*  hFile,															// HANDLE handle of file
    long  lDistanceToMove,										// number of bytes to move file pointer
    long*  lpDistanceToMoveHigh,							// address of high-order word of distance to move
    unsigned long  dwMoveMethod								// how to move
	);
	unsigned long (*GetFileSize)(
		DSKM_Io *This,
    void*  hFile,															// HANDLE handle of file
    unsigned long*  lpHigh
	);
	void* (*FindFirstFile)(
		DSKM_Io *This,
		const char* lpFileFolder,									// file folder
		char*  lpFoundFileName,										// found file
		unsigned long   dwFileNameSize						// found file name size
	);
	int (*FindNextFile)(
		DSKM_Io *This,
		void*	hFindFile,													// HANDLE search handle 
		char*  lpFoundFileName,										// found file
		unsigned long   dwFileNameSize						// found file name size
	);
	int (*FindClose)(
		DSKM_Io *This,
		void*	hFindFile														// HANDLE search handle 
	); 
};


//---------------------------------------------------------------------------------------------------------------------------
// Функции инициирования/освобождения библиотеки
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_InitLibrary
//! \brief			:	Инициирование библиотеки.
//! \return			: Контекст библиотеки. 0 - операция неуспешна. 
//! \param          : (*pfnAlloc)(AVP_uint) - указатель на функцию распределения памяти
//! \param          : void (*pfnFree((void *)	- указатель на функцию освобождения памяти
//! \param          : AVP_bool bInitSubLib - нужно ли инициировать вспомогательные библиотеки (Property.lib)
HDSKM	    DSKMAPI DSKM_InitLibrary( void* (DSKMAPI *pfnAlloc)(AVP_size_t), void (DSKMAPI *pfnFree)(void*), AVP_bool bInitSubLib );


//! \fn				: DSKM_InitLibraryEx
//! \brief			:	Инициирование библиотеки.
//! \return			: Контекст библиотеки. 0 - операция неуспешна. 
//! \param          : (*pfnAlloc)(AVP_uint) - указатель на функцию распределения памяти
//! \param          : void (*pfnFree((void *)	- указатель на функцию освобождения памяти
//! \param          : DSKM_Io *pIo - указатель на интерфейс ввода/вывода (НЕ может быть 0)
//! \param          : AVP_bool bInitSubLib - нужно ли инициировать вспомогательные библиотеки (Property.lib)
HDSKM	    DSKMAPI DSKM_InitLibraryEx( void* (DSKMAPI *pfnAlloc)(AVP_size_t), void (DSKMAPI *pfnFree)(void*), DSKM_Io *pIo, AVP_bool bInitSubLib );


//! \fn				: DSKM_DeInitLibrary
//! \brief			:	Завершение работы библиотеки 
//! \return			: Код ошибки 
//! \param          : HDSKM hDSKM - контекст библиотеки
//! \param          : AVP_bool bDeInitSubLib - нужно ли де-инициировать вспомогательные библиотеки (Property.lib)
AVP_dword	DSKMAPI DSKM_DeInitLibrary( HDSKM hDSKM, AVP_bool bDeInitSubLib );


//---------------------------------------------------------------------------------------------------------------------------
// Функции работы с вектором объектов и параметров
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_ParList_Create
//! \brief			:	Создание списка параметров операции
//! \return			: Код ошибки 
//! \param          : HDSKMLIST *hParList - указатель на создаваемый список
AVP_dword		DSKMAPI DSKM_ParList_Create( HDSKMLIST *hParList );



//! \fn				: DSKM_ParList_Delete
//! \brief			:	Удалить список параметров операции
//! \return			: Код ошибки
//! \param          : HDSKMLIST hList - список для удаления
AVP_dword 	DSKMAPI DSKM_ParList_Delete( HDSKMLIST hList );


//---------------------------------------------------------------------------------------------------------------------------
// Функции добавления параметров в список
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_ParList_AddStrParam
//! \brief			:	Добавить строковый параметр списка
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : AVP_dword dwParamId - идентификатор параметра (должен быть уникальным)
//! \param          : const AVP_char* pszStrParam	- параметр для добавления (строка копируется)
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddStrParam( HDSKMLIST hList, AVP_dword dwParamId, const AVP_char* pszStrParam );


//! \fn				: DSKM_ParList_AddDWordParam
//! \brief			:	Добавить целочисленный параметр	списка
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : AVP_dword dwParamId - идентификатор параметра	(должен быть уникальным)
//! \param          : AVP_dword dwParam	- параметр для добавления
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddDWordParam( HDSKMLIST hList, AVP_dword dwParamId, AVP_dword dwParam );


//! \fn				: DSKM_ParList_AddBinaryPtrParam
//! \brief			:	Добавить двоичный параметр списка
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : AVP_dword dwParamId - идентификатор параметра	(должен быть уникальным)
//! \param          : void *pvParamPtr	- буфер для добавления (копируется)
//! \param          : AVP_dword dwBinarySize - размер буфера
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddBinaryPtrParam( HDSKMLIST hList, AVP_dword dwParamId, void *pvParamPtr, AVP_dword dwBinarySize );


//---------------------------------------------------------------------------------------------------------------------------
// Функции добавления параметров в объект проверки (приеняется только в схеме DSKM_PrepareRegsSet/DSKM_CheckObjectsUsingRegsSet)
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_ParList_AddObjectStrParam
//! \brief			:	Добавить строковый параметр списка
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : AVP_dword dwParamId - идентификатор параметра (должен быть уникальным)
//! \param          : const AVP_char* pszStrParam	- параметр для добавления (строка копируется)
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectStrParam( HDSKMLIST hList, HDSKMLISTOBJ hListObject, AVP_dword dwParamId, const AVP_char* pszStrParam );


//! \fn				: DSKM_ParList_AddObjectDWordParam
//! \brief			:	Добавить целочисленный параметр	списка
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : AVP_dword dwParamId - идентификатор параметра	(должен быть уникальным)
//! \param          : AVP_dword dwParam	- параметр для добавления
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectDWordParam( HDSKMLIST hList, HDSKMLISTOBJ hListObject, AVP_dword dwParamId, AVP_dword dwParam );


//! \fn				: DSKM_ParList_AddObjectBinaryPtrParam
//! \brief			:	Добавить двоичный параметр списка
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : AVP_dword dwParamId - идентификатор параметра	(должен быть уникальным)
//! \param          : void *pvParamPtr	- буфер для добавления (копируется)
//! \param          : AVP_dword dwBinarySize - размер буфера
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectBinaryPtrParam( HDSKMLIST hList, HDSKMLISTOBJ hListObject, AVP_dword dwParamId, void *pvParamPtr, AVP_dword dwBinarySize );


//---------------------------------------------------------------------------------------------------------------------------
// Функции добавления ассоциаций 
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				:  DSKMAPI DSKM_ParList_AddFileAssociation
//! \brief			:	Добавить ассоциацию файлов реестров
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : AVP_dword dwObjectsType - тип объекта (DSKM_OTYPE_KEY_* или пользовательский)
//! \param          : const AVP_char* pObjRegFileName - имя файла с подписями 
//! \param          : AVP_dword dwObjRegFileNameSize - размер имени в байтах (c нулевым байтом)
//! \param          : (если 0 - строка считается ANSI и размер считается до 0-байта)
//! \param          : const AVP_char* dwObjRegFileName	- имя файла с ключами
//! \param          : AVP_dword dwKeyRegFileNameNameSize - размер имени в байтах (c нулевым байтом)
//! \param          : (если 0 - строка считается ANSI и размер считается до 0-байта)
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddFileAssociation( HDSKMLIST hList, AVP_dword dwObjectsType, const AVP_char* pszObjRegFileName, AVP_dword dwObjRegFileNameSize, const AVP_char* pszKeyRegFileName, AVP_dword dwKeyRegFileNameNameSize );



//! \fn				:  DSKMAPI DSKM_ParList_AddObjectTypeAssociationFile
//! \brief			:	Добавить файл ассоциаций типов
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : const AVP_char* pszAssocFileName - имя файла
//! \param          : AVP_dword dwAssocFileNameSize - размер имени в байтах (c нулевым байтом)
//! \param          : (если 0 - строка считается ANSI и размер считается до 0-байта)
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectTypeAssociationFile( HDSKMLIST hList, const AVP_char* pszAssocFileName, AVP_dword dwAssocFileNameSize );



//! \fn				:  DSKMAPI DSKM_ParList_AddObjectTypeAssociationBufferedObject
//! \brief			:	Добавить буферизованный объект ассоциаций типов
//! 							(буфер будет использован при выполнении операции подписывания/проверки)
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : void *pvBuffer - указатель на буфер (не может быть нулевым)
//! \param          : AVP_dword dwBufferSize - размер буфера (не может быть нулевым)
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack - функция подкачки буфера 
//!										(может быть нулевой - используется наполненный буфер)
//! \param          : void *pvCallBackParams - параметры функции подкачки
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectTypeAssociationBufferedObject( HDSKMLIST hList, const void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams );


//---------------------------------------------------------------------------------------------------------------------------
// Функции добавления объектов
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_ParList_AddNamedObject
//! \brief			:	Добавить именованный объект операции (файл)
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : AVP_dword dwObjectId - идентификатор объекта. Участвует ТОЛЬКО в операции. Должен быть уникальным.
//! \param          : const AVP_char* pszObjectName	- имя объекта (файла)
//! \param          : AVP_dword dwObjectNameSize - размер имени в байтах (c нулевым байтом)
//! \param          : (если 0 - строка считается ANSI и размер считается до 0-байта)
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddNamedObject( HDSKMLIST hList, AVP_dword dwObjectId, const AVP_char* pszObjectName, AVP_dword dwObjectNameSize );


//! \fn				: DSKM_ParList_AddBufferedObject
//! \brief			:	Добавить буферизованный объект операции	(буфер НЕ копируется)
//! 							(буфер будет использован при выполнении операции подписывания/проверки)
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : AVP_dword dwObjectId - идентификатор объекта. Участвует ТОЛЬКО в операции. Должен быть уникальным.
//! \param          : void *pvBuffer - указатель на буфер (не может быть нулевым)
//! \param          : AVP_dword dwBufferSize - размер буфера (не может быть нулевым)
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack - функция подкачки буфера 
//!												(может быть нулевой - используется наполненный буфер)
//! \param          : void *pvCallBackParams - параметры функции подкачки
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddBufferedObject( HDSKMLIST hList, AVP_dword dwObjectId, void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams );


//! \fn				: DSKM_ParList_AddObjectHash
//! \brief			:	Добавить объект операции в виде подготовленного ранее хэша
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : AVP_dword dwObjectId - идентификатор объекта. Участвует ТОЛЬКО в операции. Должен быть уникальным.
//! \param          : void *pvHashPtr - указатель на хэш
//! \param          : AVP_dword dwHashSize - размер хэша
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddObjectHash( HDSKMLIST hList, AVP_dword dwObjectId, void *pvHashPtr, AVP_dword dwHashSize );


//---------------------------------------------------------------------------------------------------------------------------
// Функции добавления реестров
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_ParList_AddNamedReg
//! \brief			:	Добавить именованный реестр (файл)
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : AVP_dword dwRegId - идентификатор реестра. Участвует ТОЛЬКО в операции. Должен быть уникальным (вместе с объектами).
//! \param          : const AVP_char* pszRegName	- имя реестра (файла)
//! \param          : AVP_dword dwRegNameSize - размер имени в байтах (c нулевым байтом)
//! \param          : (если 0 - строка считается ANSI и размер считается до 0-байта)
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddNamedReg( HDSKMLIST hList, AVP_dword dwRegId, const AVP_char* pszRegName, AVP_dword dwRegNameSize );


//! \fn				: DSKM_ParList_AddBufferedReg
//! \brief			:	Добавить буферизованный реестр	(буфер НЕ копируется)
//! 							(буфер будет использован при выполнении операции подписывания/проверки)
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список для добавления
//! \param          : AVP_dword dwRegId - идентификатор объекта. Участвует ТОЛЬКО в операции. Должен быть уникальным  (вместе с объектами).
//! \param          : void *pvBuffer - указатель на буфер (не может быть нулевым)
//! \param          : AVP_dword dwBufferSize - размер буфера (не может быть нулевым)
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack - функция подкачки буфера 
//!												(может быть нулевой - используется наполненный буфер)
//! \param          : void *pvCallBackParams - параметры функции подкачки
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_AddBufferedReg( HDSKMLIST hList, AVP_dword dwRegId, void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams );



//---------------------------------------------------------------------------------------------------------------------------
// Функции удаления отдельного объекта
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_ParList_RemoveObjectById
//! \brief			:	Удалить объект из списка через ID
//! \return			: Код ошибки 
//! \param          :  HDSKMLIST hList - список объектов
//! \param          : AVP_dword dwObjectId - идентификатор удаляемого (НЕ может быть 0)
AVP_dword 	DSKMAPI DSKM_ParList_RemoveObjectById( HDSKMLIST hList, AVP_dword dwObjectId );


//! \fn				: DSKM_ParList_RemoveObjectByH
//! \brief			:	Удалить объект из списка
//! \return			: Код ошибки 
//! \param          :  HDSKMLIST hList - список объектов
//! \param          : HDSKMLISTOBJ hListObject - объект для удаления
AVP_dword 	DSKMAPI DSKM_ParList_RemoveObjectByH( HDSKMLIST hList, HDSKMLISTOBJ hListObject );


//---------------------------------------------------------------------------------------------------------------------------
// Функции работы с ошибками обработки объекта
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				:  DSKMAPI DSKM_ParList_GetFirstObjectError
//! \brief			:	Получить первый объект с ошибкой обработки или 0, если объектов нет
//! \return			: Идентификатор объекта   
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : AVP_dword *pdwErrorCode	- указатель на код ошибки. Может быть 0.
AVP_dword   DSKMAPI DSKM_ParList_GetFirstObjectError( HDSKMLIST hList, AVP_dword *pdwErrorCode );


//! \fn				:  DSKMAPI DSKM_ParList_GetNextObjectError
//! \brief			:	Получить следующий объект с ошибкой обработки
//! \return			: Идентификатор объекта или 0, если объектов больше нет.  
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : AVP_dword dwObjectId - идентификатор предыдущего объекта. Не может быть 0.
//! \param          : AVP_dword *pdwErrorCode	- указатель на код ошибки. Может быть 0.
AVP_dword   DSKMAPI DSKM_ParList_GetNextObjectError( HDSKMLIST hList, AVP_dword dwObjectId, AVP_dword *pdwErrorCode );


//---------------------------------------------------------------------------------------------------------------------------
// Функции получения информации об объекте
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				:  DSKMAPI DSKM_ParList_GetObject
//! \brief			:	Выдать информацию об объекте
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : AVP_dword dwObjectId - идентификатор объекта
//! \param          : void *pvObject	- указатель на буфер результата 
//! \param          : AVP_dword *pdwObjectSize - указатель на размер буфера результата
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_GetObject( HDSKMLIST hList, AVP_dword dwObjectId, void *pvObject, AVP_dword *pdwObjectSize );


//! \fn				:  DSKMAPI DSKM_ParList_GetObjectId
//! \brief			:	Выдать идентификатор объекта
//! \return			: Идентификатор объекта списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hListObject - объект операции
AVP_dword   DSKMAPI DSKM_ParList_GetObjectId( HDSKMLIST hList, HDSKMLISTOBJ hListObject );


//---------------------------------------------------------------------------------------------------------------------------
// Функции работы со свойствами объекта
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_ParList_SetObjectProp
//! \brief			:	Установить свойство объекта операции (DSKM_OBJ_*)
//! \return			: Код ошибки
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : AVP_dword dwPropId - идертификатор свойства
//! \param          : void *pvData - буфер с данными
//! \param          : AVP_dword dwDataSize - размер буфера
AVP_dword 	DSKMAPI DSKM_ParList_SetObjectProp( HDSKMLIST hList, HDSKMLISTOBJ hListObject, AVP_dword dwPropId, void *pvData, AVP_dword dwDataSize );



//! \fn				: DSKM_ParList_GetObjectProp
//! \brief			:	Получить свойство объекта операции (DSKM_OBJ_*)
//! \return			: Код ошибки
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : AVP_dword dwPropId - идертификатор свойства
//! \param          : void *pvData	- указатель на буфер результата
//! \param          : AVP_dword *pdwDataSize - указатель на размер буфера результата
AVP_dword 	DSKMAPI DSKM_ParList_GetObjectProp( HDSKMLIST hList, HDSKMLISTOBJ hListObject, AVP_dword dwPropId, void *ppvData, AVP_dword *pdwDataSize );


//! \fn				: DSKM_ParList_SetObjectProp
//! \brief			:	Установить свойство объекта, подлежащее замешиванию в хэш объекта
//!               Таких свойств может быть установлено любое количество. 
//!               Замешивание выполняется последовательно (в порядке добавления)
//!               ВАЖНО! Порядок следования свойств при подписывании и проверке должен совпадать!
//! \return			: Код ошибки
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hListObject - объект операции
//! \param          : void *pvData - буфер с данными
//! \param          : AVP_dword dwDataSize - размер буфера
AVP_dword 	DSKMAPI DSKM_ParList_SetObjectHashingProp( HDSKMLIST hList, HDSKMLISTOBJ hListObject, void *pvData, AVP_dword dwDataSize );


//---------------------------------------------------------------------------------------------------------------------------
// Функции перебора объектов
//---------------------------------------------------------------------------------------------------------------------------


//! \fn				: DSKM_ParList_GetFirstObject
//! \brief			:	Получить первый объект списка
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_GetFirstObject( HDSKMLIST hList );


//! \fn				: DSKM_ParList_GetNextObject
//! \brief			:	Получить следующий объект списка
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hCurrObject - текущий объект
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_GetNextObject( HDSKMLIST hList, HDSKMLISTOBJ hCurrObject );


//---------------------------------------------------------------------------------------------------------------------------
// Функции перебора параметров листа
//---------------------------------------------------------------------------------------------------------------------------


//! \fn				: DSKM_ParList_GetFirstParam
//! \brief			:	Получить первый параметр списка/объекта
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          :  HDSKMLISTOBJ hListObject - объект операции, параметры которого нужно перебрать
//                     Если 0 - перебираются параметры списка
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_GetFirstParam( HDSKMLIST hList, HDSKMLISTOBJ hListObject );


//! \fn				: DSKM_ParList_GetNextParam
//! \brief			:	Получить следующий параметр списка/объекта
//! \return			: Объект списка (0 в случе неуспеха). Код ошибки получается через DSKM_ParList_GetLastError
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : HDSKMLISTOBJ hCurrParam - текущий параметр
HDSKMLISTOBJ 	DSKMAPI DSKM_ParList_GetNextParam( HDSKMLIST hList, HDSKMLISTOBJ hCurrParam );


//---------------------------------------------------------------------------------------------------------------------------
// Функции работы с ошибками списка
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_ParList_GetLastError
//! \brief			:	Выдать код последней ошибки на списке
//! \return			: Код ошибки
//! \param          :  HDSKMLIST hList - список объектов операции
AVP_dword DSKMAPI DSKM_ParList_GetLastError( HDSKMLIST hList );


//! \fn				: DSKM_ParList_SetLastError
//! \brief			:	Установить код последней ошибки на списке
//! \return			: void DSKMAPI 
//! \param          :  HDSKMLIST hList - список объектов операции
//! \param          : AVP_dword dwResult - код ошибки
void DSKMAPI DSKM_ParList_SetLastError( HDSKMLIST hList, AVP_dword dwResult );



//---------------------------------------------------------------------------------------------------------------------------
// Функции генерации ключевых пар и записи реестров ключей
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				:  DSKMAPI DSKM_GenerateKeyPair
//! \brief			:	Генерировать ключевую пару - закрытый и открытый ключ
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  void **ppPrivateKey - указатель на результирующий буфер закрытого ключа (распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwPrivateKeySize - указатель на результирующий размер буфера закрытого ключа
//! \param          : void **ppvPublicKey - указатель на результирующий буфер открытого ключа (распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwPublicKeySize - указатель на результирующий размер буфера открытого ключа
AVP_dword   DSKMAPI DSKM_GenerateKeyPair( HDSKM hDSKM, void **ppPrivateKey, AVP_dword *pdwPrivateKeySize, void **ppvPublicKey, AVP_dword *pdwPublicKeySize );


//! \fn				:  DSKMAPI DSKM_SaveKeyToKeyReg
//! \brief			:	Записать ключ в реестр ключей (реестр не подписывается). 
//!       				Для подписи реестра вызывается DSKM_SignOneObjectTo*
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список параметров объектов, для которых предназначен ключ
//! \param          : AVP_dword dwObjectsType	- тип объектов, для которых предназначен ключ
//!                   (может быть = 0 - тогда должен быть задан файл ассоциаций)
//! \param          : AVP_dword dwKeyType - тип ключа (DSKM_OTYPE_KEY_*)
//! \param          : void *pvKey - указатель на буфер ключа
//! \param          : AVP_dword dwKeySize	- размер буфера ключа
//! \param          : const AVP_char* pszKeyRegFileName - имя файла реестра
AVP_dword   DSKMAPI DSKM_SaveKeyToKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_dword dwKeyType, void *pvKey, AVP_dword dwKeySize, const AVP_char* pszKeyRegFileName );



//! \fn				:  DSKMAPI DSKM_RemoveKeyFromKeyReg
//! \brief			:	Удалить ключ из реестра ключей (реестр не подписывается). 
//!       				Для подписи реестра нужно вызвать DSKM_SignOneObjectTo*
//!               Для поиска ключа в реестре используются параметры объектов из списка параметров
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список параметров объектов, для которых предназначен ключ
//! \param          : AVP_dword dwObjectsType	- тип объектов, для которых предназначен ключ
//!                   (может быть = 0 - тогда должен быть задан файл ассоциаций)
//! \param          : AVP_dword dwKeyType - тип ключа (DSKM_OTYPE_KEY_*)
//! \param          : const AVP_char* pszKeyRegFileName - имя файла реестра
AVP_dword   DSKMAPI DSKM_RemoveKeyFromKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_dword dwKeyType, const AVP_char* pszKeyRegFileName );


//! \fn				:  DSKMAPI DSKM_GetKeyFromKeyReg
//! \brief			:	Получить ключи из реестра ключей. 
//! \return			: Код ошибки
//! \param          : HDSKM hDSKM - контекст библиотеки. 
//! \param          : HDSKMLIST hObjectsList - список параметров объектов, для которых предназначен ключ
//! \param          : AVP_dword dwObjectsType	- тип объектов, для которых предназначен ключ
//!                   (может быть = 0 - тогда должен быть задан файл ассоциаций)
//! \param          : AVP_dword dwKeyType - тип ключа (DSKM_OTYPE_KEY_*)
//! \param          : void **ppvKey - указатель на результирующий буфер ключа (распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwKeySize - указатель на результирующий размер буфера ключа
//! \param          : AVP_dword *pdwKeyCount - указатель на количество ключей
//! \param          : const AVP_char* pszKeyRegFileName - имя файла реестра
AVP_dword   DSKMAPI DSKM_GetKeyFromKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_dword dwKeyType, void **ppvKey, AVP_dword *pdwKeySize, AVP_dword *pdwKeyCount, const AVP_char* pszKeyRegFileName );

//---------------------------------------------------------------------------------------------------------------------------
// Функции вычисления хэш-значений 
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_HashObjectByName
//! \brief			:	Вычислить хэш файла
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  const AVP_char* pszObjectFileName - имя файла для подсчета
//! \param          : void **ppvObjectHash - результирующий хэш	(распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwObjectHashSize - указатель на результирующий размер хэша
AVP_dword	DSKMAPI DSKM_HashObjectByName( HDSKM hDSKM, const AVP_char* pszObjectFileName, void **ppvObjectHash, AVP_dword *pdwObjectHashSize );

//! \fn				: DSKM_HashObjectByBuffer
//! \brief			:	Вычислить хэш буферизованного объекта
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  void *pvBuffer - указатель на буфер
//! \param          : AVP_dword dwBufferSize - размер буфера
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack - функция подкачки буфера
//                    Может быть 0 - тогда считается, что буфер уже заполнен
//! \param          : void *pvCallBackParams	- параметры функции подкачки
//! \param          : void **ppvObjectHash - результирующий хэш	(распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwObjectHashSize - указатель на результирующий размер хэша
AVP_dword	DSKMAPI DSKM_HashObjectByBuffer( HDSKM hDSKM, void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams, void **ppvObjectHash, AVP_dword *pdwObjectHashSize );


//! \fn				: DSKM_HashObjectByNameUsingGivenHashBuffer
//! \brief			:	Вычислить хэш файла
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  const AVP_char* pszObjectFileName - имя файла для подсчета
//! \param          : void *pvObjectHash - буфер под результирующий хэш
//! \param          : AVP_dword *pdwObjectHashSize - указатель на результирующий размер хэша
AVP_dword	DSKMAPI DSKM_HashObjectByNameUsingGivenHashBuffer( HDSKM hDSKM, const AVP_char* pszObjectFileName, void *pvObjectHash, AVP_dword *pdwObjectHashSize );

//! \fn				: DSKM_HashObjectByBufferGivenHashBuffer
//! \brief			:	Вычислить хэш буферизованного объекта
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  void *pvBuffer - указатель на буфер
//! \param          : AVP_dword dwBufferSize - размер буфера
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack - функция подкачки буфера
//                    Может быть 0 - тогда считается, что буфер уже заполнен
//! \param          : void *pvCallBackParams	- параметры функции подкачки
//! \param          : void *ppvObjectHash - буфер под результирующий хэш
//! \param          : AVP_dword *pdwObjectHashSize - указатель на результирующий размер хэша
AVP_dword	DSKMAPI DSKM_HashObjectByBufferGivenHashBuffer( HDSKM hDSKM, void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams, void *pvObjectHash, AVP_dword *pdwObjectHashSize );


//! \fn				: DSKM_HashObjectByList
//! \brief			:	Вычислить хэш объекта из списка (первого!)
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : void **ppvObjectHash - результирующий хэш	(распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwObjectHashSize - указатель на результирующий размер хэша
AVP_dword	DSKMAPI DSKM_HashObjectByList( HDSKM hDSKM, HDSKMLIST hObjectsList, void **ppvObjectHash, AVP_dword *pdwObjectHashSize );


//! \fn				: DSKM_HashObjectsToRegBuffer
//! \brief			:	Вычислить хэши объектов и собрать в реестр.
//!               Реестр сериализуется, но не подписывается.
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : AVP_dword dwObjectsType	- тип объектов (пользовательский 1-32758)
//! \param          : AVP_bool bSpecialReg - признак специального реестра
//! \param          : void **ppvHashRegBuffer - результирующий сериализованный реестр хэшей (распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwHashRegBufferSize	- указатель на результирующий размер реестра хэшей
AVP_dword	DSKMAPI DSKM_HashObjectsToRegBuffer( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, void **ppvHashRegBuffer, AVP_dword *pdwHashRegBufferSize );

//---------------------------------------------------------------------------------------------------------------------------
// Функции подписывания группы объектов
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				:  DSKMAPI DSKM_SignObjectsToRegFileByKey
//! \brief			:	Создать файла реестра хэшей объектов, используя для подписи переданный закрытый ключ
//! \return			: Код ошибки  
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : AVP_dword dwObjectsType	- тип объектов (пользовательский 1-32758)
//! \param          : AVP_bool bSpecialReg - признак специального реестра
//! \param          : void *pvPrivateKey	- закрытый ключ для подписывания реестра
//! \param          : AVP_dword dwKeySize - размер закрытого ключа
//! \param          : const AVP_char* pszObjsRegFileName - имя выходного файла реестра хэшей
AVP_dword   DSKMAPI DSKM_SignObjectsToRegFileByKey( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, void *pvPrivateKey, AVP_dword dwKeySize, const AVP_char* pszObjsRegFileName );


//! \fn				:  DSKMAPI DSKM_SignObjectsToRegBufferByKey
//! \brief			:	Создать буфер реестра хэшей объектов, используя для подписи переданный закрытый ключ
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : AVP_dword dwObjectsType	- тип объектов (пользовательский 1-32758)
//! \param          : AVP_bool bSpecialReg - признак специального реестра
//! \param          : void *pvPrivateKey	- закрытый ключ для подписывания реестра
//! \param          : AVP_dword dwKeySize - размер закрытого ключа
//! \param          : void **ppvRegBuffer - результирующий буфер	(распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwRegBufferSize - указатель на результирующий размер буфера
AVP_dword   DSKMAPI DSKM_SignObjectsToRegBufferByKey( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, void *pvPrivateKey, AVP_dword dwKeySize, void **ppvRegBuffer, AVP_dword *pdwRegBufferSize );


//! \fn				:  DSKMAPI DSKM_SignObjectsToRegFileByKeyReg
//! \brief			:	Создать буфер реестра хэшей объектов, используя для подписи реестр закрытых ключей
//!               Для поиска ключа в реестре используются параметры объектов из списка параметров
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : AVP_dword dwObjectsType	- тип объектов (пользовательский 1-32758)
//! \param          : AVP_bool bSpecialReg - признак специального реестра
//! \param          : const AVP_char* pszKeyRegFileName - имя файла реестра закрытых ключей
//! \param          : const AVP_char* pszObjsRegFileName - имя выходного файла реестра хэшей
AVP_dword   DSKMAPI DSKM_SignObjectsToRegFileByKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, const AVP_char* pszKeyRegFileName, const AVP_char* pszObjsRegFileName );


//! \fn				:  DSKMAPI DSKM_SignObjectsToRegBufferByKeyReg
//! \brief			:	Создать буфер реестра хэшей объектов, используя для подписи реестр закрытых ключей
//!               Для поиска ключа в реестре используются параметры объектов из списка параметров
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : AVP_dword dwObjectsType	- тип объектов (пользовательский 1-32758)
//! \param          : AVP_bool bSpecialReg - признак специального реестра
//! \param          : const AVP_char* pszKeyRegFileName - имя файла реестра закрытых ключей
//! \param          : void **ppvRegBuffer - результирующий буфер (распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwRegBufferSize - указатель на результирующий размер буфера
AVP_dword   DSKMAPI DSKM_SignObjectsToRegBufferByKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, AVP_bool bSpecialReg, const AVP_char* pszKeyRegFileName, void **ppvRegBuffer, AVP_dword *pdwRegBufferSize );


//---------------------------------------------------------------------------------------------------------------------------
// Функции подписывания одного объекта
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				:  DSKMAPI DSKM_SignOneObjectToObjectByKey
//! \brief			:	Подписать один объект (первый в списке файл), поместив подпись непосредственно в файл 
//!               Для подписывания используется переданный закрытый ключ 
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : AVP_dword dwObjectsType - тип объекта (DSKM_OTYPE_KEY_* или пользовательский)
//! \param          : void *pvPrivateKey	- закрытый ключ для подписывания реестра
//! \param          : AVP_dword dwKeySize - размер закрытого ключа
AVP_dword   DSKMAPI DSKM_SignOneObjectToObjectByKey( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, void *pvPrivateKey, AVP_dword dwKeySize );


//! \fn				:  DSKMAPI DSKM_SignOneObjectToObjectByKeyReg
//! \brief			:	Подписать один объект (первый в списке файл), поместив подпись непосредственно в файл 
//!               Для подписывания используется ключ, найденный в реестре закрытых ключей 
//!               Для поиска ключа в реестре используются параметры объектов из списка параметров
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : AVP_dword dwObjectsType - тип объекта (DSKM_OTYPE_KEY_* или пользовательский)
//! \param          : const AVP_char* pszKeyRegFileName	- имя файла реестра закрытых ключей
AVP_dword   DSKMAPI DSKM_SignOneObjectToObjectByKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pszKeyRegFileName );


//! \fn				:  DSKMAPI DSKM_SignOneObjectToSignBufferByKey
//! \brief			:	Подписать один объект (первый в списке), поместив подпись в выходной буфер 
//!               Для подписывания используется переданный закрытый ключ 
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : AVP_dword dwObjectsType - тип объекта (DSKM_OTYPE_KEY_* или пользовательский)
//! \param          : void *pvPrivateKey	- закрытый ключ для подписывания реестра
//! \param          : AVP_dword dwKeySize - размер закрытого ключа 
//! \param          : void **ppvSignBuffer - результирующий буфер с подписью объекта (распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwSignBufferSize - указатель на результирующий размер буфера с подписью
AVP_dword   DSKMAPI DSKM_SignOneObjectToSignBufferByKey( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, void *pvPrivateKey, AVP_dword dwKeySize, void **ppvSignBuffer, AVP_dword *pdwSignBufferSize );


//! \fn				:  DSKMAPI DSKM_SignOneObjectToObjectByKeyReg
//! \brief			:	Подписать один объект (первый в списке), поместив подпись в выходной буфер 
//!               Для подписывания используется ключ, найденный в реестре закрытых ключей 
//!               Для поиска ключа в реестре используются параметры объектов из списка параметров
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : AVP_dword dwObjectsType - тип объекта (DSKM_OTYPE_KEY_* или пользовательский)
//! \param          : const AVP_char* pszKeyRegFileName	- имя файла реестра закрытых ключей
//! \param          : void **ppvSignBuffer - результирующий буфер с подписью объекта (распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwSignBufferSize - результирующий размер буфера с подписью
AVP_dword   DSKMAPI DSKM_SignOneObjectToSignBufferByKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pszKeyRegFileName, void **ppvSignBuffer, AVP_dword *pdwSignBufferSize );



//! \fn				:  DSKMAPI DSKM_SignOneObjectToObjectBySignBuffer
//! \brief			:	Подписать один объект (первый в списке), используя переданную подпись.
//!               Соответствие подписи и объекта НЕ контролируется
//!               Если объект является файлом, то подпись к нему дописывается
//!               Если объект является буфером, то выдается новый буфер, как "старый+подпись"
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
//! \param          : void *pvSignBuffer - буфер с подписью объекта 
//! \param          : AVP_dword dwSignBufferSize - размер буфера с подписью
//! \param          : void **ppvSignedBuffer - подписанный буфер (распределяется через pfnAlloc)
//! \param          : AVP_dword *pdwSignedBufferSize - размер подписанного буфера
AVP_dword   DSKMAPI DSKM_SignOneObjectToObjectBySignBuffer( HDSKM hDSKM, HDSKMLIST hObjectsList, void *pvSignBuffer, AVP_dword dwSignBufferSize, void **ppvSignedBuffer, AVP_dword *pdwSignedBufferSize );


//---------------------------------------------------------------------------------------------------------------------------
// Функции сравнения объектов, исключая подпись внутри объекта
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				:  DSKMAPI DSKM_CompareObjectsIgnoreSign
//! \brief			:	Сравнить вместе ВСЕ объекты из списка
//                Cравниваются все объеты вместе, путем расчета и сравнения хэшей
//! \return			: Код ошибки - DSKM_ERR_OBJECTS_NOT_EQUAL, если объекты не совпадают  
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов
AVP_dword   DSKMAPI DSKM_CompareObjectsIgnoreSign( HDSKM hDSKM, HDSKMLIST hObjectsList );


//---------------------------------------------------------------------------------------------------------------------------
// Функции проверки объектов через реестры хэш-значений
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_CheckObjectsUsingHashRegsFolder
//! \brief			:	Проверить объекты через реестры хэшей, размещенные в заданной папке
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов для проверки и параметров
//! \param          : AVP_dword dwObjectsType	- тип объектов
//! \param          : const AVP_char* pszRegFilesFolder	- папка размещения реестров
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingHashRegsFolder( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pszRegFilesFolder );


//! \fn				: DSKM_CheckObjectsUsingHashRegFile
//! \brief			:	Проверить объекты через переданный реестр хэшей
//                Подпись реестра должна быть уже проверена (через DSKM_CheckObjectsUsingInsideSign*)
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов для проверки и параметров
//! \param          : AVP_dword dwObjectsType	- тип объектов
//! \param          : const AVP_char* pszRegFileName	- файл реестра
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingHashRegFile( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pszRegFileName );


//! \fn				: DSKM_CheckObjectsUsingHashRegsAssoc
//! \brief			:	Проверить объекты через реестры хэшей, заданные через ассоциации
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов для проверки, параметров и ассоциаций
//! \param          : AVP_dword dwObjectsType	- тип объектов
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingHashRegsAssoc( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType );


//! \fn				: DSKM_CheckObjectsUsingBufferedHashReg
//! \brief			:	Проверить объекты, используя буферизованный реестр.
//                Подпись реестра должна быть уже проверена (через DSKM_CheckObjectsUsingInsideSign*)
//! \return			: Код ошибки
//! \param          : HDSKM hDSKM - контекст библиотеки. 
//! \param          : HDSKMLIST hObjectsList - список объектов для проверки и параметров
//! \param          : AVP_dword dwObjectsType	- тип объектов
//! \param          : void *pvBuffer - указатель на буфер
//! \param          : AVP_dword dwBufferSize - размер буфера
//! \param          : pfnDSKM_GetBuffer_CallBack pfnCallBack - функция подкачки буфера
//                    Может быть 0 - тогда считается, что буфер уже заполнен
//! \param          : void *pvCallBackParams	- параметры функции подкачки
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingBufferedHashReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, void *pvBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams );


//---------------------------------------------------------------------------------------------------------------------------
// Функции проверки объектов через заданный набор реестров
//---------------------------------------------------------------------------------------------------------------------------


//! \fn				: DSKM_PrepareRegsSet
//! \brief			:	Проверить набор реестров (задаются через функции DSKM_ParList_Add * Reg)
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список реестров для проверки, параметров и ассоциаций
//! \param          : AVP_dword dwObjectsType	- тип объектов
AVP_dword  	DSKMAPI DSKM_PrepareRegsSet( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType );


//! \fn				: DSKM_CheckObjectsUsingRegsSet
//! \brief			:	Проверить объекты, используя подготовленный набор реестров (DSKM_PrepareRegsSet)
//! \return			: Код ошибки 
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов для проверки, параметров и ассоциаций
//! \param          : AVP_dword dwObjectsType	- тип объектов
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingRegsSet( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType );


//! \fn				:  DSKMAPI DSKM_IsRegsSetPrepared
//! \brief			:	Проверить - подготовлен ли набор реестров к проверке
//! \return			: Код ошибки 
//! \param          : HDSKM hDSKM - контекст библиотеки. 
//! \param          : HDSKMLIST hObjectsList - список объектов для проверки, параметров и ассоциаций
//! \param          : AVP_dword dwObjectsType	- тип объектов
//!										Если dwObjectsType == 0 - тип определяется через ассоциации (если заданы)
//!										Если dwObjectsType != 0 - ищется подготовленный реестр для данного типа 
//!										Если dwObjectsType == 0 - ищется первый подготовленный реестр любого типа 
AVP_dword   DSKMAPI DSKM_IsRegsSetPrepared( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType );

//---------------------------------------------------------------------------------------------------------------------------
// Функции проверки объектов через подпись в самом объекте
//---------------------------------------------------------------------------------------------------------------------------

//! \fn				: DSKM_CheckObjectsUsingInsideSignAndKeysFolder
//! \brief			:	Проверить объекты с подписью в самих себе, используя для поиска ключей переданную папку
//! \return			: Код ошибки
//! \param          : HDSKM hDSKM - контекст библиотеки. 
//! \param          : HDSKMLIST hObjectsList - список объектов для проверки и параметров
//! \param          : AVP_dword dwObjectsType	- тип объектов
//! \param          : const AVP_char* pszRegFilesFolder	- папка размещения реестров
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingInsideSignAndKeysFolder( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pszRegFilesFolder );



//! \fn				: DSKM_CheckObjectsUsingInsideSignAndKeysAssoc
//! \brief			:	Проверить объекты с подписью в самих себе, используя для поиска ключей заданные ассоциации
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов для проверки, параметров и ассоциаций
//! \param          : AVP_dword dwObjectsType	- тип объектов
//! \param          : const AVP_char* pszRegFilesFolder	- папка размещения реестров
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingInsideSignAndKeysAssoc( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType );



//! \fn				: DSKM_CheckObjectsUsingInsideSignAndKeyReg
//! \brief			:	Проверить объекты с подписью в самих себе, используя переданный файл реестра ключей.
//! \return			: Код ошибки
//! \param          :  HDSKM hDSKM - контекст библиотеки. 
//! \param          :  HDSKMLIST hObjectsList - список объектов для проверки и параметров
//! \param          : AVP_dword dwObjectsType	- тип объектов
//! \param          : const AVP_char* pszKeyRegFileName	- файл реестра ключей
AVP_dword  	DSKMAPI DSKM_CheckObjectsUsingInsideSignAndKeyReg( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pszKeyRegFileName );


//---------------------------------------------------------------------------------------------------------------------------
// Вспомогательные функции
//---------------------------------------------------------------------------------------------------------------------------


//! \fn				: DSKM_SaveTypeAssociationToFile
//! \brief			:	Сохранить ассоциацию "object/key params => objects type" в файл
//! \return			: DSKMAPI 
//! \return			: Код ошибки
//! \param          : HDSKM hDSKM - контекст библиотеки. 
//! \param          : HDSKMLIST hObjectsList - список параметров
//! \param          : AVP_dword dwObjectsType	- тип объектов
//! \param          : const AVP_char* pszAssocFileName - имя файла ассоциаций (файл не подписывается)
AVP_dword  	DSKMAPI DSKM_SaveTypeAssociationToFile(  HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pszAssocFileName );



//! \fn				:  DSKMAPI DSKM_DumpReg
//! \brief			:	Вывести реестр в текстовом виде.
//! \return			: Код ошибки
//! \param          : HDSKM hDSKM - контекст библиотеки. 
//! \param          : const AVP_char* pszRegFileName - имя файла реестра
//! \param          : const AVP_char* pszOutFileName - имя выходного файла
AVP_dword   DSKMAPI DSKM_DumpReg( HDSKM hDSKM, const AVP_char* pszRegFileName, const AVP_char* pszOutFileName );



//! \fn				: DSKM_RandomFillBuffer
//! \brief			:	Заполнить буфер случайными числами
//! \return			: Код ошибки
//! \param          : HDSKM hDSKM - контекст библиотеки. 
//! \param          : void *pBuffer - буфер для заполнения
//! \param          : AVP_dword dwBufferSize - размер буфера
AVP_dword	DSKMAPI DSKM_RandomFillBuffer( HDSKM hDSKM, void *pBuffer, AVP_dword dwBufferSize );



//! \fn				: * DSKM_Alloc
//! \brief			:	Рапределить память, используя систему распределения памяти переданную при инициировании
//! \return			: Указатель на рапределенный буфер
//! \param          : AVP_int iSize - размер
void * DSKMAPI DSKM_Alloc( AVP_int iSize );


//! \fn				: DSKM_Free
//! \brief			:	Удалить память, используя систему распределения памяти переданную при инициировании 
//! \return			: void
//! \param          : void *pvBuffer - буфер для удаления
void DSKMAPI DSKM_Free( void *pvBuffer );

//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
#if defined( __cplusplus )
}
#endif


#endif //__dskm_h__
