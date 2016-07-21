// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Friday,  11 February 2005,  15:57 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- AV Engine
// Purpose     -- Менеджер антивирусной обработки (Anti-Virus Processing Manager).//eng:Anti-Virus Processing Manager.
// Author      -- Graf
// File Name   -- plugin_avpmgr.c
// Additional info
//    Модуль AvpMgr обеспечиает антивирусную функциональность верхнего уровня для приложений.
//    //eng:
//    AvpMgr plugin - Anti-Virus Processing Manager - represents AV Engine to Applications.
//
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_avpmgr )
#define __public_plugin_avpmgr
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <AV/iface/i_engine.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_AVPMGR  ((tPID)(62002))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_GRAF  ((tVID)(66))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Engine )
// Engine interface implementation
// Short comments -- Базовый интерфейс обработки объектов.//eng:Generic interface for object processing.

// constants
#define ENGINE_ICHECK_USE_DATABASE_DAYS ((tDWORD)(-1)) //  --

// properties
#define plENGINE_STACK_PROBE                mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plENGINE_ICHECKER                   mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002001 )
#define plENGINE_DB_TIME_DWORD              mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002002 )
#define plENGINE_QUARANTINE_DAYS_DWORD      mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002003 )
#define plENGINE_LONG_QUARANTINE_DAYS_DWORD mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002005 )
#define plENGINE_ICHECKER_STREAM            mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002004 )
#define plENGINE_ICHECKER_INIT_NUMBER       mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002006 )

// message class
//    msg_id == тип Действия
//    send-point == обрабатываемый объект. (зависит от типа)
//    ctx == hENGINE
//    В буффере - tDWORD Класс действия.
//
//    Посылается только в том случае, если подходит по маске класса. См. pm_ENGINE_IO_OPTIONS_ACTION_CLASS_MASK.
//
//    Если возаратить errENGINE_ACTION_SKIP - действие будет пропущено.
//! #define pmc_ENGINE_ACTION 0xffe6a144 // Сообщения Action- процессора.

// message class
//    Испрользуется во время инициализации.
//    В параметре ctx - hENGINE.
//! #define pmc_ENGINE_OPTIONS 0x84538783 // Запрос основных настроек.//eng:General engine options request.

// message class
//    Usually hIO in obj parameter.
//! #define pmc_ENGINE_IO 0x688c733f // Main notification class for processing.

// message class
//    Сообщения этого класса посылаются во время инициализации ENGINE и вызова метода LoadConfiguration.
//! #define pmc_ENGINE_LOAD 0x7f067b36 // Класс сообщений о ходе загрузки ENGINE и конфигурации

// message class
//! #define pmc_ENGINE_PROCESS 0xd27b6592 // Сообщения о прохождении функций Process() и ICheck()

// message class
#define pmc_AVPMGR_INTERNAL 0x9c7ee77f // Внутренние сообщения AvpMgr

	#define pm_AVPMGR_INTERNAL_IO_PROCESS 0x00002000 // (8192) -- Команда запроса на сканирование от поддвижка
	// context comment
	//    Engine
	//
	// data content comment
	//    не используется.

	#define pm_AVPMGR_INTERNAL_IO_PROCESS_ARCHIVE 0x00002001 // (8193) -- Команда запроса на обработку OS от поддвижка
	// context comment
	//    Engine
	//
	// data content comment
	//    tDWORD - смещение от начала io

	#define pm_AVPMGR_INTERNAL_IO_OS_OFFSET_REGISTER 0x00002002 // (8194) -- Exclude offset from futher OS processing
	// context comment
	//    engine
	//
	// data content comment
	//    tDWORD - смещение для регистрации

	#define pm_AVPMGR_INTERNAL_IO_OS_OFFSET_FLUSH 0x00002003 // (8195) -- Flush all offsets offset from futher OS processing
	// context comment
	//    engine
	//
	// data content comment
	//    не используется
	//

	#define pm_AVPMGR_INTERNAL_IO_HASH_FRAME_CHECK 0x00002004 // (8196) --

	#define pm_AVPMGR_INTERNAL_HASH_CONT_DELETE 0x00002005 // (8197) --

	#define pm_AVPMGR_INTERNAL_IO_HASH_FRAME_LEAVE 0x00002006 // (8198) --

	#define pm_AVPMGR_INTERNAL_IO_REGISTER_FOR_POSTPROCESS 0x00002007 // (8199) --
	// context comment
	//    handle of IO to be registered
	//

	#define pm_AVPMGR_INTERNAL_IO_DISINFECT_REQUEST 0x00002008 // (8200) --

	#define pm_AVPMGR_INTERNAL_IO_SECTOR_OVERWRITE_REQUEST 0x00002009 // (8201) --

	#define pm_AVPMGR_INTERNAL_IO_REGISTER_PASSWORD 0x0000200a // (8202) --
// AVP Prague stamp end



#define ECLSID_AVPMGR            PID_AVPMGR //0xADAD50a1 


#pragma pack(push,1)

typedef struct tag_IC_DATA{
	tDWORD dwAVBTimeFirstCheck; //prague time rounded to minutes
	tDWORD dwAVBTimeLastCheck;  //prague time rounded to minutes
	tQWORD qwLevel;
}IC_DATA;

#pragma pack(pop)

#define AVPMGR_ICDATA_SIZE       sizeof(IC_DATA)

#define IC_BIT_RANGE_MFLAGS      LONG_LONG_CONST(0x00000000FFFFFFFF)
#define IC_BIT_RANGE_EMUL_VAL    LONG_LONG_CONST(0x000000FF00000000)
#define IC_BIT_RANGE_EMUL_ON     LONG_LONG_CONST(0x0000010000000000)
#define IC_BIT_RANGE_EXECUTED    LONG_LONG_CONST(0x0000020000000000)
#define IC_BIT_RANGE_TRUSTED     LONG_LONG_CONST(0x0040000000000000)
#define IC_BIT_RANGE_MSTRUSTED   LONG_LONG_CONST(0x0080000000000000)
#define IC_BIT_RANGE_VERDICTS    LONG_LONG_CONST(0xFF00000000000000)

// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_avpmgr
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export methods,  )
#ifdef IMPEX_TABLE_CONTENT
{ (tDATA)&Engine_ICGetData, PID_AVPMGR, 0xf339f0dbl },
{ (tDATA)&Engine_ICFreeData, PID_AVPMGR, 0x19092138l },
{ (tDATA)&Engine_ICCheckData, PID_AVPMGR, 0x483ce41bl },
{ (tDATA)&Engine_ICUpdate, PID_AVPMGR, 0x5a6f4d45l },
{ (tDATA)&Engine_SetWriteAccess, PID_AVPMGR, 0xe61849ddl },
{ (tDATA)&Engine_AddReopenData, PID_AVPMGR, 0x544f5723l },

#elif defined(IMPEX_EXPORT_LIB) || defined(IMPEX_IMPORT_LIB)
#include <Prague/iface/impexhlp.h>

IMPEX_DECL tERROR IMPEX_FUNC(Engine_ICGetData)( hENGINE p_this, hOBJECT p_obj, tPID p_pid, tPTR* p_icdata_ptr, tCHAR* p_hash_init_buf, tDWORD p_hash_init_size )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(Engine_ICFreeData)( hENGINE p_this, tPTR p_icdata )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(Engine_ICCheckData)( hENGINE p_this, tPTR p_icdata, tQWORD p_bit_compare_data, tDWORD p_mandatory_scan_days )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(Engine_ICUpdate)( hENGINE p_this, hOBJECT p_obj, tPID p_pid, tPTR p_icdata, tQWORD p_bit_compare_data, tBOOL p_delete_status )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(Engine_SetWriteAccess)( hOBJECT p_obj, hOBJECT* p_deletable_parent )IMPEX_INIT;
IMPEX_DECL tERROR IMPEX_FUNC(Engine_AddReopenData)( hIO p_io, tPTR p_reopen_data, tDWORD p_user_data )IMPEX_INIT;
#endif
// AVP Prague stamp end



