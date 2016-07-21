// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 1.0 --------
// -------- Wednesday,  25 August 2004,  16:59 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Integrated Solution
// Sub project -- Integrated Solution Lite
// Purpose     -- Secure Files Database Plugin
// Author      -- mike
// File Name   -- sfdb.c
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __sfdb_c__d07d7bd4_1d9c_4b6e_8fa4_dc2968640beb )
#define __sfdb_c__d07d7bd4_1d9c_4b6e_8fa4_dc2968640beb
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_root.h>
#include <ProductCore/iface/i_sfdb.h>
#include "plugin_sfdb.h"
#include <Prague/iface/i_csect.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
#define cSFDB_SERIALIZE_CHECKSUM       ((tDWORD)(1)) // Подсчитать контрольную сумму файла.
#define cSFDB_SERIALIZE_CRYPT          ((tDWORD)(2)) // Шифровать данные при сохранении
#define cSFDB_SERIALIZE_COMPACT_ON_SAVE ((tDWORD)(4)) // Сжать базу данных при сохранении
#define cSFDB_AUTOLOAD                 ((tDWORD)(8)) // Автоматически загружать базу при установке свойстве SFDB_FILEPATH
#define cSFDB_AUTOSAVE                 ((tDWORD)(16)) // Автоматически сохранять базу при закрытии объекта, если установлено свойство SFDB_FILEPATH
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )

// message class
//! #define pmcSFDB 0xba94e5cf //
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// SecureFileDatabase interface implementation
// Short comments -- Secure Files Database
//    Интерфейс предоставляет сервис по хранению данных в базе, ее быстрому поиску, обновлению и удалению. Каждая запись в база данных идентифицируется по уникальному идентификатору и имеет постоянный набор данных. Размеры  идентификатора и данных записи определяются при создании БД.
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call SecureFileDatabase_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
  // data structure  is declared in O:\Prague\ISLite\SFDB\sfdb.c source file
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
#define plSERIALIZE_OPTIONS         mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plUNUSED_RECORDS_LIMIT_TIME mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plDATABASE_SIZE_LIMIT       mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002004 )
#define plSFDB_FILEPATH             mPROPERTY_MAKE_LOCAL( pTYPE_STRING  , 0x00002003 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // sfdb_c
// AVP Prague stamp end



