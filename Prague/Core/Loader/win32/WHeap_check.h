#if !defined( _WHeap_check_h )
#define _WHeap_check_h


#if defined(_DEBUG)

	//#define MEM_CHECK      // check for count and amount of allocated objects, and count of objects which are isn't freed 

	#if defined(MEM_CHECK)
		#define MEM_CHECK_ADV  // + list of allocated objects with signature checking and allocator address fixing
	#endif

#endif


// ---
typedef struct tag_hi_Heap* hi_Heap;

#define enter
#define leave

// ---
#if !defined(MEM_CHECK)

	typedef tDWORD tHeapCheckData;

	#define _heap_calc_tagged_size( heap ) (heap->data->m_requested_size)
	#define _heap_check_init( heap )
	#define _heap_check_deinit( heap )
	#define _heap_check_w_output( heap )
	
	#define _heap_check_pre_alloc( heap, size )
	#define _heap_check_post_alloc( heap, ptr, size )

	#define _heap_check_pre_delete( heap, ptr ) (errOK)
	#define _heap_check_post_delete( heap, ptr )

	#define _heap_check_obj( heap, ptr )      (errOK)
	#define _heap_check_obj_size( heap, obj ) (HeapSize(heap->data->m_hHeap,0,obj))
	
	#define _heap_locked_add( pvar, size )
	#define _heap_locked_sub( pvar, size )


#else // !defined(MEM_CHECK)

	#define MEM_CHECK_SKIP_MODULES "prstring;prloader;prkernel" 
	//#define signature(t)  ((tDWORD)((LOWORD((tDWORD)(t))<<16) | LOWORD((tDWORD)((t)->data))))
	#define signature(d)  ((tDWORD)d)
	#define DELETED       (0xdead)


	// ---
	typedef struct tagHeader {
		#ifdef MEM_CHECK_ADV
			tDWORD m_dummy1;
			tDWORD m_dummy2;
		#endif

		tDWORD  m_size;

		#ifdef MEM_CHECK_ADV
			tDWORD            m_sig;
			tDWORD            m_num;
			tDWORD            m_tag;
			tDWORD            m_caller_address;
			struct tagHeader* m_next;
			struct tagHeader* m_prev;
		#endif

	} tHeader;

	// ---
	#ifdef MEM_CHECK_ADV
		typedef struct tad_HeapCheckData {
			HANDLE  m_sync;
			tHeader	m_head;
			tHeader	m_tail;
		} tHeapCheckData;
		tUINT  _heap_calc_tagged_size( hi_Heap heap );
		tVOID  _heap_check_init( hi_Heap heap );
		tVOID  _heap_check_deinit( hi_Heap heap );
	#else
		typedef tDWORD tHeapCheckData;
		#define _heap_calc_tagged_size( heap ) (heap->data->m_requested_size)
		#define _heap_check_init( heap )
		#define _heap_check_deinit( heap )
	#endif

	#define  _heap_check_obj_size( heap, obj ) (((tHeader*)obj)->m_size)

	tVOID    _heap_check_pre_alloc( hi_Heap heap, tDWORD* size );
	tVOID    _heap_check_post_alloc( hi_Heap heap, tPTR* ptr, tDWORD* size );

	tERROR   _heap_check_pre_delete( hi_Heap heap, tPTR* ptr );
	tVOID    _heap_check_post_delete( hi_Heap heap, tPTR ptr );
	
	tERROR   _heap_check_obj( hi_Heap heap, tPTR* ptr );

	tVOID    _heap_locked_add( tDWORD* pvar, tDWORD size );
	tVOID    _heap_locked_sub( tDWORD* pvar, tDWORD size );

#endif // defined(MEM_CHECK)



// ---
typedef struct tag_HeapCheckFileOutParams {
	const tCHAR* m_output_folder;
	HANDLE       m_file_handle;
} tHeapCheckFileOutParams;
tVOID _heap_check_file_output_func( tVOID* params, tVOID* heap, const tCHAR* format, ... );


// ---
typedef struct tag_HeapCheckTraceOutParams {
	hi_Heap heap;
	tINT    prtLevel;
} tHeapCheckTraceOutParams;
tVOID _heap_check_trace_output_func( tVOID* params, tVOID* heap, const tCHAR* format, ... );


// ---
typedef tVOID (*tHeapCheckOutputFunc)( tVOID* params, tVOID* heap, const tCHAR* format, ... );
tVOID _heap_check( tVOID* heap, tHeapCheckOutputFunc output, tVOID* params );

// ---
tVOID _heap_check_file_output( hi_Heap heap, const tCHAR* output_folder );
tVOID _heap_check_trace_output( hi_Heap heap, tDWORD prtLevel );


#endif
		
