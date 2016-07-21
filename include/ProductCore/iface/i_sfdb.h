// AVP Prague stamp begin( Interface header,  )
// -------- Wednesday,  16 February 2005,  11:38 --------
// File Name   -- (null)i_sfdb.c
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_sfdb__4cfcdce3_1754_4032_b326_52f7832570fb )
#define __i_sfdb__4cfcdce3_1754_4032_b326_52f7832570fb
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end


#include <Prague/pr_vtbl.h>


// AVP Prague stamp begin( Interface comment,  )
// SecureFileDatabase interface implementation
// Short comments -- Secure Files Database
//    Интерфейс предоставляет сервис по хранению данных в базе, ее быстрому поиску, обновлению и удалению. Каждая запись в база данных идентифицируется по уникальному идентификатору и имеет постоянный набор данных. Размеры  идентификатора и данных записи определяются при создании БД.
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_SECUREFILEDB  ((tIID)(58001))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )

// message class
#define pmcSFDB 0xba94e5cf //

	#define pmUPDATE_CLUSTER_FULL 0x5fdb5fdc // (1608212444) --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hSECUREFILEDB;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iSecureFileDatabaseVtbl;
typedef struct iSecureFileDatabaseVtbl iSecureFileDatabaseVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cSecureFileDatabase;
	typedef cSecureFileDatabase* hSECUREFILEDB;
#else
	typedef struct tag_hSECUREFILEDB {
		const iSecureFileDatabaseVtbl* vtbl; // pointer to the "SecureFileDatabase" virtual table
		const iSYSTEMVtbl*             sys;  // pointer to the "SYSTEM" virtual table
	} *hSECUREFILEDB;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( SecureFileDatabase_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpSecureFileDatabase_InitNew)      ( hSECUREFILEDB _this, tDWORD dwRecordIDAlg, tDWORD dwRecordIDSize, tDWORD dwRecordDataSize, tDWORD dwDBInitialSize ); // -- Создает новую базу данных;
	typedef   tERROR (pr_call *fnpSecureFileDatabase_Load)         ( hSECUREFILEDB _this, tCHAR* szFileName ); // -- Загружает базу данных из файла;
	typedef   tERROR (pr_call *fnpSecureFileDatabase_Save)         ( hSECUREFILEDB _this, tCHAR* szFileName ); // -- Сохраняет базу данных в файл;
	typedef   tERROR (pr_call *fnpSecureFileDatabase_GetRecord)    ( hSECUREFILEDB _this, tVOID* pRecordID, tVOID* pRecordData ); // -- Получает данные из записи;
	typedef   tERROR (pr_call *fnpSecureFileDatabase_UpdateRecord) ( hSECUREFILEDB _this, tVOID* pRecordID, tVOID* pRecordData ); // -- Обновляет данные записи;
	typedef   tERROR (pr_call *fnpSecureFileDatabase_DeleteRecord) ( hSECUREFILEDB _this, tVOID* pRecordID ); // -- Удаляет указанную запись вместе с ее данными;
	typedef   tERROR (pr_call *fnpSecureFileDatabase_GetDBInfo)    ( hSECUREFILEDB _this, tDWORD* pRecordIDAlg, tDWORD* pRecordIDSize, tDWORD* pRecordDataSize ); // -- Получает информацию о конфигурации базы данных;
	typedef   tERROR (pr_call *fnpSecureFileDatabase_CleanDB)      ( hSECUREFILEDB _this );         // -- Очищает базу;
	typedef   tERROR (pr_call *fnpSecureFileDatabase_EnumRecords)  ( hSECUREFILEDB _this, tDWORD nRecordNum, tVOID* pRecordID, tVOID* pRecordData ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iSecureFileDatabaseVtbl {
	fnpSecureFileDatabase_InitNew       InitNew;
	fnpSecureFileDatabase_Load          Load;
	fnpSecureFileDatabase_Save          Save;
	fnpSecureFileDatabase_GetRecord     GetRecord;
	fnpSecureFileDatabase_UpdateRecord  UpdateRecord;
	fnpSecureFileDatabase_DeleteRecord  DeleteRecord;
	fnpSecureFileDatabase_GetDBInfo     GetDBInfo;
	fnpSecureFileDatabase_CleanDB       CleanDB;
	fnpSecureFileDatabase_EnumRecords   EnumRecords;
}; // "SecureFileDatabase" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( SecureFileDatabase_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_SecureFileDatabase_InitNew( _this, dwRecordIDAlg, dwRecordIDSize, dwRecordDataSize, dwDBInitialSize )                        ((_this)->vtbl->InitNew( (_this), dwRecordIDAlg, dwRecordIDSize, dwRecordDataSize, dwDBInitialSize ))
	#define CALL_SecureFileDatabase_Load( _this, szFileName )                                                                                 ((_this)->vtbl->Load( (_this), szFileName ))
	#define CALL_SecureFileDatabase_Save( _this, szFileName )                                                                                 ((_this)->vtbl->Save( (_this), szFileName ))
	#define CALL_SecureFileDatabase_GetRecord( _this, pRecordID, pRecordData )                                                                ((_this)->vtbl->GetRecord( (_this), pRecordID, pRecordData ))
	#define CALL_SecureFileDatabase_UpdateRecord( _this, pRecordID, pRecordData )                                                             ((_this)->vtbl->UpdateRecord( (_this), pRecordID, pRecordData ))
	#define CALL_SecureFileDatabase_DeleteRecord( _this, pRecordID )                                                                          ((_this)->vtbl->DeleteRecord( (_this), pRecordID ))
	#define CALL_SecureFileDatabase_GetDBInfo( _this, pRecordIDAlg, pRecordIDSize, pRecordDataSize )                                          ((_this)->vtbl->GetDBInfo( (_this), pRecordIDAlg, pRecordIDSize, pRecordDataSize ))
	#define CALL_SecureFileDatabase_CleanDB( _this )                                                                                          ((_this)->vtbl->CleanDB( (_this) ))
	#define CALL_SecureFileDatabase_EnumRecords( _this, nRecordNum, pRecordID, pRecordData )                                                  ((_this)->vtbl->EnumRecords( (_this), nRecordNum, pRecordID, pRecordData ))
#else // if !defined( __cplusplus )
	#define CALL_SecureFileDatabase_InitNew( _this, dwRecordIDAlg, dwRecordIDSize, dwRecordDataSize, dwDBInitialSize )                        ((_this)->InitNew( dwRecordIDAlg, dwRecordIDSize, dwRecordDataSize, dwDBInitialSize ))
	#define CALL_SecureFileDatabase_Load( _this, szFileName )                                                                                 ((_this)->Load( szFileName ))
	#define CALL_SecureFileDatabase_Save( _this, szFileName )                                                                                 ((_this)->Save( szFileName ))
	#define CALL_SecureFileDatabase_GetRecord( _this, pRecordID, pRecordData )                                                                ((_this)->GetRecord( pRecordID, pRecordData ))
	#define CALL_SecureFileDatabase_UpdateRecord( _this, pRecordID, pRecordData )                                                             ((_this)->UpdateRecord( pRecordID, pRecordData ))
	#define CALL_SecureFileDatabase_DeleteRecord( _this, pRecordID )                                                                          ((_this)->DeleteRecord( pRecordID ))
	#define CALL_SecureFileDatabase_GetDBInfo( _this, pRecordIDAlg, pRecordIDSize, pRecordDataSize )                                          ((_this)->GetDBInfo( pRecordIDAlg, pRecordIDSize, pRecordDataSize ))
	#define CALL_SecureFileDatabase_CleanDB( _this )                                                                                          ((_this)->CleanDB( ))
	#define CALL_SecureFileDatabase_EnumRecords( _this, nRecordNum, pRecordID, pRecordData )                                                  ((_this)->EnumRecords( nRecordNum, pRecordID, pRecordData ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iSecureFileDatabase {
		virtual tERROR pr_call InitNew( tDWORD dwRecordIDAlg, tDWORD dwRecordIDSize, tDWORD dwRecordDataSize, tDWORD dwDBInitialSize ) = 0; // -- Создает новую базу данных
		virtual tERROR pr_call Load( tCHAR* szFileName ) = 0; // -- Загружает базу данных из файла
		virtual tERROR pr_call Save( tCHAR* szFileName ) = 0; // -- Сохраняет базу данных в файл
		virtual tERROR pr_call GetRecord( tVOID* pRecordID, tVOID* pRecordData ) = 0; // -- Получает данные из записи
		virtual tERROR pr_call UpdateRecord( tVOID* pRecordID, tVOID* pRecordData ) = 0; // -- Обновляет данные записи
		virtual tERROR pr_call DeleteRecord( tVOID* pRecordID ) = 0;
		virtual tERROR pr_call GetDBInfo( tDWORD* pRecordIDAlg, tDWORD* pRecordIDSize, tDWORD* pRecordDataSize ) = 0; // -- Получает информацию о конфигурации базы данных
		virtual tERROR pr_call CleanDB() = 0; // -- Очищает базу
		virtual tERROR pr_call EnumRecords( tDWORD nRecordNum, tVOID* pRecordID, tVOID* pRecordData ) = 0;
	};

	struct pr_abstract cSecureFileDatabase : public iSecureFileDatabase, public iObj {

		OBJ_IMPL( cSecureFileDatabase );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hSECUREFILEDB()   { return (hSECUREFILEDB)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_sfdb__4cfcdce3_1754_4032_b326_52f7832570fb
// AVP Prague stamp end



