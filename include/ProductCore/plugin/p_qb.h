// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Tuesday,  27 June 2006,  16:31 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- plugin_qb.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_qb )
#define __public_plugin_qb
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_posio_sp.h>
#include <ProductCore/iface/i_qbrestorer.h>
#include <ProductCore/iface/i_qb.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_QB  ((tPID)(18))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, QB )
// QB interface implementation
// Short comments --

// constants
#define cCOUNT_PROTECTED               ((tDWORD)(1)) //
#define cCOUNT_INFECTED                ((tDWORD)(2)) //
#define cCOUNT_TOTAL_SIZE              ((tDWORD)(3)) //  --
#define cCOUNT_REFRESH                 ((tDWORD)(0x80000000)) //

// properties
#define plQB_STORAGE_PATH mPROPERTY_MAKE_LOCAL( pTYPE_STRING  , 0x00002000 )
#define plQB_STORAGE_ID   mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )

// message class
#define pmc_QBSTORAGE 0x7a96d0e9 //

	#define pm_QB_IO_DELETE 0xa47443a9 // (2759082921) --

	#define pm_QB_IO_RESTORE 0xa4744310 // (2759082768) --

	#define pm_QB_IO_STORE 0x0518e5af // (85517743) --

	#define pm_QB_IO_MODIFIED 0xfd5ec6a0 // (4250846880) --
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, PosIO_SP )
// PosIO_SP interface implementation
// Short comments -- PosIO with Stored Properties

// constants

// --- Именованные свойства ---
#define cNP_QB_FULLNAME                ((tSTRING)("cNP_QB_FULLNAME")) //
#define cNP_QB_USER                    ((tSTRING)("cNP_QB_USER")) //
#define cNP_QB_MACHINE                 ((tSTRING)("cNP_QB_MACHINE")) //
#define cNP_QB_DOMAIN                  ((tSTRING)("cNP_QB_DOMAIN")) //
#define cNP_QB_RESTORER_PID            ((tSTRING)("cNP_QB_RESTORER_PID")) //
#define cNP_QB_FILE_CREATION_TIME      ((tSTRING)("cNP_QB_FILE_CREATION_TIME")) //
#define cNP_QB_FILE_LAST_ACCESS_TIME   ((tSTRING)("cNP_QB_FILE_LAST_ACCESS_TIME")) //
#define cNP_QB_FILE_LAST_WRITE_TIME    ((tSTRING)("cNP_QB_FILE_LAST_WRITE_TIME")) //
#define cNP_QB_FILE_ATTRIBUTES         ((tSTRING)("cNP_QB_FILE_ATTRIBUTES")) //
#define cNP_QB_FILE_SECURITY           ((tSTRING)("cNP_QB_FILE_SECURITY")) //  --
#define cNP_QB_INFO                    ((tSTRING)("cNP_QB_INFO")) //  --

// -- Алгоритмы хэширования ---
#define cHASH_ALGORITHM_NONE           ((tDWORD)(0)) // Хеширование не производится
#define cHASH_ALGORITHM_XOR            ((tDWORD)(1)) // Алгоритм хеширования - XOR

// - Аттрибуты объекта --
#define fQB_ATTRIBUTE_LOCKED           ((tDWORD)(fATTRIBUTE_HIDDEN)) // Объект заблокирован на запись
#define fQB_ATTRIBUTE_INFECTED         ((tDWORD)(fATTRIBUTE_SYSTEM)) // Объект инфицирован
#define fQB_ATTRIBUTE_PROTECTED        ((tDWORD)(fATTRIBUTE_READONLY)) // Объект защищен

// properties
#define plHASH_ALGORITHM mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plHASH_KEY       mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plHASHED_IO      mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002002 )

// message class
#define pmc_POS_IO 0x9cbb13f8 //

	#define pm_IO_MODIFIED 0xf8a3cb85 // (4171484037) --
// AVP Prague stamp end



// Additional named props
#define cNP_QB_ID                    ((tSTRING)("cNP_QB_ID")) //  --
#define cNP_QB_LINKED_OBJECT_ID        ((tSTRING)("cNP_QB_LINKED_OBJECT_ID")) //  --


// AVP Prague stamp begin( Plugin public definitions, QBRestorer )
// QBRestorer interface implementation
// Short comments --

// errors
#define warnOBJECT_INFO_EXIST                    PR_MAKE_IMP_WARN(PID_QB, 0x001) // 0x10012001,268509185 (1) --
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_qb
// AVP Prague stamp end



