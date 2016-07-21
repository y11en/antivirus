#ifndef _NPR_ICHECKER2_H_
#define _NPR_ICHECKER2_H_

#if defined (__unix__)
#include <compat.h>
#else
#include <windows.h>
#endif
#include "npr.h"
#include "npr_hash.h"
#include "npr_io.h"
#include "../sfdb/npr_sfdb.h"

struct tag_IChecker
{
	hSECUREFILEDB hDataBase;         // База данных, содержащая информацию ассоциированную с хешами файлов
	tCHAR         szDBFileName[260]; // Путь к файлу базы данных
	tDWORD        dwUserDataSize;    // Размер ассоциированных данных сохраняемых в базе вместе с хешем. В байтах.
	tDWORD        dwUserVersion;     // Версия (тип) ассоциированных данных, хранящихся в базе данных
};
typedef struct tag_IChecker * hIChecker;

#define cICHECKER_ARCHIVES_OFF ((tDWORD)(1)) // Отключает подсчет хеша для архивов

#if !defined (__unix__)
#ifdef NPR_ICHECKER2_EXPORTS
#define NPR_ICHECKER2_API __declspec(dllexport)
#else
#define NPR_ICHECKER2_API __declspec(dllimport)
#endif
#else
#define NPR_ICHECKER2_API
#endif

#if defined (__cplusplus)
extern "C" {
#endif
NPR_ICHECKER2_API tERROR IChecker_Create( hIChecker *_this, char * szDBFileName, tDWORD dwUserDataSize, tDWORD dwUserVersion );
NPR_ICHECKER2_API tERROR IChecker_Close( hIChecker _this );
NPR_ICHECKER2_API tERROR IChecker_GetStatus( hIChecker _this, tDWORD* result, tQWORD* pHash, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize );
NPR_ICHECKER2_API tERROR IChecker_GetStatusEx( hIChecker _this, tDWORD* result, tQWORD* pHash, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize, tDWORD dwFlags );
NPR_ICHECKER2_API tERROR IChecker_UpdateStatus( hIChecker _this, tQWORD* pHash, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize );
NPR_ICHECKER2_API tERROR IChecker_DeleteStatus( hIChecker _this, tQWORD* pHash );
NPR_ICHECKER2_API tERROR IChecker_FlushStatusDB( hIChecker _this );
NPR_ICHECKER2_API tERROR IChecker_ClearStatusDB( hIChecker _this );
#if defined (__cplusplus)
}
#endif

#endif //_ICHECKER2N_H_
