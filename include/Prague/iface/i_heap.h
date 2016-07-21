// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:21 --------
// File Name   -- (null)i_heap.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_heap__2e7dcfc3_2c1a_4da0_b025_c7f6726cb419 )
#define __i_heap__2e7dcfc3_2c1a_4da0_b025_c7f6726cb419
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// Heap interface implementation
// Short comments -- распределение, освобождение памяти
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_HEAP  ((tIID)(64003))
// AVP Prague stamp end

// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hHEAP;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iHeapVtbl;
typedef struct iHeapVtbl iHeapVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cHeap;
	typedef cHeap* hHEAP;
#else
	/*typedef*/ struct tag_hHEAP {
		const iHeapVtbl*   vtbl; // pointer to the "Heap" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	};// *hHEAP;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Heap_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpHeap_Alloc)   ( hHEAP _this, tPTR* result, tDWORD size );               // -- //rus:распределяет регион памяти размером "size" байт.//eng:Allocate "size" bytes from heap;
	typedef   tERROR (pr_call *fnpHeap_Realloc) ( hHEAP _this, tPTR* result, tPTR mem, tDWORD size );     // -- //rus:Изменяет размер ранее распределенного региона пямяти//eng:Changes size of previously allocated block;
	typedef   tERROR (pr_call *fnpHeap_Free)    ( hHEAP _this, tPTR mem );                  // -- //rus:освобождает ранее распределенный регион.//eng:frees previously allocated block. ;
	typedef   tERROR (pr_call *fnpHeap_Size)    ( hHEAP _this, tDWORD* result, tPTR mem );                  // -- //rus:возвращает размер распределенного региона//eng:returns size of previously allocated block;
	typedef   tERROR (pr_call *fnpHeap_Clear)   ( hHEAP _this );                            // -- //rus:освободить все регионы, выделенные этим объектом//eng:Free all alocations made from this heap;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iHeapVtbl {
	fnpHeap_Alloc    Alloc;
	fnpHeap_Realloc  Realloc;
	fnpHeap_Free     Free;
	fnpHeap_Size     Size;
	fnpHeap_Clear    Clear;
}; // "Heap" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Heap_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgHEAP_GRANULARITY  mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001000 )
#define pgHEAP_MOVE_ALLOWED mPROPERTY_MAKE_GLOBAL( pTYPE_BOOL    , 0x00001001 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Heap_Alloc( _this, result, size )         ((_this)->vtbl->Alloc( (_this), result, size ))
	#define CALL_Heap_Realloc( _this, result, mem, size )  ((_this)->vtbl->Realloc( (_this), result, mem, size ))
	#define CALL_Heap_Free( _this, mem )                   ((_this)->vtbl->Free( (_this), mem ))
	#define CALL_Heap_Size( _this, result, mem )           ((_this)->vtbl->Size( (_this), result, mem ))
	#define CALL_Heap_Clear( _this )                       ((_this)->vtbl->Clear( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_Heap_Alloc( _this, result, size )         ((_this)->Alloc( result, size ))
	#define CALL_Heap_Realloc( _this, result, mem, size )  ((_this)->Realloc( result, mem, size ))
	#define CALL_Heap_Free( _this, mem )                   ((_this)->Free( mem ))
	#define CALL_Heap_Size( _this, result, mem )           ((_this)->Size( result, mem ))
	#define CALL_Heap_Clear( _this )                       ((_this)->Clear( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iHeap {
		virtual tERROR pr_call Alloc( tPTR* result, tDWORD size ) = 0; // -- //rus:распределяет регион памяти размером "size" байт.//eng:Allocate "size" bytes from heap
		virtual tERROR pr_call Realloc( tPTR* result, tPTR mem, tDWORD size ) = 0; // -- //rus:Изменяет размер ранее распределенного региона пямяти//eng:Changes size of previously allocated block
		virtual tERROR pr_call Free( tPTR mem ) = 0; // -- //rus:освобождает ранее распределенный регион.//eng:frees previously allocated block.
		virtual tERROR pr_call Size( tDWORD* result, tPTR mem ) = 0; // -- //rus:возвращает размер распределенного региона//eng:returns size of previously allocated block
		virtual tERROR pr_call Clear() = 0; // -- //rus:освободить все регионы, выделенные этим объектом//eng:Free all alocations made from this heap
	};

	struct pr_abstract cHeap : public iHeap, public iObj {

		OBJ_IMPL( cHeap );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hHEAP()   { return (hHEAP)this; }

		tDWORD pr_call get_pgHEAP_GRANULARITY() { return (tDWORD)getDWord(pgHEAP_GRANULARITY); }
		tERROR pr_call set_pgHEAP_GRANULARITY( tDWORD value ) { return setDWord(pgHEAP_GRANULARITY,(tDWORD)value); }

		tBOOL pr_call get_pgHEAP_MOVE_ALLOWED() { return (tBOOL)getBool(pgHEAP_MOVE_ALLOWED); }
		tERROR pr_call set_pgHEAP_MOVE_ALLOWED( tBOOL value ) { return setBool(pgHEAP_MOVE_ALLOWED,(tBOOL)value); }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_heap__37b13798_ae7b_11d4_b757_00d0b7170e50
// AVP Prague stamp end

