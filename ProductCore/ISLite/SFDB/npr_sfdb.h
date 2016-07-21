#ifndef _NPR_SFDB_H_
#define _NPR_SFDB_H_

#include "database.h"
#include "locals.h"

typedef struct tag_SecureFileDatabase {
	PLOCALDATA data;
} * hi_SecureFileDatabase;

typedef hi_SecureFileDatabase hSECUREFILEDB;

#define CALL_SecureFileDatabase_Create( _this )                                                                    SecureFileDatabase_Create( (_this) )
#define CALL_SecureFileDatabase_Close( _this )                                                                     SecureFileDatabase_Close( (_this) )
#define CALL_SecureFileDatabase_SetSerializeOptions( _this, dwOptions )	                                           SecureFileDatabase_SetSerializeOptions( (_this), dwOptions )
#define CALL_SecureFileDatabase_InitNew( _this, dwRecordIDAlg, dwRecordIDSize, dwRecordDataSize, dwDBInitialSize ) SecureFileDatabase_InitNew( (_this), dwRecordIDAlg, dwRecordIDSize, dwRecordDataSize, dwDBInitialSize )
#define CALL_SecureFileDatabase_Load( _this, szFileName )                                                          SecureFileDatabase_Load( (_this), szFileName )
#define CALL_SecureFileDatabase_Save( _this, szFileName )                                                          SecureFileDatabase_Save( (_this), szFileName )
#define CALL_SecureFileDatabase_GetRecord( _this, pRecordID, pRecordData )                                         SecureFileDatabase_GetRecord( (_this), pRecordID, pRecordData )
#define CALL_SecureFileDatabase_UpdateRecord( _this, pRecordID, pRecordData )                                      SecureFileDatabase_UpdateRecord( (_this), pRecordID, pRecordData )
#define CALL_SecureFileDatabase_DeleteRecord( _this, pRecordID )                                                   SecureFileDatabase_DeleteRecord( (_this), pRecordID )
#define CALL_SecureFileDatabase_GetDBInfo( _this, pRecordIDAlg, pRecordIDSize, pRecordDataSize )                   SecureFileDatabase_GetDBInfo( (_this), pRecordIDAlg, pRecordIDSize, pRecordDataSize )

tERROR SecureFileDatabase_Create( hSECUREFILEDB *_this );
tERROR SecureFileDatabase_Close( hSECUREFILEDB _this );
tERROR SecureFileDatabase_SetSerializeOptions( hSECUREFILEDB _this, tDWORD dwOptions );
tERROR SecureFileDatabase_InitNew( hSECUREFILEDB _this, tDWORD dwRecordIDAlg, tDWORD dwRecordIDSize, tDWORD dwRecordDataSize, tDWORD dwIndexSize );
tERROR SecureFileDatabase_Load( hSECUREFILEDB _this, tCHAR* szFileName );
tERROR SecureFileDatabase_Save( hSECUREFILEDB _this, tCHAR* szFileName );
tERROR SecureFileDatabase_GetRecord( hSECUREFILEDB _this, tVOID* pRecordID, tVOID* pRecordData );
tERROR SecureFileDatabase_UpdateRecord( hSECUREFILEDB _this, tVOID* pRecordID, tVOID* pRecordData );
tERROR SecureFileDatabase_DeleteRecord( hSECUREFILEDB _this, tVOID* pRecordID );
tERROR SecureFileDatabase_GetDBInfo( hSECUREFILEDB _this, tDWORD* pRecordIDAlg, tDWORD* pRecordIDSize, tDWORD* pRecordDataSize );

#define cSFDB_SERIALIZE_CHECKSUM       ((tDWORD)(1)) // Подсчитать контрольную сумму файла.
#define cSFDB_SERIALIZE_CRYPT          ((tDWORD)(2)) // Шифровать данные при сохранении
#define cSFDB_SERIALIZE_COMPACT_ON_SAVE ((tDWORD)(4)) // Сжать базу данных при сохранении
#define cSFDB_AUTOLOAD                 ((tDWORD)(8)) // Автоматически загружать базу при установке свойстве SFDB_FILEPATH
#define cSFDB_AUTOSAVE                 ((tDWORD)(16)) // Автоматически сохранять базу при закрытии объекта, если установлено свойство SFDB_FILEPATH

#endif //_NPR_SFDB_H_
