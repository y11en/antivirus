// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Monday,  22 November 2004,  14:34 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- msoe
// Sub project -- msoe
// Purpose     -- плагин для работы с Outlook Express
// Author      -- Mezhuev
// File Name   -- msoe_objptr.c
// Additional info
//    Данный плагин предназначен для работы со следующими объектами Outlook Express:
//    - почтовая система (идентифицируется по Identity, если не указан, то успользуется текущий)
//    - база сообщений в формате dbx.
//    - сообщение в формате RFC822 (eml файл)
//    - тела и аттачменты сообщения (message part)
//
//    Интерфейсы данного плагина не поддерживают идентификацию объектов по строке!!!
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __msoe_objptr_c__fd4211bb_fe36_4514_a05f_f6071758b2a6 )
#define __msoe_objptr_c__fd4211bb_fe36_4514_a05f_f6071758b2a6
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin identifier,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Vendor identifier,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_os.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <Extract/plugin/p_msoe.h>
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface comment,  )
// ObjPtr interface implementation
// Short comments -- pointer to the object
//    Интерфейс позволяет иерархическую навигацию по объектам Outlook Express. Корнем иерархии является объект инициализирующий OS. Объект может быть создан следующими способами:
//    - вызов ObjectCreate с родителем OS.
//    - вызов PtrCreate интерфейса OS.
// AVP Prague stamp end

// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call ObjPtr_Register( hROOT root );
// AVP Prague stamp end

// AVP Prague stamp begin( Data structure,  )
// Interface ObjPtr. Inner data structure

typedef struct tag_OEEnum_Data 
{
	tObjPtrState ptrState; // --
	tBOOL        fFolder;  // --
	hOS          hOS;      // --
	tDWORD       Origin;   // --
	PROEObject * ptrclass; // --
	PROEObject * objclass; // --
} OEEnum_Data;


// to get pointer to the structure OEEnum_Data from the hOBJECT obj
#define CUST_TO_OEEnum_Data(object)  ((OEEnum_Data*)( (((tPTR*)object)[2]) ))
  // data structure OEEnum_Data is declared in O:\Praque\MSOE\msoe_objptr.c source file
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_ObjPtr 
{
	const iObjPtrVtbl* vtbl; // pointer to the "ObjPtr" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	OEEnum_Data*       data; // pointer to the "ObjPtr" data structure
} *hi_ObjPtr;

// AVP Prague stamp end

// AVP Prague stamp begin( Property table,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // msoe_objptr_c
// AVP Prague stamp end

