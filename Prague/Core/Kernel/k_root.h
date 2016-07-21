// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Thursday,  01 September 2005,  12:44 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Kernel interfaces
// Sub project -- Not defined
// Purpose     -- Kernel interfaces
// Author      -- petrovitch
// File Name   -- k_root.c
// Additional info
//    Kernel interface implementations
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __k_root_c__3e7d4055_a030_4e01_b15a_cdced0b01017 )
#define __k_root_c__3e7d4055_a030_4e01_b15a_cdced0b01017
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
#include <Prague/prague.h>
#include <Prague/iface/i_iface.h>
#include <Prague/iface/i_plugin.h>
#include <Prague/iface/i_root.h>

#include "kernel.h"
// AVP Prague stamp end



/*

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Root. Inner data structure

typedef struct tag_tRootData {
  HDATA        named_props; // named property list
  tOUTPUT_FUNC output_func; // current output func for system tracer
} tRootData;


// to get pointer to the structure tRootData from the hOBJECT obj
#define CUST_TO_tRootData(object)  ((tRootData*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



*/
//#include <m_utils.h>


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )

// message class
//! #define pmc_KERNEL 0x2ffe4995 // (805194133) -- //rus:сообщения ядра "Праги"//eng:Prague kernel message class
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Root interface implementation
// Short comments -- //rus:корневой интерфейс ядра //eng:kernel root interface
//    //rus:Интерфейс предоставляет базовые возможности ядра Праги, в дополнение к тем, которые определены в наборе системных методов
//      - Регистрация и разрегистрация интерфейсов
//      - Регистрация и разрегистрация идентификаторов свойств, определяемых пользователем
//      - Перебор модулей
//      - Регистрация экспортных функций, определяемых в модуле
//      - Импортирование функций, реализованных в дргих модулях
//      - Трассировка
//    //eng:Interface supplies common kernel facility in adition to System methods
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
  tERROR pr_call Root_Register( hROOT root );
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
#define plMODULE_LOADING mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002000 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_Root {
	const iRootVtbl*   vtbl; // pointer to the "Root" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	tRootData*         data; // pointer to the "Root" data structure
} *hi_Root;

// AVP Prague stamp end



  
// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // k_root_c
// AVP Prague stamp end



