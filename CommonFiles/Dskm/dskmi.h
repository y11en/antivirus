#ifndef __dskmi_h__
#define __dskmi_h__

#include "dskm.h"
#include "dskmdefs.h"
#include "../byteorder.h"
#include "../Property/Property.h"
#include "../AVPPort.h"

/*
#if !defined (__unix__)
#include <crtdbg.h>
#endif
*/



//#define DSKM_EXCEPTION_ENABLE

#if defined( _WIN32 ) && defined( DSKM_EXCEPTION_ENABLE )
#include <excpt.h>
#define DSKM_TRY						__try
#define DSKM_EXCEPT					__except( DSKM_ProcessException(GetExceptionCode()) ) { return DSKM_UNKNOWN_ERROR; }
#define DSKM_EXCEPT_SIZE		__except( DSKM_ProcessException(GetExceptionCode()) ) { return 0; }
#define DSKM_EXCEPT_VOID		__except( DSKM_ProcessException(GetExceptionCode()) ) {}

#if defined( __cplusplus )
extern "C" 
#endif
unsigned long DSKM_ProcessException( unsigned long code );

#else
#define DSKM_TRY      
#define DSKM_EXCEPT   
#define DSKM_EXCEPT_SIZE		
#define DSKM_EXCEPT_VOID	
#endif


#define DSKM_AID									0x15	
#define DSKM_ROOT_PID             MAKE_AVP_PID( 0, DSKM_AID, avpt_nothing, 0 )

#define DSKM_KID									0x16	
#define DSKM_KEY_PID							MAKE_AVP_PID( 0, DSKM_KID, avpt_nothing, 0 )

#define DSKM_KL_KID								0x17	
#define DSKM_KL_KEY_PID						MAKE_AVP_PID( 0, DSKM_KL_KID, avpt_nothing, 0 )

#define DSKM_OP_KID								0x18	
#define DSKM_OP_KEY_HEAD_PID			MAKE_AVP_PID( 0, DSKM_OP_KID, avpt_nothing, 0 )
#define DSKM_OP_KEY_PID(a)				MAKE_AVP_PID( a, DSKM_OP_KID, avpt_nothing, 0 )

#define DSKM_HS_HID								0x19	
#define DSKM_HS_KEY_PID(a)				MAKE_AVP_PID( a, DSKM_HS_HID, avpt_nothing, 0 )

#define DSKM_AS_AID								0x1a	
#define DSKM_AS_HEAD_PID			    MAKE_AVP_PID( 0, DSKM_AS_AID, avpt_nothing, 0 )
#define DSKM_AS_PID(a)			      MAKE_AVP_PID( a, DSKM_AS_AID, avpt_nothing, 0 )

#define DSKM_APP_DWORD_PARAM			(1)
#define DSKM_APP_STR_PARAM				(2)
#define DSKM_APP_BPTR_PARAM				(3)
#define DSKM_APP_ROOT_PARAM				(9)

#define DSKM_APP_OBJECT_NAME_PARAM  (10)
#define DSKM_APP_OBJECT_BUFF_PARAM  (11)
#define DSKM_APP_OBJECT_HASH_PARAM  (12)
#define DSKM_APP_OBJECT_ASSOC_PARAM (13)
#define DSKM_APP_OBJECT_ASSOC_FILE  (14)
#define DSKM_APP_OBJECT_ASSOC_BUFF  (15)

#define DSKM_APP_REG_PARAM					(20)
#define DSKM_APP_KEY								(30)
#define DSKM_APP_ERR								(40)
#define DSKM_APP_FUNC_TABLE					(50)
#define DSKM_APP_CL_CONT						(60)
#define DSKM_APP_IO								  (70)
#define DSKM_APP_LISTOBJ						(75)

#define DSKM_APP_REG_NAME_PARAM			(80)
#define DSKM_APP_REG_BUFF_PARAM			(81)

#define DSKM_APP_OBJECT_PROP				(90)
#define DSKM_APP_OBJECT_HASHED_PROP	(100)

#define DSKM_APP_CHECKED_OBJ				(110)

#define DSKM_APP_TYPES_PROP					(120)

// Идентификаторы параметров ключа
#define DSKM_DWORD_PARAM_ID(a)				MAKE_AVP_PID(a, DSKM_APP_DWORD_PARAM, avpt_dword, 0)
#define DSKM_STR_PARAM_ID(a)					MAKE_AVP_PID(a, DSKM_APP_STR_PARAM, avpt_str, 0)
#define DSKM_BPTR_PARAM_ID(a)					MAKE_AVP_PID(a, DSKM_APP_BPTR_PARAM, avpt_bin, 0)
#define DSKM_BIN_PARAM_ID(a)					MAKE_AVP_PID(a, DSKM_APP_BPTR_PARAM, avpt_bin, 0)
#define DSKM_PARAM_ROOT_ID						MAKE_AVP_PID(0, DSKM_APP_ROOT_PARAM, avpt_nothing, 0 )

// Идентификаторы файловых ассоциаций
#define DSKM_ASSOC_OTYPE_ID(a)				MAKE_AVP_PID(a, DSKM_APP_OBJECT_ASSOC_PARAM, avpt_dword, 0)
#define DSKM_ASSOC_SIGN_FILE_ID				MAKE_AVP_PID(1, DSKM_APP_OBJECT_ASSOC_PARAM, avpt_bin, 0)
#define DSKM_ASSOC_KEY_FILE_ID				MAKE_AVP_PID(2, DSKM_APP_OBJECT_ASSOC_PARAM, avpt_bin, 0)

// Идентификаторы параметрический ассоциаций (параметры - тип объекта)
#define DSKM_ASSOC_FILE_ID(a)			    MAKE_AVP_PID(a, DSKM_APP_OBJECT_ASSOC_FILE, avpt_bin, 0)
#define DSKM_ASSOC_BUFF_PTR_ID(a)		  MAKE_AVP_PID(a, DSKM_APP_OBJECT_ASSOC_BUFF, avpt_bin, 0)

// Идентификаторы объектов операции
#define DSKM_OBJECT_NAME_ID(a)				MAKE_AVP_PID(a, DSKM_APP_OBJECT_NAME_PARAM, avpt_bin, 0)
#define DSKM_OBJECT_BUFF_PTR_ID(a)		MAKE_AVP_PID(a, DSKM_APP_OBJECT_BUFF_PARAM, avpt_bin, 0)
#define DSKM_OBJECT_BUFF_SIZE_ID			MAKE_AVP_PID(1, DSKM_APP_OBJECT_BUFF_PARAM, avpt_dword, 0)
#define DSKM_OBJECT_BUFF_PFN_ID				MAKE_AVP_PID(2, DSKM_APP_OBJECT_BUFF_PARAM, avpt_bin, 0)
#define DSKM_OBJECT_BUFF_PFN_PARS_ID	MAKE_AVP_PID(3, DSKM_APP_OBJECT_BUFF_PARAM, avpt_bin, 0)
#define DSKM_OBJECT_HASH_ID(a)				MAKE_AVP_PID(a, DSKM_APP_OBJECT_HASH_PARAM, avpt_bin, 0)

// Идентификатор сохранения хэша проверенного реестра
#define DSKM_OBJECT_REG_HASH_ID(a)		MAKE_AVP_PID(a, DSKM_APP_REG_PARAM, avpt_bin, 0)

// Идентификатор сохранения хэша объекта
#define DSKM_OBJECT_REG_ARR_HASH_ID	  MAKE_AVP_PID(0, DSKM_APP_REG_PARAM, avpt_bin, 1)

// Идентификатор сохранения хэша и подписи проверенного реестра
#define DSKM_CHECKED_OBJ_HASH_ID(a)		MAKE_AVP_PID(a, DSKM_APP_CHECKED_OBJ, avpt_bin, 0)
#define DSKM_CHECKED_OBJ_SIGN_ID(a)		MAKE_AVP_PID(a, DSKM_APP_CHECKED_OBJ, avpt_bin, 0)

// Идентификатор свойств ключа
#define DSKM_KEYREG_KEY_ID(a)					MAKE_AVP_PID(a, DSKM_APP_KEY,  avpt_bin, 0)
#define DSKM_KEYREG_OBJTYPE_ID				MAKE_AVP_PID(1, DSKM_APP_KEY,  avpt_dword, 0)
#define DSKM_KEYREG_ISSPEC_ID				  MAKE_AVP_PID(2, DSKM_APP_KEY,  avpt_nothing, 0)

// Идентификатор ошибки обработки объекта
#define DSKM_OBJECT_ERROR_ID				  MAKE_AVP_PID(1, DSKM_APP_ERR, avpt_dword, 0)
#define DSKM_OBJECT_ERROR_IDA(a)		  MAKE_AVP_PID(a, DSKM_APP_ERR, avpt_dword, 0)

// Идентификатор объекта входного списка
#if defined( _M_X64 )
#define DSKM_OBJECT_LISTOBJ_ID				MAKE_AVP_PID(1, DSKM_APP_LISTOBJ, avpt_qword, 0)
#else
#define DSKM_OBJECT_LISTOBJ_ID			  MAKE_AVP_PID(1, DSKM_APP_LISTOBJ, avpt_dword, 0)
#endif

// Идентификатор таблицы функций 
#if defined( _M_X64 )
#define DSKM_CLFUNC_TABLE_ID				  MAKE_AVP_PID(1, DSKM_APP_FUNC_TABLE, avpt_qword, 1)
#else
#define DSKM_CLFUNC_TABLE_ID				  MAKE_AVP_PID(1, DSKM_APP_FUNC_TABLE, avpt_dword, 1)
#endif

// Идентификатор контекстов CriptoLib
#define DSKM_CLCONT_R_ID							MAKE_AVP_PID(1, DSKM_APP_CL_CONT, avpt_bin, 0)
#define DSKM_CLCONT_C_ID							MAKE_AVP_PID(2, DSKM_APP_CL_CONT, avpt_bin, 0)

// Идентификатор объекта ввода/вывода
#define DSKM_IO_ID										MAKE_AVP_PID(1, DSKM_APP_IO, avpt_dword, 0)

// Идентификаторы реестров в памяти
#define DSKM_REG_NAME_ID(a)						MAKE_AVP_PID(a, DSKM_APP_REG_NAME_PARAM, avpt_bin, 0)
#define DSKM_REG_BUFF_PTR_ID(a)				MAKE_AVP_PID(a, DSKM_APP_REG_BUFF_PARAM, avpt_bin, 0)

// Идентификаторы внешних свойств объектов 
#define DSKM_OBJECT_PROP_ID(a)		    MAKE_AVP_PID(a, DSKM_APP_OBJECT_PROP, avpt_bin, 0)
#define DSKM_OBJECT_HASHED_PROP_ID(a) MAKE_AVP_PID(a, DSKM_APP_OBJECT_HASHED_PROP, avpt_bin, 0)

// Идентификатор массива типов объектов
#define DSKM_OBJ_TYPES_PROP_ID				MAKE_AVP_PID(0, DSKM_APP_TYPES_PROP, avpt_dword, 1)

#define DSKM_INT_ERR_EQUAL_HASH       0xf06901c5 /*is a crc32 for "DSKM_INT_ERR_EQUAL_HASH"*/

extern void* (* DSKMAllocator)(AVP_size_t);
extern void  (* DSKMLiberator)(void*);

extern void      *g_pDSKMData;
extern AVP_dword g_dwDSKMDataSize;

extern void      *g_pDSKMEData;
extern AVP_dword g_dwDSKMEDataSize;


#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
#define AVP_REG_MAGIC									(0x67724c4b)//('grLK')
#else
#define AVP_REG_MAGIC									('grLK')
#endif



#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
# if defined( __cplusplus )
#pragma pack(1)
# endif
# define PACKSTRUCT __attribute ((packed))
#else
#pragma pack(push,1)
# define PACKSTRUCT 
#endif


#define DSKM_REG_VERSION (1)

// ---		         
typedef struct {
	unsigned int		uiMagic;	
	union {
		unsigned int  uiObjType;
		struct {
			unsigned int uiVersion   : 8;		// 0x000000ff
			unsigned int uiBaseType  : 23;	// 0x7fffff00
			unsigned int uiIsSpecial : 1;		// 0x80000000
		};
	};
} PACKSTRUCT AVP_REG_Header;


#define RHD_VERSION(h)		(0x000000ff & (unsigned int)h)
#define RHD_BASETYPE(h)		((0x7fffff00 & (unsigned int)h) >>  8)
#define RHD_ISSPECIAL(h)	(!!(0x80000000 & (unsigned int)h))

#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__unix__)))
#pragma pack(pop)
#else
# if defined( __cplusplus )
#pragma pack()
# endif
#endif


typedef struct tagDSKMFILE {
	const AVP_char *m_pFileName; 
	HANDLE					m_hFile;
} DSKMFILE, *HDSKMFILE;


typedef struct tagDSKMCBP {
	DSKM_Io *m_pIo;
	HANDLE   m_hFile;
} DSKMCBP;

#define DSKMCreateFile(io,a,b,c,d,e,f,g)	io->lpVtbl->CreateFile(io,a,b,c,d,e,f,g)
#define DSKMCloseHandle(io,a)							io->lpVtbl->CloseHandle(io,a)
#define DSKMReadFile(io,a,b,c,d,e)				io->lpVtbl->ReadFile(io,a,b,c,d,e)
#define DSKMWriteFile(io,a,b,c,d,e)				io->lpVtbl->WriteFile(io,a,b,c,d,e)
#define DSKMSetFilePointer(io,a,b,c,d)		io->lpVtbl->SetFilePointer(io,a,b,c,d)
#define DSKMGetFileSize(io,a,b)						io->lpVtbl->GetFileSize(io,a,b)
#define DSKMFindFirstFile(io,a,b,c)				io->lpVtbl->FindFirstFile(io,a,b,c)
#define DSKMFindNextFile(io,a,b,c)				io->lpVtbl->FindNextFile(io,a,b,c)
#define DSKMFindClose(io,a)								io->lpVtbl->FindClose(io,a)


// ds_utils.c
AVP_dword DSKMAPI DSKM_FindUniquePID( HDATA hParentData, AVP_dword nSourcePID );
AVP_dword DSKMAPI DSKM_FindUniquePropPID( HDATA hParentData, AVP_dword nSourcePID );
AVP_char *DSKMAPI DSKM_GetPropValueAsString( HPROP hProp, AVP_dword *dwStrSize );
AVP_dword DSKMAPI DSKM_CheckRegFile( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDSKMFILE hRegFile, HDSKMFILE hKeyFile, void *pKeyData, AVP_dword dwKeyDataSize );
AVP_dword DSKMAPI DSKM_CheckRegObject( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDATA hRegObject, HDATA hKeyObject, void *pKeyData, AVP_dword dwKeyDataSize );
AVP_dword DSKMAPI DSKM_CheckObjectList( HDSKM hDSKM, HDSKMLIST hObjectsList, HDATA hKeysData );
HDATA DSKMAPI DSKM_CopyObjectsTree( HDATA hSrcData );
void  DSKMAPI DSKM_ProcessObjectsError( HDATA hObjectsList, HDATA hProcessedObjects, AVP_dword dwResult );
HDSKMFILE DSKMAPI DSKM_CreateHFile( HDSKM hDSKM, const AVP_char* pFileName );
void DSKMAPI DSKM_OpenHFileHandle( HDSKM hDSKM, HDSKMFILE hFile );
void	DSKMAPI DSKM_CloseHFile( HDSKM hDSKM, HDSKMFILE hFile );
void	DSKMAPI DSKM_ReuseHFile( HDSKM hDSKM, HDSKMFILE hFile, const AVP_char* pFileName );
DSKM_Io *DSKMAPI DSKM_GetIO( HDSKM hDSKM );
void DSKMAPI DSKM_SetIsSpecialProp( HDATA hRegData, AVP_bool bSpecialReg );
void DSKMAPI DSKM_SetObjTypeProp( HDATA hRegData, AVP_dword dwObjType );
AVP_dword DSKMAPI DSKM_IsObject( HDATA hObjData );
HDATA DSKMAPI DSKM_FindCorrespondingListObject( HDATA hObjectToFind );

// ds_io.c
AVP_bool DSKMAPI DSKM_LoadIO( HDSKM hDSKM );

// ds_ff.c
AVP_bool DSKMAPI DSKM_LoadFF( HDSKM hDSKM );

// ds_serial.c
AVP_dword DSKMAPI DSKM_SerializeRegToFile( HDSKM hDSKM, AVP_dword dwObjectsType, HDATA hRegData, const AVP_char* pRegFileName );
AVP_dword DSKMAPI DSKM_SerializeRegToBuffer( HDSKM hDSKM, AVP_dword dwObjectsType, HDATA hRegData, AVP_byte *pRegBuffer, AVP_dword dwRegBufferSize );

// ds_deser.c
AVP_dword DSKMAPI DSKM_ReadRegHeaderFromFile( HDSKM hDSKM, HANDLE hFile, AVP_dword dwObjectsType, AVP_REG_Header *pKLHeader );
AVP_dword DSKMAPI DSKM_DeserializeRegFile( HDSKM hDSKM, const AVP_char* pRegFileName, AVP_dword dwObjectsType, HDATA *hRootData );
AVP_dword DSKMAPI DSKM_DeserializeRegHFile( HDSKM hDSKM, HDSKMFILE hDSFile, AVP_dword dwObjectsType, HDATA *hRootData );
AVP_dword DSKMAPI DSKM_ReadRegHeaderFromMemory( AVP_char *pBuffer, AVP_dword *pdwBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams, AVP_dword dwObjectsType, AVP_REG_Header *pKLHeader, AVP_dword *pdwRead );
AVP_dword DSKMAPI DSKM_DeserializeRegBuffer( HDSKM hDSKM, void *pvRegBuffer, AVP_dword dwRegBufferSize, pfnDSKM_GetBuffer_CallBack pfnCallBack, void *pvCallBackParams, AVP_dword dwObjectsType, HDATA *hRootData );

// ds_hash.c
AVP_dword DSKMAPI DSKM_HashAdditionObjectParams( HDSKM hDSKM, HDATA hObjectData, void *pvObjectHash, AVP_dword dwObjectHashSize );
AVP_dword DSKMAPI DSKM_PrepareObjectHash( HDSKM hDSKM, HDATA hObjectData, void **ppvObjectHash, AVP_dword *pdwObjectHashSize );

// ds_fkey.c
AVP_bool DSKMAPI DSKM_FindParamList( HDATA hObjectsList, HDATA hCurrRegData );
HDATA DSKMAPI DSKM_FindKeyData( HDATA hParamList, AVP_dword dwObjectsType, HDATA hRegRootData, HDATA hKeysTree );
AVP_dword DSKMAPI DSKM_FindKeyInFile( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, const AVP_char* pKeyRegFileName, void **pKey, AVP_dword *dwKeySize );
AVP_dword DSKMAPI DSKM_FindKeyInBuffer( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, void *pvKeyRegBuffer, AVP_dword dwKeyRegBufferSize, void **pKey, AVP_dword *dwKeySize );
AVP_dword DSKMAPI DSKM_FindKeysTreeInBuffer( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, void *pvKeyRegBuffer, AVP_dword dwKeyRegBufferSize, HDATA hKeysTree );
AVP_dword DSKMAPI DSKM_FindKeysTreeInFile( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDSKMFILE hKeyFile, HDATA hKeysTree );
AVP_dword DSKMAPI DSKM_FindKeysTreeInObject( HDSKM hDSKM, HDSKMLIST hObjectsList, AVP_dword dwObjectsType, HDATA hKeyRegObject, HDATA hKeysTree );

// ds_chkfl.c
AVP_dword  	DSKMAPI DSKM_CheckObjectByInsideSign( HDSKM hDSKM, HDATA hObject, HDATA hKeysData );
AVP_dword  	DSKMAPI DSKM_CheckObjectsByInsideSign( HDSKM hDSKM, HDATA hObjects, HDATA hKeysData );
AVP_dword		DSKMAPI DSKM_CheckObjectByHashRegData( HDSKM hDSKM, HDATA hRegData, HDATA hObject );
AVP_dword  	DSKMAPI DSKM_CheckObjectsByHashRegData( HDSKM hDSKM, HDATA hObjects, HDATA hRegData );
AVP_dword  	DSKMAPI DSKM_CheckObjectsByHashRegFile( HDSKM hDSKM, HDATA hObjects, AVP_dword dwObjectsType, HDSKMFILE hRegFile, HDATA hTypesCollectionData );

// ds_gkey.c
AVP_dword   DSKMAPI DSKM_SaveParamsToTypeNode( HDSKMLIST hObjectsList, HDATA hTypeNode );

// ds_assch.c
AVP_dword  	DSKMAPI DSKM_CheckObjectTypeByAssoc( HDSKM hDSKM, HDSKMLIST hObjectsList, HDATA hParamList, AVP_dword dwObjectsType, HDATA hCollectionData );
AVP_dword  	DSKMAPI DSKM_PrepareTypeAssocByKeys( HDSKM hDSKM, HDSKMLIST hObjectsList, HDATA hKeysData );


// cl_sign.c
AVP_dword DSKMAPI DSKM_SignFile( HDSKM hDSKM, const AVP_char* pFileName, void *pPrivateKey, AVP_dword dwKeySize );
AVP_dword DSKMAPI DSKM_SignBuffer( HDSKM hDSKM, void **pRegBuffer, AVP_dword *dwRegBufferSize, void *pPrivateKey, AVP_dword dwKeySize );
AVP_dword DSKMAPI DSKM_SignHash( HDSKM hDSKM, void *pObjectHash, AVP_dword dwObjectHashSize, void *pPrivateKey, AVP_dword dwKeySize, void **ppSignBuffer, AVP_dword *pdwSignBufferSize );
AVP_dword DSKMAPI DSKM_SignFileBySign( HDSKM hDSKM, const AVP_char* pszFileName, void *pSign, AVP_dword dwSignSize );
AVP_dword	DSKMAPI DSKM_SignBufferBySign( HDSKM hDSKM, void *pBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pCallBack, void *pCallBackParams, void *pSign, AVP_dword dwSignSize, void **ppvSignedBuffer, AVP_dword *pdwSignedBufferSize );
	
// cl_check.c
AVP_dword DSKMAPI DSKM_CheckFile( HDSKM hDSKM, HDATA hObject, const AVP_char* pszFileName, HDATA hKeysData );
AVP_dword DSKMAPI DSKM_CheckHFile( HDSKM hDSKM, HDSKMFILE hFile, HDATA hKeysData );
AVP_dword DSKMAPI DSKM_CheckObjectByBuffer( HDSKM hDSKM, HDATA hObject, void *pBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pCallBack, void *pCallBackParams, HDATA hKeysData );
AVP_dword DSKMAPI DSKM_CheckObject( HDSKM hDSKM, HDATA hObject, HDATA hKeysData );

// cl_hash.c
AVP_dword	DSKMAPI DSKM_HashObjectByNameImp( HDSKM hDSKM, const AVP_char* pObjectFileName, void **pObjectHash, AVP_dword *pdwObjectHashSize, HDATA hKeysData );
AVP_dword	DSKMAPI DSKM_HashObjectByBufferImp( HDSKM hDSKM, void *pBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pCallBack, void *pCallBackParams, void **pObjectHash, AVP_dword *dwObjectHashSize, HDATA hKeysData );
AVP_dword	DSKMAPI DSKM_HashObjectByHFileImp( HDSKM hDSKM, HDSKMFILE hObjFile, void **pObjectHash, AVP_dword *dwObjectHashSize, HDATA hKeysData );
AVP_dword	DSKMAPI DSKM_HashBufferToHash( HDSKM hDSKM, void *pvPropBuffer, AVP_dword dwPropSize, void *pvObjectHash, AVP_dword dwObjectHashSize );

// cl_hass.c
AVP_dword	DSKMAPI DSKM_HashObjectByNameCheckSignImp( HDSKM hDSKM, const AVP_char* pObjectFileName, void **pObjectHash, AVP_dword *pdwObjectHashSize, HDATA hKeysData, AVP_bool bTreatAnotherSignAsError );
AVP_dword	DSKMAPI DSKM_HashObjectByBufferCheckSignImp( HDSKM hDSKM, void *pBuffer, AVP_dword dwBufferSize, pfnDSKM_GetBuffer_CallBack pCallBack, void *pCallBackParams, void **pObjectHash, AVP_dword *dwObjectHashSize, HDATA hKeysData, AVP_bool bTreatAnotherSignAsError );

// cl_init.c
AVP_dword	DSKMAPI DSKM_InitCriptoLibrary( HDSKM hDSKM );
AVP_dword	DSKMAPI DSKM_DeInitCriptoLibrary( HDSKM hDSKM );

#define DSKM_MEMUTIL

#if defined( DSKM_MEMUTIL )
void* DSKMAPI ds_mcpy( void* dst, const void* src, unsigned size );
void* DSKMAPI ds_mset( void* dst, unsigned fill, unsigned size );
unsigned DSKMAPI ds_slen( const char* src );
int   DSKMAPI ds_mcmp( const void* c1, const void* c2, unsigned size );

#else
#include <memory.h>
#include <string.h>
#define ds_mcpy memcpy
#define ds_mset memset
#define ds_mcmp memcmp
#define ds_slen strlen
#endif



#endif //__dskmi_h__
