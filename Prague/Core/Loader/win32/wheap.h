// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  18 August 2004,  11:14 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Prague
// Sub project -- Loader
// Purpose     -- Win32 plugin finder/loader
// Author      -- petrovitch
// File Name   -- wheap.c
// Additional info
//    This implementation finds loadable prague plugins in disk folder
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __wheap_c__79cda147_02a1_4dc7_920c_22d628319273 )
#define __wheap_c__79cda147_02a1_4dc7_920c_22d628319273
// AVP Prague stamp end


#include <wincompat.h>

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/plugin/p_win32loader.h>
// AVP Prague stamp end


#include "WHeap_check.h"

// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Plugin identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
//! #define PID_WIN32_PLUGIN_LOADER  ((tPID)(5))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Vendor identifier,  )
//! Section moved to correspondent "plugin_*.c" or "plugin_*.h" file
//! #define VID_PETROVITCH  ((tVID)(100))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// Heap interface implementation
// Short comments -- распределение, освобождение памяти
//    Интерфейс предоставляет услуги по распределению, перераспределению и освобождению регионов памяти.
// AVP Prague stamp end


// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Registration call prototype,  )
	tERROR pr_call Heap_Register( hROOT root );
// AVP Prague stamp end




// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Data structure,  )
// Interface Heap. Inner data structure

	
typedef struct tag_HeapData {
	HANDLE         m_hHeap;           // --
	tDWORD         m_granularity;   // --
	tDWORD         m_move_allowed;  // --
	tBOOL          m_early_initialized; // //rus:инициализация Windows heap во время инициализации объекта
	tINT           m_allocated_count; // --
	tDWORD         m_requested_size;  // --
#if defined(MEM_CHECK)
	tHeapCheckData m_check_data;
#endif
} HeapData;

// to get pointer to the structure HeapData from the hOBJECT obj
#define CUST_TO_HeapData(object)  ((HeapData*)( (((tPTR*)object)[2]) ))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
struct tag_hi_Heap {
	const iHeapVtbl*   vtbl; // pointer to the "Heap" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	HeapData*          data; // pointer to the "Heap" data structure
};
// AVP Prague stamp end

typedef struct tag_hi_Heap* hi_Heap;



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Property table,  )
#define plREQUESTED_HEAP_SIZE mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002000 )
#define plWIN32_HEAP_SIZE     mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plALLOCATED_POINTERS  mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002002 )
#define plEARLY_INITIALIZED   mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002003 )
#define plGLOBAL_ALLOCATED    mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002004 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // wheap_c
// AVP Prague stamp end



