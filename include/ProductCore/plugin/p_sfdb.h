// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  16 February 2005,  11:39 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Integrated Solution
// Sub project -- Integrated Solution Lite
// Purpose     -- Secure Files Database Plugin
// Author      -- mike
// File Name   -- plugin_sfdb.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_sfdb )
#define __public_plugin_sfdb
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_sfdb.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_SFDB  ((tPID)(58006))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MIKE  ((tVID)(70))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, SecureFileDatabase )
// SecureFileDatabase interface implementation
// Short comments -- Secure Files Database

// constants
#define cSFDB_SERIALIZE_CHECKSUM       ((tDWORD)(1)) // Подсчитать контрольную сумму файла.
#define cSFDB_SERIALIZE_CRYPT          ((tDWORD)(2)) // Шифровать данные при сохранении
#define cSFDB_SERIALIZE_COMPACT_ON_SAVE ((tDWORD)(4)) // Сжать базу данных при сохранении
#define cSFDB_AUTOLOAD                 ((tDWORD)(8)) // Автоматически загружать базу при установке свойстве SFDB_FILEPATH
#define cSFDB_AUTOSAVE                 ((tDWORD)(16)) // Автоматически сохранять базу при закрытии объекта, если установлено свойство SFDB_FILEPATH

// errors
#define warnALREADY_EXIST                        PR_MAKE_IMP_WARN(PID_SFDB, 0x001) // 0x1e296001,506028033 (1) --

// properties
#define plSERIALIZE_OPTIONS         mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plUNUSED_RECORDS_LIMIT_TIME mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plDATABASE_SIZE_LIMIT       mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002004 )
#define plSFDB_FILEPATH             mPROPERTY_MAKE_LOCAL( pTYPE_STRING  , 0x00002003 )

// message class
//! #define pmcSFDB 0xba94e5cf //
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_sfdb
// AVP Prague stamp end



